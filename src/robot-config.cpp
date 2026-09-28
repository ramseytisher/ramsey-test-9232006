#include "vex.h"

using namespace vex;

brain Brain;

motor ColtonMotor = motor(PORT2, ratio18_1, false);
rotation Rot1 = rotation(PORT5, false);

void vexcodeInit(void) {
  ColtonMotor.setStopping(brake);
  ColtonMotor.setVelocity(30, percent);
}