#include "main.h"

using namespace lemlib;
using namespace pros;

// Initialize the controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// Define motor groups for left and right sides of the drivetrain
pros::MotorGroup left_mg({1, -2, 3, 4});
pros::MotorGroup right_mg({-5, 6, 7, 8});

// Define intake motor
pros::Motor intake_mtr(9);

// Initialize sensors
pros::Imu inertial(10);
pros::Rotation horizontal_rotation(11); // horizontal tracking wheel

// Initialize pneumatic components
pros::adi::DigitalOut mogoClamp('A', false);
pros::adi::DigitalOut intakePiston('B', true);
// Configure drivetrain
lemlib::Drivetrain drivetrain(&left_mg, &right_mg, 10,
                              lemlib::Omniwheel::OLD_325, 480, 5);

// Configure tracking wheels
lemlib::TrackingWheel horizontal_tracking_wheel(&horizontal_rotation,
                                                lemlib::Omniwheel::OLD_275_HALF,
                                                -5.75);

// Configure odometry sensors
lemlib::OdomSensors sensors(nullptr, nullptr, &horizontal_tracking_wheel,
                            nullptr, &inertial);

// Configure lateral PID controller
lemlib::ControllerSettings lateral_controller(10, 0, 3, 3, 1, 100, 3, 500, 20);

// Configure angular PID controller
lemlib::ControllerSettings angular_controller(2, 0, 10, 3, 1, 100, 3, 500, 0);

// Configure input curves for driver control
lemlib::ExpoDriveCurve throttle_curve(3, 10, 1.019);
lemlib::ExpoDriveCurve steer_curve(3, 10, 1.019);

// Create the chassis
lemlib::Chassis chassis(drivetrain, lateral_controller, angular_controller,
                        sensors, &throttle_curve, &steer_curve);

// Define autonomous routines
void close_side_auto() { controller.rumble(".-"); }
void far_side_auto() { controller.rumble("-."); }
void skills_auto() { controller.rumble("..."); }

// Enum for autonomous mode selection
enum class AutoMode { OFF, CLOSE_SIDE, FAR_SIDE, SKILLS };
AutoMode selected_auto = AutoMode::OFF;

// Button map for LVGL GUI
static const char *btnmMap[] = {"far side", "close side", "skills", ""};

// LVGL button matrix action callback
static void autonBtnmAction(lv_event_t *e) {
  lv_obj_t *obj = lv_event_get_target(e);
  const char *txt =
      lv_btnmatrix_get_btn_text(obj, lv_btnmatrix_get_selected_btn(obj));
  if (lv_obj_get_user_data(obj) == (void *)100) {
    if (std::string(txt) == "far side") {
      controller.rumble("._");
      selected_auto = AutoMode::FAR_SIDE;
    } else if (std::string(txt) == "close side") {
      controller.rumble(".. _");
      selected_auto = AutoMode::CLOSE_SIDE;
    } else if (std::string(txt) == "skills") {
      controller.rumble("_._");
      selected_auto = AutoMode::SKILLS;
    }
  }
}

// Initialization function
void initialize() {
  pros::lcd::initialize(); // initialize brain screen
  chassis.calibrate();     // calibrate sensors

  // LVGL GUI setup
  lv_theme_t *th = lv_theme_default_init(
      lv_disp_get_default(), lv_palette_main(LV_PALETTE_BLUE),
      lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
  lv_theme_set_apply_cb(th, NULL);

  std::cout << pros::millis() << ": creating gui..." << std::endl;

  // Create tabview and add tabs
  lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 20);
  lv_obj_t *mainTab = lv_tabview_add_tab(tabview, "Autons");

  // Create button matrix for autonomous selection
  lv_obj_t *mainBtnm = lv_btnmatrix_create(mainTab);
  lv_btnmatrix_set_map(mainBtnm, btnmMap);
  lv_btnmatrix_set_btn_ctrl_all(mainBtnm, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_obj_add_event_cb(mainBtnm, autonBtnmAction, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_set_size(mainBtnm, 450, 50);
  lv_obj_align(mainBtnm, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_user_data(mainBtnm, (void *)100);
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
  bool ip_extended = true; // intake piston extended = down state
  bool mogo_clamped = false;

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
    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
      intake_mtr.move(127); // Run intake at full speed
    } else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
      intake_mtr.move(-127); // Run intake in reverse at full speed
    } else {
      intake_mtr.move(0); // Stop intake when no button is pressed
    }

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

    // Apply reverse drive logic to chassis control and update screen color
    if (reverse_drive) {
      chassis.tank(-rightY, -leftY);
      pros::lcd::set_text(0, "REVERSE"); // Indicate reverse drive mode
    } else {
      chassis.tank(leftY, rightY);
      pros::lcd::set_text(0, "FORWARD"); // Indicate forward drive mode
    }

    // Mogo clamp control
    if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
      mogo_clamped = !mogo_clamped;
      mogoClamp.set_value(mogo_clamped); // Toggle mogo clamp
    }

    // Intake up/down control
    if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) {
      ip_extended = !ip_extended;
      intakePiston.set_value(ip_extended); // Toggle intake position
    }

    pros::delay(30); // Run for 30 ms then update
  }
}
