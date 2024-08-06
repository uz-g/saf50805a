#include "autonomous/routines.hpp"
#include "globals.h"
#include "robot/robot.hpp"

namespace auton_routines {

void close_side_auto() {
    controller.rumble(".-");
    // Implement close side autonomous routine
    // Example:
    // chassis.moveTo(x, y, theta, timeout);
    // robot::intakeOn();
    // pros::delay(1000);
    // robot::intakeOff();
}

void far_side_auto() {
    controller.rumble("-.");
    // Implement far side autonomous routine
}

void skills_auto() {
    controller.rumble("...");
    // Implement skills autonomous routine
}

void runSelectedAutonomous() {
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

}  // namespace auton_routines