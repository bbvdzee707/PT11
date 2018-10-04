#pragma config(Sensor, S1,     Linefinder,     sensorEV3_Color)
#pragma config(Sensor, S2,     Gyro,           sensorEV3_Gyro)
#pragma config(Sensor, S3,     Color,          sensorEV3_Color)
#pragma config(Sensor, S4,     Ultra,          sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          claw,          tmotorEV3_Medium, PIDControl, encoder)
#pragma config(Motor,  motorB,          left,          tmotorEV3_Large, PIDControl, reversed, driveLeft, encoder)
#pragma config(Motor,  motorC,          right,         tmotorEV3_Large, PIDControl, reversed, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define ENCODER_10CM (10/17.6)*360
#define DEFAULT_SPEED 32
#define BEND_SPEED 15
#define CORRECTION_RATE 1.1
#define GYRO_CORRECTION 1.0
#define COLOR_THRESHOLD 50
#include "EV3Mailbox.c"

#define SLINE S1
#define SGYRO S2
#define SCOLOR S3
#define SULTRA S4

//move with speed
void move(int speedB, int speedC) {
	setMotorSpeed(motorB, speedB);
	setMotorSpeed(motorC, speedC);
}

//move with speed for time
void moveTime(int speedB, int speedC, int time) {
	move(speedB, speedC);
	delay(time);
	move(0, 0);
}

//move the claw
void moveMotorA(int speed, int ms) {
	setMotorSpeed(motorA, speed);
	delay(ms);
	setMotorSpeed(motorA, 0);
}

//pick up or set down
void clawControl(bool pickUp) {
	if (pickUp == 1) {
		setMotorSpeed(motorA, -40);
		delay(1000);
		setMotorSpeed(motorA, 0);
	} else {
		setMotorSpeed(motorA, 40);
		delay(1000);
		setMotorSpeed(motorA, -5);
		delay(300);
		setMotorSpeed(motorA, 0);

	}
}

//turn x degrees
void xturnDegrees(int degrees) {
	resetGyro(SGYRO);
	int correction = 100;

	if (getGyroDegrees(SGYRO) <= degrees*GYRO_CORRECTION) {
		correction *= -1;
	}

	while (abs(getGyroDegrees(SGYRO)) < abs(degrees*GYRO_CORRECTION)) {
		setMotorSync(motorB, motorC, correction, 15);
	}

	move(0, 0);
}

// listen to commands from computer
task TListen() {
	char msgBufIn[MAX_MSG_LENGTH];  // To contain the incoming message.
	char msgBufOut[MAX_MSG_LENGTH];  // To contain the outgoing message
	openMailboxIn("EV3_INBOX0");
	openMailboxOut("EV3_OUTBOX0");
	while (true){
		readMailboxIn("EV3_INBOX0", msgBufIn);
		if (strcmp(msgBufIn, "")) {

		} else if (strcmp(msgBufIn, "")) {

		} else {

		}

		int dist = getUSDistance(SULTRA);
		int head = getGyroDegrees(SGYRO);

		sprintf(msgBufOut, "%i %i", dist, head);
		writeMailboxOut("EV3_OUTBOX0", msgBufOut);

		delay(100);
	}
}

// initialization
void init() {
	moveMotorA(100, 600);
	resetGyro(SGYRO);
	//startTask(TListen);
}

//search
bool search() {
	int dist = 255;
	int color_reflect = 0;
	bool busy = true;
	resetGyro(SGYRO);
	int heading = getGyroDegrees(SGYRO);
	int dHeading = 0;
	bool done = false;

	while (busy) {
		dist = getUSDistance(SULTRA);
		color_reflect = getColorReflected(SCOLOR);
		dHeading = getGyroDegrees(SGYRO) - heading;
		setMotorSync(motorB, motorC, -1*dHeading*CORRECTION_RATE, -1*DEFAULT_SPEED);

		if (dist < 10) {
			moveTime(-5, -20, 3500);
			resetGyro(SGYRO);
		} else if (color_reflect > 10) {
			move(0, 0);
			setMotorSyncEncoder(motorB, motorC, -1*dHeading*CORRECTION_RATE, ENCODER_10CM, -1*DEFAULT_SPEED);
			delay(750);
			clawControl(true);
			busy = false;
		} else {
			setMotorSync(motorB, motorC, -1*dHeading*CORRECTION_RATE, -1*DEFAULT_SPEED);
		}
	}

	if (done) {
		return false;
	} else {
		return true;
	}
}

// return to base
void returnToBase() {
	xturnDegrees(90);

	while (getColorReflected(SLINE) > 50) {
		move(DEFAULT_SPEED, DEFAULT_SPEED);
	}

	xturnDegrees(90);

	sensorReset(SLINE);

	while(true) {
		if(getColorReflected(SLINE) < COLOR_THRESHOLD) {
			motor(motorC) = BEND_SPEED;
			motor(motorB) = DEFAULT_SPEED;

		} else {
			motor(motorC) = DEFAULT_SPEED;
			motor(motorB) = BEND_SPEED;
		}

		if(getUSDistance(SULTRA) < 15) {
			xturnDegrees(90);
			break;
		}
	}
}

// sort item
void sortItem() {
	displayBigTextLine(1, "SORTING");
	waitUntil(getButtonPress(buttonAny));
}

task main() {
	init();

	bool searching = true;
	while(searching) {
		searching = search();
		returnToBase();
		sortItem();
	}
}
