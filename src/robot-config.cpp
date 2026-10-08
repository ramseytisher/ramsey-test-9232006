#include "vex.h"

using namespace vex;

brain Brain;
controller Controller1 = controller(primary);

motor ColtonMotor = motor(PORT2, ratio18_1, false);
rotation Rot1 = rotation(PORT5, false);

// 3 motors per side: two green 11W and one 5.5W.
// 5.5W motors are fixed at 200 rpm, so they use ratio18_1 as well.
motor LeftA = motor(PORT1, ratio18_1, false);  // 11W green
motor LeftB = motor(PORT3, ratio18_1, false);  // 11W green
motor LeftC = motor(PORT4, ratio18_1, false);  // 5.5W

motor RightA = motor(PORT7, ratio18_1, true);  // 11W green
motor RightB = motor(PORT8, ratio18_1, true);  // 11W green
motor RightC = motor(PORT9, ratio18_1, true);  // 5.5W

motor_group LeftDrive = motor_group(LeftA, LeftB, LeftC);
motor_group RightDrive = motor_group(RightA, RightB, RightC);

// 4 inch wheel circumference, track width, wheelbase, mm, direct drive.
drivetrain Drivetrain = drivetrain(LeftDrive, RightDrive, 319.19, 295, 230, mm, 1.0);

void vexcodeInit(void) {
  ColtonMotor.setStopping(brake);
  ColtonMotor.setVelocity(30, percent);

  // Driver control stops the motor groups. Autonomous stop() uses the drivetrain.
  LeftDrive.setStopping(brake);
  RightDrive.setStopping(brake);
  Drivetrain.setStopping(brake);
  Drivetrain.setDriveVelocity(50, percent);
  Drivetrain.setTurnVelocity(30, percent);
}