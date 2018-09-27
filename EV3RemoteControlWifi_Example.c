////////////////////////////////////////////////////////////////////////////////
// ROBOTC EV3 remote control demonstration program.
// This program receives messages from the host computer: Forward, Backward, Left and Right.
// It sends back the distance in cm measured by the ultrasonic sensor and the angle in degrees
// measured by the gyro sensor.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Connection scheme:
// motorA: medium motor front lever
// motorB: large motor right wheel
// motorC: large motor left wheel
// motorD: -
// Sensor S1: Touch sensor
// Sensor S2: Gyro sensor
// Sensor S3: Color sensor
// Sensor S4: Ultrasonic sensor
////////////////////////////////////////////////////////////////////////////////


// EV3Mailbox.c is needed to enable sending and receiving mailbox messages.
// It is possible to have ten input mailboxes and one output mailbox.
// However in the below code only EV3_INBOX0 and EV3_OUTBOX0 are used.
// See the comments in EV3Mailbox.c for additional info.
// Unfortunately ROBOTC cannot handle multipe C files so we have to include it.
#include "EV3Mailbox.c"
#define ENCODER_10CM (10/17.6)*360
#define DEFAULT_SPEED 20
#define DELAY_TIME 1000

task main() {
	displayBigTextLine(0, "Started!");

	char msgBufIn[MAX_MSG_LENGTH];  // To contain the incoming message.
	char msgBufOut[MAX_MSG_LENGTH];  // To contain the outgoing message

	openMailboxIn("EV3_INBOX0");
	openMailboxOut("EV3_OUTBOX0");

	while (true) {
		// Read input message.
		// readMailboxIn() is non-blocking and returns "" if there is no message.
		readMailboxIn("EV3_INBOX0", msgBufIn);
		if (strcmp(msgBufIn, "Forward") == 0) {
			setMotorSpeed(motorB, DEFAULT_SPEED);
			setMotorSpeed(motorC, DEFAULT_SPEED);
			delay(DELAY_TIME);
			setMotorSpeed(motorB, 0);
			setMotorSpeed(motorC, 0);
			displayBigTextLine(8, "forward");
			} else if (strcmp(msgBufIn, "Stop") == 0) {
			setMotorSpeed(motorB, 0);
			setMotorSpeed(motorC, 0);
			displayBigTextLine(8, "stop");
			} else if (strcmp(msgBufIn, "Left") == 0) {
			setMotorSpeed(motorC, DEFAULT_SPEED);
			delay(DELAY_TIME);
			setMotorSpeed(motorC, 0);
			displayBigTextLine(8, "left");
			} else if (strcmp(msgBufIn, "Right") == 0) {
			setMotorSpeed(motorB, DEFAULT_SPEED);
			delay(DELAY_TIME);
			setMotorSpeed(motorB, 0);
			displayBigTextLine(8, "right");
			} else if (strcmp(msgBufIn, "Backward") == 0) {
			setMotorSpeed(motorB, -1*DEFAULT_SPEED);
			setMotorSpeed(motorC, -1*DEFAULT_SPEED);
			delay(DELAY_TIME);
			setMotorSpeed(motorB, 0);
			setMotorSpeed(motorC, 0);
			displayBigTextLine(8, "backward");
			} else {
			setMotorSpeed(motorB, 0);
			setMotorSpeed(motorC, 0);
			displayBigTextLine(8, "empty message!");
		}


		// Here fill your outgoing message handling.
		// This means sending the distance in cm measured by the ultrasonic sensor
		// and the angle in degrees measured by the gyro sensor.
		// Put both values in msgBufOut.
		// Use the format ?<distance> <angle>" (space between distance and angle) so that it can be read by the host program.
		int dist = getUSDistance(S4);
		int head = getGyroDegrees(S2);

		sprintf(msgBufOut, "%i %i", dist, head);
		writeMailboxOut("EV3_OUTBOX0", msgBufOut);

		delay(100);  // Wait 100 ms to give host computer time to react.
	}

	closeMailboxIn("EV3_INBOX0");
	closeMailboxOut("EV3_OUTBOX0");
	return;
}
