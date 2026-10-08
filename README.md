# Robot notebook

Edit this file, then ask to sync the code from the README. Hardware tables map to `src/robot-config.cpp`, `include/robot-config.h`, and `src/main.cpp`. Leave a row in place when a device is removed and mark it `removed` so the matching code is deleted.

Values already in the code are filled in below. Anything marked `placeholder` was assumed and should be replaced with a measurement or the real port.

## Team

| Field | Value |
|---|---|
| Team number | |
| Robot name | |
| Season / game | V5RC Override, manual 2.0 |
| Last hardware check | |

## Drivetrain

Six motors, three per side. Each side is two green 11W motors and one 5.5W motor. All six use `ratio18_1` because a 5.5W motor is fixed at 200 rpm, the same shaft speed as a green 11W cartridge. One 5.5W motor sits on each side.

| Name | Port | Type | Cartridge | Reversed | Code name |
|---|---|---|---|---|---|
| Left front | 1 | 11W | green 18:1 | no | `LeftA` |
| Left middle | 3 | 11W | green 18:1 | no | `LeftB` |
| Left 5.5W | 4 | 5.5W | fixed 200 rpm | no | `LeftC` |
| Right front | 7 | 11W | green 18:1 | yes | `RightA` |
| Right middle | 8 | 11W | green 18:1 | yes | `RightB` |
| Right 5.5W | 9 | 5.5W | fixed 200 rpm | yes | `RightC` |

`LeftA` and `RightA` must stay 11W motors. `driveFor` reads position from the first motor in each group.

| Measurement | Value | Status |
|---|---|---|
| Wheel diameter | 4 in | placeholder |
| Wheel circumference | 319.19 mm | placeholder, from the 4 in wheel |
| Track width (left wheel center to right wheel center) | 295 mm | placeholder |
| Wheelbase (front axle to rear axle) | 230 mm | placeholder |
| External gear ratio (driven teeth / driving teeth) | 1.0 | placeholder, direct drive |
| Drive velocity | 50% | in code |
| Turn velocity | 30% | in code |
| Stop mode | brake | in code |
| Driver control | arcade: axis 3 forward, axis 1 turn | in code |

## Other devices

| Name | Port | Type | Notes | Code name |
|---|---|---|---|---|
| Colton motor | 2 | 11W, green 18:1, not reversed | brake, 30% velocity. Not part of the drive. | `ColtonMotor` |
| Rotation sensor | 5 | rotation, not reversed | Printed on the brain as `Rot1` degrees. | `Rot1` |
| Controller | — | primary | | `Controller1` |

Add a row for each new motor, motor group, sensor, pneumatics solenoid, or inertial sensor. Include the port, gear cartridge, reverse flag, and what the mechanism does.

Ports already used: 1, 2, 3, 4, 5, 7, 8, 9.

## Control map

| Input | Action |
|---|---|
| Left stick up / down (axis 3) | Drive forward / back |
| Right stick left / right (axis 1) | Turn |
| Controller screen | Motor heat and battery. See `src/motor_feedback.cpp`. |
| | |

Buttons, joysticks, and partner-controller actions go in the empty rows.

The controller has three rows. Row 1 shows `OK`, `HOT`, `STOP`, or `MISS`, plus battery percent. Row 2 is left-drive temperature (`LA LB LC`). Row 3 is right-drive temperature (`RA RB RC`) and Colton (`CO`). Numbers are degrees Celsius. `HOT` starts at 55°C, where the motor API says full performance ends. `STOP` starts at 70°C, where the motor shuts down until it cools. `--` means it is unplugged. The controller rumbles once at 55°C, and a longer rumble at 70°C or when a motor drops offline. Driver control spins the motor groups directly, because the drivetrain class has `drive` and `turn` but no arcade command.

## Autonomous

| Routine | What it should do | Status |
|---|---|---|
| Left Side Match | Stay on our half. Score three of our Pins across two Goals, then finish off the Field Perimeter. Steps live in `leftSideMatchAuton()` in `src/auton/left_match.cpp`. | selector wired, path empty |
| Right Side Match | Mirror of the left routine. Steps live in `rightSideMatchAuton()` in `src/auton/right_match.cpp`. | selector wired, path empty |
| Skills | Steps live in `skillsAuton()` in `src/auton/skills.cpp`. No brain button starts it. | path empty, not started |

The brain shows LEFT MATCH and RIGHT MATCH. On a field, the buttons take taps only while the robot is disabled. With no field, they stay up until autonomous starts. No tap does nothing. The robot does not drive.

Write the path in the routine, top to bottom. Distances are inches and turns are degrees:

```cpp
setDriveSpeed(50);
driveForward(24);
turnRight(90);
autonStep("Score");
```

## Mechanisms

| Mechanism | Motors / pistons | Behavior we want |
|---|---|---|
| Pin and Cup handler | | Hold one Pin and one Cup. Score them, then release. |
| Toggle | | Seat a Toggle fully, or hold it so it stays neutral. |
| Endgame stance | | Pull every lift down before driving into the Midfield. |

The possession limit is one Pin and one Cup. A Cup does not score. Its opaque half hides the Pin half nested inside it.

## Manual references

Links and page notes the code should follow. Prefer the official page over a paraphrase.

| Topic | Source | Notes |
|---|---|---|
| V5 C++ API | https://api.vex.com/v5/home/cpp/ | Motors, motor groups, drivetrain, sensors |
| Game manual | `reference/override-2.0.pdf` | V5RC Override, version 2.0. Scoring is `<SC1>` through `<SC8>`. Match rules are `<SG1>` through `<SG13>`. Skills rules are `<RSC1>` through `<RSC5>`. |
| Robot build rules | `reference/override-2.0.pdf` | Start at or under 18 in × 18 in × 18 in (`<SG1>`). Stay inside 24 in × 24 in (`<SG2>`) and 50 in tall (`<SG3>`). |
| Team notebook | | Binder, sheet, or photo folder |

## Game updates

Dated rule and game changes. Newest row on top. Say whether the code or the strategy needs to change.

| Date | Update | Affects code? | What to change |
|---|---|---|---|
| 2026-09-03 | Manual 2.0 is the strategy source. Autonomous Win Point and Endgame rules are the version in that file. | Paths still empty | Fill `src/auton/` from the Strategy section. |

## Strategy

Source: `reference/override-2.0.pdf`, version 2.0. Own the Toggles, then stack yellow Pin halves under them. A yellow half is 10 points. An Alliance-colored half is 5. The manual's quadrant example swings from 15–5 to 45–5 or 15–35 when the Toggle changes.

Each Pin has two halves, and each visible half scores on its own. A Cup scores nothing.

| Object, if we own the Toggle | Our points | Opponent's points |
|---|---|---|
| Yellow/yellow Pin | 20 | 0 |
| Our color/yellow Pin | 15 | 0 |
| Our color/yellow Pin, opponent owns the Toggle | 5 | 10 |
| Opponent color/yellow Pin, we own the Toggle | 10 | 5 |

A Toggle counts only when it is fully seated and no Robot is touching it (`<SC4>`). Touching it at the end makes it neutral, and the yellow halves in that quadrant score zero.

Alliance Goals are protected (`<SG9>`). Objects placed on a neutral Goal stay there (`<SG10>`). The possession limit is one Pin and one Cup (`<SG6>`).

### Match

| Priority | Choice |
|---|---|
| 1 | Set our Toggles, then stack yellow/yellow Pins on our two Alliance Goals and the nearest neutral Goal. |
| 2 | Feed that cycle from our loaders. Cover a half we do not want scored with the opaque side of a Cup. |
| 3 | Before the last 10 seconds, stack the tall Midfield Goal. Both Robots then enter the Midfield. |

- Autonomous goal: stay on our half of the Autonomous Line. Score three of our Pins across two Goals so both halves count, then finish with neither Robot touching the Field Perimeter. That is the normal-event Autonomous Win Point (`<SC8>`): six scored halves, two Goals with at least two each. A qualifying event needs seven scored halves and three Goals. The Autonomous Bonus is 12 points to the leader, or 6 each on a tie (`<SC7>`). A violation gives the Bonus to the opponent.
- Driver roles: Robot A cycles loaders and Goals. Robot B sets Toggles, then both Robots go to the Midfield. Each Alliance has 10 Match Load Pins and 10 Cups (`<SG11>`).
- Endgame: the last 10 seconds. Do not place anything else on the Midfield Goal (`<SG12>`). Yellow Pins there belong to the Alliance with more Robots in the Midfield. A tie scores those yellows for nobody. Each Robot in the Midfield is also 8 points. Two Robots against one is 16 to 8, plus every yellow half on the center Goal. Retract before driving in.
- What we refuse to do: cross the Autonomous Line, descore a protected or neutral Goal, carry more than one Pin or one Cup, or hold a tall lift in the Midfield.

### Skills

`skillsAuton()` is not started by a brain button or by a missed tap. Controller Programming Skills calls the same autonomous function as a match, so that menu does nothing until a skills start is wired again.

One Robot, 60 seconds, starting in the red quadrant by the red Alliance Station (`<RSC2>`). Goals start empty (`<RSC4>`).

- Route outline: set each Toggle to its own quadrant color. A red Toggle set to blue does not own the yellow Pins in the red quadrant (`<RSC3>`). Stack yellow/yellow Pins first. Score red Pins only in a red quadrant or the Midfield, and blue Pins only in a blue quadrant or the Midfield. End in the Midfield.
- Score target: every owned yellow half (10) and the 8 points for ending in the Midfield. A yellow/yellow Pin is 20. A color/yellow Pin on the matching side is 15.
- Stop conditions: stop early only after the Field is clear, and only if the scorekeeper was told before the match (`<RSC5>`).

### Rules for code changes

- Follow the strategy section when two behaviors conflict.
- Keep driver control responsive. Drive commands stay in the `usercontrol` loop.
- New devices get a row in this file and a declaration in `include/robot-config.h` in the same change.
- Do not invent ports. If a port cell is empty, ask before writing the device into `robot-config.cpp`.
