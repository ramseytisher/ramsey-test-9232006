#include "vex.h"
#include "hello.h"

using namespace vex;

competition Competition;

void pre_auton(void) {
  vexcodeInit();          // DO NOT REMOVE
  showMultifileTest();    // from hello.cpp

  wait(2, seconds);
}

void autonomous(void) {
  Brain.Screen.setCursor(11, 1);
  Brain.Screen.setPenColor(yellow);
  Brain.Screen.print("auton slot (empty)");
}

void usercontrol(void) {
  int blink = 0;
  while (1) {
    blink++;
    Brain.Screen.setFillColor((blink % 2) == 0 ? orange : black);
    Brain.Screen.drawCircle(220, 200, 16);
    wait(20, msec);
  }
}

int main() {
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);
  
  pre_auton();
  
  ColtonMotor.spinFor(forward, 1, turns);
  ColtonMotor.stop();
  
  while (true) {
    wait(100, msec);

    Brain.Screen.setPenColor(red);
    Brain.Screen.setCursor(10, 1);
    Brain.Screen.clearLine();
    Brain.Screen.print("Rot1: %.1f deg", Rot1.position(degrees));
  }
}