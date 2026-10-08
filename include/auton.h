#ifndef AUTON_H
#define AUTON_H

// The three brain buttons, plus "nothing tapped yet".
enum AutonChoice {
  AUTON_NONE = 0,
  AUTON_LEFT_MATCH,
  AUTON_RIGHT_MATCH,
  AUTON_SKILLS
};

// Draw the buttons and read taps. Pass false while a match routine is
// running so a tap cannot change it.
void autonSelectorUpdate(bool allowChange);

// Called when a match field starts the autonomous period.
void runSelectedAuton();

// Runs skills.cpp. Nothing calls this until a skills start is wired again.
void runSkillsAuton();

// True after LEFT MATCH or RIGHT MATCH was tapped.
bool matchSideSelected();

// One file per routine so different people can edit them at the same time.
// src/auton/left_match.cpp, src/auton/right_match.cpp, src/auton/skills.cpp.
void leftSideMatchAuton();
void rightSideMatchAuton();
void skillsAuton();

// Tools used inside those routines.
// Distances are inches. Angles are degrees. Speeds are percent.
void autonStep(const char *label);       // Show which step is running.
void setDriveSpeed(double percentSpeed); // Speed for driveForward / driveReverse.
void setTurnSpeed(double percentSpeed);  // Speed for turnLeft / turnRight.
void driveForward(double distanceInches);
void driveReverse(double distanceInches);
void turnRight(double angleDegrees);
void turnLeft(double angleDegrees);

#endif
