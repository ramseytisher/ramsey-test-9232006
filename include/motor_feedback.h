#ifndef MOTOR_FEEDBACK_H
#define MOTOR_FEEDBACK_H

// Motor heat, unplugged motors, and battery on the controller screen.
// Call from the main loop. It limits its own refresh rate.
void updateMotorFeedback();

#endif
