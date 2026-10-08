#include "vex.h"
#include "auton.h"

using namespace vex;

// Match autonomous from the left start. Add steps in order.
// Helpers: setDriveSpeed, setTurnSpeed, driveForward, driveReverse,
// turnLeft, turnRight, autonStep. Distances are inches. Angles are degrees.
void leftSideMatchAuton() {
  autonStep("Left match");
  // setDriveSpeed(50);
  // driveForward(24);
  // turnRight(90);
}
