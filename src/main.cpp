#include "main.h"
using namespace lemlib;
using namespace pros;

pros::Controller controller(pros::E_CONTROLLER_MASTER);

pros::MotorGroup left_mg({1, -2, 3, 4});
pros::MotorGroup right_mg({-5, 6, 7, 8});

pros::Motor intake_mtr(9);

pros::Imu inertial(10);
pros::Rotation horizontal_rotation(11); // horizontal
// pros::Rotation vertical_rotation(12); // vertical

pros::adi::DigitalOut mogoClamp('A', false);
pros::adi::DigitalOut intakePiston('B', true);

lemlib::Drivetrain drivetrain(&left_mg, // left motor group
                              &right_mg, 10, lemlib::Omniwheel::OLD_325, 480, 5

                              // horizontal drift is 2 (for now)
);

lemlib::TrackingWheel horizontal_tracking_wheel(&horizontal_rotation,
                                                lemlib::Omniwheel::OLD_275_HALF,
                                                -5.75);
// lemlib::TrackingWheel vertical_tracking_wheel(&vertical_rotation,
// lemlib::Omniwheel::OLD_275_HALF, -5.75);

lemlib::OdomSensors
    sensors(nullptr, // null
            nullptr,
            &horizontal_tracking_wheel, // horizontal tracking wheel
            nullptr, &inertial);

// lateral PID controller
lemlib::ControllerSettings
    lateral_controller(10,  // proportional gain (kP)
                       0,   // integral gain (kI)
                       3,   // derivative gain (kD)
                       3,   // anti windup
                       1,   // small error range, in inches
                       100, // small error range timeout, in milliseconds
                       3,   // large error range, in inches
                       500, // large error range timeout, in milliseconds
                       20   // maximum acceleration (slew)
    );

// angular PID controller
lemlib::ControllerSettings
    angular_controller(2,   // proportional gain (kP)
                       0,   // integral gain (kI)
                       10,  // derivative gain (kD)
                       3,   // anti windup
                       1,   // small error range, in degrees
                       100, // small error range timeout, in milliseconds
                       3,   // large error range, in degrees
                       500, // large error range timeout, in milliseconds
                       0    // maximum acceleration (slew)
    );

// input curve for throttle input during driver control
lemlib::ExpoDriveCurve
    throttle_curve(3,    // joystick deadband out of 127
                   10,   // minimum output where drivetrain will move out of 127
                   1.019 // expo curve gain
    );

// input curve for steer input during driver control
lemlib::ExpoDriveCurve
    steer_curve(3,    // joystick deadband out of 127
                10,   // minimum output where drivetrain will move out of 127
                1.019 // expo curve gain
    );

// create the chassis
lemlib::Chassis chassis(drivetrain, lateral_controller, angular_controller,
                        sensors, &throttle_curve, &steer_curve);

// Define autonomous routines
void close_side_auto() { controller.rumble(".-"); }

void far_side_auto() { controller.rumble("-."); }

void skills_auto() { controller.rumble("..."); }

// Enum to track selected autonomous
enum class AutoMode { CLOSE_SIDE, FAR_SIDE, SKILLS, OFF };

// Global variable to track selected autonomous
AutoMode selected_auto = AutoMode::OFF;

// Callback functions for button presses
void on_left_button() {
  selected_auto = AutoMode::CLOSE_SIDE;
  pros::lcd::clear_line(5);
  pros::lcd::set_text(5, "Selected: Close Side");
}

void on_center_button() {
  selected_auto = AutoMode::FAR_SIDE;
  pros::lcd::clear_line(5);
  pros::lcd::set_text(5, "Selected: Far Side");
}

void on_right_button() {
  selected_auto = AutoMode::SKILLS;
  pros::lcd::clear_line(5);
  pros::lcd::set_text(5, "Selected: Skills");
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  pros::lcd::initialize(); // initialize brain screen
  chassis.calibrate();     // calibrate sensors

  // Set up buttons on the brain screen with labels
  pros::lcd::set_text(0, "Select Autonomous:");
  pros::lcd::set_text(1, "Left: Close Side");
  pros::lcd::set_text(2, "Center: Far Side");
  pros::lcd::set_text(3, "Right: Skills");

  pros::lcd::register_btn0_cb(on_left_button);
  pros::lcd::register_btn1_cb(on_center_button);
  pros::lcd::register_btn2_cb(on_right_button);

  // Default selection
  on_left_button();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  // Turn off all motors
  left_mg.move(0);
  right_mg.move(0);
  intake_mtr.move(0);
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
  switch (selected_auto) {
  case AutoMode::CLOSE_SIDE:
    close_side_auto();
    break;
  case AutoMode::FAR_SIDE:
    far_side_auto();
    break;
  case AutoMode::SKILLS:
    skills_auto();
    break;
  case AutoMode::OFF:
    // Do nothing or run a default routine
    break;
  }
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
  while (true) {
    int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    int rightY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);

    // Calculate average temperatures
    double avg_left_temp = (left_mg.get_temperature(0) + left_mg.get_temperature(1) + left_mg.get_temperature(2)) / 3.0;
    double avg_right_temp = (right_mg.get_temperature(0) + right_mg.get_temperature(1) + right_mg.get_temperature(2)) / 3.0;
    double intake_temp = intake_mtr.get_temperature();

    // Update LCD with temperature information
    pros::lcd::clear();
    pros::lcd::print(0, "Left Temp: %.1f°C", avg_left_temp);
    pros::lcd::print(1, "Right Temp: %.1f°C", avg_right_temp);
    pros::lcd::print(2, "Intake Temp: %.1f°C", intake_temp);

    // Intake controls
    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
      intake_mtr.move(127); // Run intake at full speed
    } else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
      intake_mtr.move(-127); // Run intake in reverse at full speed
    } else {
      intake_mtr.move(0); // Stop intake when no button is pressed
    }

    // Toggle reverse drive
    static bool reverse_drive = false;
    if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
      reverse_drive = !reverse_drive;
    }

    // Apply reverse drive logic to chassis control and update LCD color
    if (reverse_drive) {
      chassis.tank(-rightY, -leftY);
      pros::lcd::set_text(0, "REVERSE"); // Indicate reverse drive mode
    } else {
      chassis.tank(leftY, rightY);
      pros::lcd::set_text(0, "FORWARD"); // Indicate forward drive mode
    }

    // Mogo clamp control
    static bool mogo_clamped = false;
    if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
      mogo_clamped = !mogo_clamped;
      mogoClamp.set_value(mogo_clamped); // Toggle mogo clamp
    }

    // Intake up/down control
    static bool ip_extended = true; // intake piston extended = down state
    if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) {
      ip_extended = !ip_extended;
      intakePiston.set_value(ip_extended); // Toggle intake position
    }

    pros::delay(20); // Run for 20 ms then update
  }
}