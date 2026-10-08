#include "vex.h"
#include "auton.h"

using namespace vex;

// Match autonomous from the right start. Add steps in order.
// Helpers: setDriveSpeed, setTurnSpeed, driveForward, driveReverse,
// turnLeft, turnRight, autonStep. Distances are inches. Angles are degrees.
void rightSideMatchAuton() {
  autonStep("Right match");
  // setDriveSpeed(50);
  // driveForward(24);
  // turnLeft(90);
}
