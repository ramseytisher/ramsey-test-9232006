#include "vex.h"
#include "auton.h"
#include "motor_feedback.h"

using namespace vex;

competition Competition;

// Runs once at startup, before the field enables autonomous or driver control.
void pre_auton(void) {
  vexcodeInit();
}

// A match field and controller Timed Run both call this.
// Left or Right runs that routine. No tap does nothing, including the
// controller Programming Skills menu, which has no separate signal.
void autonomous(void) {
  if (matchSideSelected()) {
    runSelectedAuton();
    return;
  }
  Drivetrain.stop();
}

// Keep a joystick command inside the percent range the motor API accepts.
static int clampPercent(int speed) {
  if (speed > 100) {
    return 100;
  }
  if (speed < -100) {
    return -100;
  }
  return speed;
}

// The drivetrain class has drive and turn, not arcade.
// Spin each motor group from the joysticks instead.
static void spinSide(motor_group &side, int speed) {
  // A small stick reading is noise. Stop instead of creeping.
  if (speed > -5 && speed < 5) {
    side.stop();
  } else if (speed > 0) {
    side.spin(forward, speed, percent);
  } else {
    side.spin(reverse, -speed, percent);
  }
}

// Field control calls this for the driver portion of the match.
void usercontrol(void) {
  while (true) {
    int forward = Controller1.Axis3.position();
    int turn = Controller1.Axis1.position();
    spinSide(LeftDrive, clampPercent(forward + turn));
    spinSide(RightDrive, clampPercent(forward - turn));
    wait(20, msec);
  }
}

int main() {
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);
  
  pre_auton();

  while (true) {
    // A match field accepts taps only while it is disabled.
    // Controller Timed Run counts down without disabling, so with no field
    // the buttons stay up until autonomous starts.
    bool choosing = !Competition.isAutonomous() &&
                    (!Competition.isDriverControl() || !Competition.isFieldControl());
    autonSelectorUpdate(choosing);
    // Motor heat and battery on the controller, including during auton and driver.
    updateMotorFeedback();
    wait(20, msec);
  }
}