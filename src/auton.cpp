#include "vex.h"
#include "auton.h"

using namespace vex;

// Which routine the brain buttons picked. Stays NONE until someone taps.
static AutonChoice selected = AUTON_NONE;
// True while a finger is down, so one tap counts once.
static bool wasPressed = false;
// The buttons are drawn once, then only again when the choice changes.
static bool selectorDrawn = false;

// One tappable rectangle on the brain. x, y, w, h are pixels.
struct AutonButton {
  int x;
  int y;
  int w;
  int h;
  AutonChoice choice;
  const char *line1;
  const char *line2;
};

// Buttons sit under the title bar and above the Rot1 readout.
static const int kButtonY = 40;
static const int kButtonH = 150;

// Left match and right match. Skills has no brain button.
static const int kButtonCount = 2;
static AutonButton buttons[] = {
    {8, kButtonY, 224, kButtonH, AUTON_LEFT_MATCH, "LEFT", "MATCH"},
    {248, kButtonY, 224, kButtonH, AUTON_RIGHT_MATCH, "RIGHT", "MATCH"},
};

// Fill color for each button.
static color fillFor(AutonChoice choice) {
  switch (choice) {
  case AUTON_LEFT_MATCH:
    return blue;
  case AUTON_RIGHT_MATCH:
    return green;
  case AUTON_SKILLS:
    return orange;
  default:
    return black;
  }
}

// Short name shown on the title line.
static const char *choiceName(AutonChoice choice) {
  switch (choice) {
  case AUTON_LEFT_MATCH:
    return "Left Match";
  case AUTON_RIGHT_MATCH:
    return "Right Match";
  case AUTON_SKILLS:
    return "Skills";
  default:
    return "None - tap a button";
  }
}

// Draw the title, the three buttons, and the rotation readout.
static void drawSelector() {
  Brain.Screen.clearScreen();
  Brain.Screen.setFont(mono20);
  Brain.Screen.setFillColor(black);
  Brain.Screen.setPenColor(white);
  Brain.Screen.setPenWidth(1);
  Brain.Screen.drawRectangle(0, 0, 480, 36);
  // false picks the printAt overload that takes an opaque flag. A bare
  // string is ambiguous against the format-string overload.
  Brain.Screen.printAt(8, 8, false, "Auton: %s", choiceName(selected));

  for (int i = 0; i < kButtonCount; i++) {
    AutonButton button = buttons[i];
    bool isSelected = button.choice == selected;
    Brain.Screen.setFillColor(fillFor(button.choice));
    // White border marks the routine that will run.
    Brain.Screen.setPenColor(isSelected ? white : black);
    Brain.Screen.setPenWidth(isSelected ? 4 : 1);
    Brain.Screen.drawRectangle(button.x, button.y, button.w, button.h);
    Brain.Screen.setPenColor(white);
    // Center each word in the wider two-button layout.
    int line1X = button.x + (button.w - Brain.Screen.getStringWidth(button.line1)) / 2;
    int line2X = button.x + (button.w - Brain.Screen.getStringWidth(button.line2)) / 2;
    Brain.Screen.printAt(line1X, button.y + 52, false, "%s", button.line1);
    Brain.Screen.printAt(line2X, button.y + 80, false, "%s", button.line2);
  }

  Brain.Screen.setPenWidth(1);
  Brain.Screen.setFillColor(black);
  Brain.Screen.setPenColor(black);
  Brain.Screen.drawRectangle(0, 196, 480, 44);
  Brain.Screen.setPenColor(white);
  Brain.Screen.printAt(8, 206, false, "Rot1: %.1f deg", Rot1.position(degrees));
  selectorDrawn = true;
}

// Refresh only the bottom line so the angle stays current.
static void drawRotation() {
  Brain.Screen.setFillColor(black);
  Brain.Screen.setPenColor(black);
  Brain.Screen.drawRectangle(0, 196, 480, 44);
  Brain.Screen.setPenColor(white);
  Brain.Screen.setFont(mono20);
  Brain.Screen.printAt(8, 206, false, "Rot1: %.1f deg", Rot1.position(degrees));
}

// Return the routine under this touch, or NONE if the tap missed every button.
static AutonChoice buttonAt(int x, int y) {
  for (int i = 0; i < kButtonCount; i++) {
    AutonButton button = buttons[i];
    bool insideX = x >= button.x && x < button.x + button.w;
    bool insideY = y >= button.y && y < button.y + button.h;
    if (insideX && insideY) {
      return button.choice;
    }
  }
  return AUTON_NONE;
}

// Poll the brain. allowChange is false while a match routine is running.
void autonSelectorUpdate(bool allowChange) {
  if (!allowChange) {
    // Draw the buttons again the next time a choice is allowed.
    selectorDrawn = false;
    return;
  }

  if (!selectorDrawn) {
    drawSelector();
  }

  bool pressed = Brain.Screen.pressing();
  // Act on the moment the finger lands, not on every frame it stays down.
  if (pressed && !wasPressed) {
    AutonChoice hit = buttonAt(Brain.Screen.xPosition(), Brain.Screen.yPosition());
    if (hit != AUTON_NONE && hit != selected) {
      selected = hit;
      drawSelector();
    }
  }
  wasPressed = pressed;

  drawRotation();
}

// Print the routine name and the step we are on. Call this as a routine runs.
void autonStep(const char *label) {
  Brain.Screen.clearScreen();
  Brain.Screen.setFont(mono20);
  Brain.Screen.setPenColor(white);
  Brain.Screen.printAt(8, 20, false, "Auton: %s", choiceName(selected));
  Brain.Screen.setPenColor(yellow);
  Brain.Screen.printAt(8, 60, false, "%s", label);
}

// How fast driveForward and driveReverse move. 100 is full speed.
void setDriveSpeed(double percentSpeed) {
  Drivetrain.setDriveVelocity(percentSpeed, percent);
}

// How fast turnLeft and turnRight move. 100 is full speed.
void setTurnSpeed(double percentSpeed) {
  Drivetrain.setTurnVelocity(percentSpeed, percent);
}

// Drive straight forward. distanceInches is how far the wheels travel.
void driveForward(double distanceInches) {
  Drivetrain.driveFor(forward, distanceInches, inches);
}

// Drive straight backward.
void driveReverse(double distanceInches) {
  Drivetrain.driveFor(reverse, distanceInches, inches);
}

// Turn clockwise. angleDegrees is how far the robot spins.
void turnRight(double angleDegrees) {
  Drivetrain.turnFor(right, angleDegrees, degrees);
}

// Turn counterclockwise.
void turnLeft(double angleDegrees) {
  Drivetrain.turnFor(left, angleDegrees, degrees);
}

// Run the tapped routine when a match field starts autonomous.
// The paths themselves are in src/auton/left_match.cpp, right_match.cpp, and skills.cpp.
void runSelectedAuton() {
  switch (selected) {
  case AUTON_LEFT_MATCH:
    leftSideMatchAuton();
    break;
  case AUTON_RIGHT_MATCH:
    rightSideMatchAuton();
    break;
  case AUTON_SKILLS:
    skillsAuton();
    break;
  default:
    // No Left or Right tap. Leave the selector up and do not drive.
    break;
  }
  // Make sure the drive is stopped when the routine ends.
  Drivetrain.stop();
}

// Runs skills.cpp. The brain has no skills button, and autonomous() does
// not call this. Programming Skills and a missed tap are the same signal.
void runSkillsAuton() {
  selected = AUTON_SKILLS;
  skillsAuton();
  Drivetrain.stop();
}

// Left and Right are match routines. Skills and no tap are not.
bool matchSideSelected() {
  return selected == AUTON_LEFT_MATCH || selected == AUTON_RIGHT_MATCH;
}
