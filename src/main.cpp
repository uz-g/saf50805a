#include "main.h"
#include "globals.h"
#include "robot/robot.hpp"
#include "screen/gui.hpp"
#include "autonomous/routines.hpp"

using namespace lemlib;
using namespace pros;

// Define autonomous routines
void close_side_auto() { controller.rumble(".-"); }
void far_side_auto() { controller.rumble("-."); }
void skills_auto() { controller.rumble("..."); }

// Initialization function
void initialize() {
  pros::lcd::initialize(); // initialize brain screen
  pros::lcd::set_text(1, "Initializing...");
  robot::initChassis();    // initialize and calibrate chassis

  gui::initializeGUI(); // initialize GUI
}

// Disabled function
void disabled() {
  // Turn off all motors
  left_mg.move(0);
  right_mg.move(0);
  intake_mtr.move(0);
  
}

// Competition initialize function
void competition_initialize() {}

// Autonomous function
void autonomous() {
  auto start_time = std::chrono::high_resolution_clock::now();

  auton_routines::runSelectedAutonomous();

  // Stop the timer
  auto end_time = std::chrono::high_resolution_clock::now();

  // Calculate the duration in seconds
  auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(
      end_time - start_time);

  // Log the duration
  std::string message =
      "Autonomous completed in " + std::to_string(duration.count()) + " s";

  // Display the duration on the LCD
  pros::lcd::clear_line(0);
  pros::lcd::print(0, "Auto time: %.2f s", duration.count());

  // Optionally, display on controller
  controller.print(0, 0, "Auto: %.2f s", duration.count());
}

// Operator control function
void opcontrol() {
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(120, 0, 255),
                            LV_PART_MAIN); // Set screen to purple
  controller.rumble(".");
  // Initialize a timer variable
  std::chrono::steady_clock::time_point start_time =
      std::chrono::steady_clock::now();
  bool timer_marked = false;
  bool reverse_drive = false;

  while (true) {
    // Check if 1 minute 30 seconds have passed
    if (!timer_marked) {
      auto current_time = std::chrono::steady_clock::now();
      auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(
                              current_time - start_time)
                              .count();
      if (elapsed_time >= 90) { //  1 minute 30 seconds of 1m 45s match
        timer_marked = true;
        controller.rumble(".."); // Vibrate controller when 15 seconds left
      }
    }

    int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    int rightY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);

    // Calculate average temperatures
    double avg_left_temp =
        (left_mg.get_temperature(0) + left_mg.get_temperature(1) +
         left_mg.get_temperature(2)) /
        3.0;
    double avg_right_temp =
        (right_mg.get_temperature(0) + right_mg.get_temperature(1) +
         right_mg.get_temperature(2)) /
        3.0;
    double intake_temp = intake_mtr.get_temperature();

    // Update LCD with temperature information
    pros::lcd::clear();
    pros::lcd::print(0, "Left Temp: %.1f°C", avg_left_temp);
    pros::lcd::print(1, "Right Temp: %.1fC", avg_right_temp);
    pros::lcd::print(2, "Intake Temp: %.1f°C", intake_temp);

    // Intake controls
    bool intakeIn = controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1);
    bool intakeOut = controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2);
    robot::updateIntake(intakeIn, intakeOut);

    // Toggle reverse drive
    if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
      reverse_drive = !reverse_drive;
      if (reverse_drive) {
        lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(255, 0, 0),
                                  LV_PART_MAIN); // Set screen to red
      } else {
        lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(120, 0, 255),
                                  LV_PART_MAIN); // Set screen to purple
      }
    }

    // Apply drive control
    robot::updateDrive(leftY, rightY, reverse_drive);

    // Update LCD with drive direction
    pros::lcd::set_text(0, reverse_drive ? "REVERSE" : "FORWARD");

    // Mogo clamp control
    if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
      robot::toggleMogoClamp();
    }

    gui::updateGUI(); // Update GUI if needed

    pros::delay(20); // Run every 20 ms (50 Hz)
  }
}