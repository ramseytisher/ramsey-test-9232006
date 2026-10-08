#include "vex.h"
#include "motor_feedback.h"

#include <stdio.h>

using namespace vex;

// Controller screen: 3 rows, 19 characters each.
// Row 1 is the alert: OK, HOT, or MISS, plus battery percent.
// Row 2 is left-drive heat: LA, LB, LC.
// Row 3 is right-drive heat: RA, RB, RC, then Colton (CO).
// Numbers are degrees Celsius from motor.temperature(celsius).
// The motor API keeps full performance below 55°C.
// At 70°C the motor stops itself until it cools.
// "--" means the motor is unplugged.
// LC and RC are the 5.5W motors. They use the same temperature call.

static const int kHotCelsius = 55;
static const int kStopCelsius = 70;
static const int kMissCycles = 3;

struct WatchedMotor {
  const char *name;
  motor *device;
};

static WatchedMotor kMotors[] = {
    {"LA", &LeftA},
    {"LB", &LeftB},
    {"LC", &LeftC},
    {"RA", &RightA},
    {"RB", &RightB},
    {"RC", &RightC},
    {"CO", &ColtonMotor},
};

static const int kMotorCount = 7;

// Three characters, right aligned, so the columns stay put.
static void writeTemp(char *dest, int temp) {
  if (temp < 0) {
    snprintf(dest, 4, " --");
  } else {
    snprintf(dest, 4, "%3d", temp);
  }
}

static void printRow(int row, const char *text) {
  char padded[20];
  snprintf(padded, sizeof(padded), "%-19.19s", text);
  Controller1.Screen.setCursor(row, 1);
  Controller1.Screen.print("%s", padded);
}

void updateMotorFeedback() {
  // One screen command per call. The controller drops updates sent faster than this.
  static uint32_t lastUpdate = 0;
  uint32_t now = Brain.Timer.time(msec);
  if (now - lastUpdate < 150) {
    return;
  }
  lastUpdate = now;

  static bool screenCleared = false;
  if (!screenCleared) {
    Controller1.Screen.clearScreen();
    screenCleared = true;
    return;
  }

  static int shownTemp[kMotorCount] = {0};
  static int missStreak[kMotorCount] = {0};

  for (int i = 0; i < kMotorCount; i++) {
    if (kMotors[i].device->installed()) {
      missStreak[i] = 0;
      shownTemp[i] = (int)(kMotors[i].device->temperature(celsius) + 0.5);
    } else if (missStreak[i] < 100) {
      missStreak[i]++;
    }
  }

  int temps[kMotorCount];
  int missIndex = -1;
  int hotIndex = 0;
  for (int i = 0; i < kMotorCount; i++) {
    // Ignore a single missed packet so a loose reading does not flash "--".
    temps[i] = (missStreak[i] >= kMissCycles) ? -1 : shownTemp[i];
    if (temps[i] < 0 && missIndex < 0) {
      missIndex = i;
    }
    if (temps[i] > temps[hotIndex]) {
      hotIndex = i;
    }
  }

  bool anyMiss = missIndex >= 0;
  bool anyStop = temps[hotIndex] >= kStopCelsius;
  bool anyHot = temps[hotIndex] >= kHotCelsius;

  // Rumble once when the alert changes, not on every refresh.
  // Wait a few seconds so motors can finish connecting at startup.
  static int samples = 0;
  static bool announcedMiss = false;
  static bool announcedStop = false;
  static bool announcedHot = false;
  if (samples < 30) {
    samples++;
  }
  bool alertsLive = samples >= 20 && Controller1.installed();
  if (alertsLive && anyMiss && !announcedMiss) {
    Controller1.rumble("---");
  } else if (alertsLive && !anyMiss && anyStop && !announcedStop) {
    Controller1.rumble("---");
  } else if (alertsLive && !anyMiss && !anyStop && anyHot && !announcedHot) {
    Controller1.rumble("..");
  }
  announcedMiss = anyMiss;
  announcedStop = anyStop;
  announcedHot = anyHot;

  char line[20];
  static int nextRow = 1;
  int battery = (int)Brain.Battery.capacity();

  if (nextRow == 1) {
    if (anyMiss) {
      snprintf(line, sizeof(line), "MISS %s B%3d", kMotors[missIndex].name, battery);
    } else if (anyStop) {
      snprintf(line, sizeof(line), "STOP %s %dC B%3d", kMotors[hotIndex].name, temps[hotIndex], battery);
    } else if (anyHot) {
      snprintf(line, sizeof(line), "HOT %s %dC B%3d", kMotors[hotIndex].name, temps[hotIndex], battery);
    } else {
      snprintf(line, sizeof(line), "OK B%3d", battery);
    }
    printRow(1, line);
  } else if (nextRow == 2) {
    char leftA[4], leftB[4], leftC[4];
    writeTemp(leftA, temps[0]);
    writeTemp(leftB, temps[1]);
    writeTemp(leftC, temps[2]);
    snprintf(line, sizeof(line), "L%s%s%s", leftA, leftB, leftC);
    printRow(2, line);
  } else {
    char rightA[4], rightB[4], rightC[4], colton[4];
    writeTemp(rightA, temps[3]);
    writeTemp(rightB, temps[4]);
    writeTemp(rightC, temps[5]);
    writeTemp(colton, temps[6]);
    snprintf(line, sizeof(line), "R%s%s%s C%s", rightA, rightB, rightC, colton);
    printRow(3, line);
  }

  nextRow++;
  if (nextRow > 3) {
    nextRow = 1;
  }
}
