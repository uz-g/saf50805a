#include "globals.h"

// Controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// Motor groups
pros::MotorGroup left_mg({-1, 2, -3});
pros::MotorGroup right_mg({8, -6, 7});

// Motors
pros::Motor intake_mtr(9);

// Sensors
pros::Imu inertial(19);

// Pneumatics
pros::adi::DigitalOut mogo1('A', false);
pros::adi::DigitalOut mogo2('B', false);

// Chassis
lemlib::Drivetrain drivetrain(&left_mg, &right_mg, 10, lemlib::Omniwheel::OLD_325, 480, 5);
lemlib::OdomSensors sensors(nullptr, nullptr, nullptr, nullptr, &inertial);
lemlib::ControllerSettings lateral_controller(10, 0, 3, 3, 1, 100, 3, 500, 20);
lemlib::ControllerSettings angular_controller(2, 0, 10, 3, 1, 100, 3, 500, 0);
lemlib::ExpoDriveCurve throttle_curve(3, 10, 1.038);
lemlib::ExpoDriveCurve steer_curve(3, 10, 1.038);

lemlib::Chassis chassis(drivetrain, lateral_controller, angular_controller, sensors, &throttle_curve, &steer_curve);

// Autonomous mode
AutoMode selected_auto = AutoMode::OFF;