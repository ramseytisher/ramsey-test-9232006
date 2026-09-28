#include "vex.h"
#include "hello.h"

using namespace vex;

static const char *kFile2Banner = "FILE 2: hello.cpp linked";

int extraFileMarker() {
  return 2;
}

void showMultifileTest() {
  Brain.Screen.clearScreen();
  Brain.Screen.setFont(mono20);

  Brain.Screen.setPenColor(cyan);
  Brain.Screen.setCursor(1, 1);
  Brain.Screen.print("FILE 1: main.cpp running");

  Brain.Screen.setPenColor(green);
  Brain.Screen.setCursor(3, 1);
  Brain.Screen.print(kFile2Banner);

  Brain.Screen.setPenColor(white);
  Brain.Screen.setCursor(5, 1);
  Brain.Screen.print("marker = %d  (want 2)", extraFileMarker());

  Brain.Screen.setCursor(7, 1);
  Brain.Screen.print("Competition template + extra file");

  Brain.Screen.setCursor(9, 1);
  Brain.Screen.print("Both lines = multi-file OK");
}