#pragma once

#include "api.h"
#include "lemlib/api.hpp"

// Controller
extern pros::Controller controller;

// Motor groups
extern pros::MotorGroup left_mg;
extern pros::MotorGroup right_mg;

// Motors
extern pros::Motor intake_mtr;

// Sensors
extern pros::Imu inertial;

// Pneumatics
extern pros::adi::DigitalOut mogo1;
extern pros::adi::DigitalOut mogo2;

// Chassis
extern lemlib::Chassis chassis;

// Autonomous mode
enum class AutoMode { OFF, CLOSE_SIDE, FAR_SIDE, SKILLS };
extern AutoMode selected_auto;