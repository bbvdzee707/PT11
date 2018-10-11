#pragma config(Sensor, S1,     Linefinder,     sensorEV3_Color)
#pragma config(Sensor, S2,     Gyro,           sensorEV3_Gyro)
#pragma config(Sensor, S3,     Color,          sensorEV3_Color)
#pragma config(Sensor, S4,     Ultra,          sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          claw,          tmotorEV3_Medium, PIDControl, encoder)
#pragma config(Motor,  motorB,          left,          tmotorEV3_Large, PIDControl, reversed, driveLeft, encoder)
#pragma config(Motor,  motorC,          right,         tmotorEV3_Large, PIDControl, reversed, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define ENCODER_10CM (10/17.6)*360
#define CORRECTION_RATE 1.1
#define GYRO_CORRECTION 1.0
#define COLOR_THRESHOLD 50
#include "EV3Mailbox.c"

int DEFAULT_SPEED = 25;
int BEND_SPEED = (int)(DEFAULT_SPEED/2);
bool go = true;
bool DEFAULT_GO = true;
bool evenLane = true;

#define SLINE S1
#define SGYRO S2
#define SCOLOR S3
#define SULTRA S4

//move with speed
void move(int speedB, int speedC) {
	setMotorSpeed(motorB, speedB);
	setMotorSpeed(motorC, speedC);
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
		setMotorSpeed(motorA, 0);
		delay(1000);
		setMotorSpeed(motorA, 0);

	}
}

//turn x degrees
void xturnDegrees(int degrees) {
	move(0, 0);
	delay(100);
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

bool followLineRight() {
	if(getColorReflected(SLINE) < COLOR_THRESHOLD) {
		motor(motorC) = BEND_SPEED;
		motor(motorB) = DEFAULT_SPEED;
		} else {
		motor(motorC) = DEFAULT_SPEED;
		motor(motorB) = BEND_SPEED;
	}
	return true;
}

bool followLineLeft() {
	if(getColorReflected(SLINE) < COLOR_THRESHOLD) {
		motor(motorB) = BEND_SPEED;
		motor(motorC) = DEFAULT_SPEED;
		} else {
		motor(motorB) = DEFAULT_SPEED;
		motor(motorC) = BEND_SPEED;
	}
	return true;
}

//string getColorFromHue(int hue) {
//	if(hue > 0 && hue < ){return "red";}
//	else if(){return "yellow";}
//	else if(){return "green";}
//	else if(){return "cyan";}
//	else if(){return "blue";}
//	else if(){return "purple";}
//	else if(){return "red";}
//	else {return null;}
//}

bool followSortLine() {
	if(getColorReflected(SLINE) < COLOR_THRESHOLD) {
		motor(motorB) = BEND_SPEED;
		motor(motorC) = DEFAULT_SPEED;

		} else {
		motor(motorB) = DEFAULT_SPEED;
		motor(motorC) = BEND_SPEED;
	}

	if (getColorHue(SCOLOR) == getColorHue(SLINE)) {
		move(0, 0);
		return false;
		} else {
		return true;
	}
	return true;
}

// listen to commands from computer
bool TListen() {
	char msgBufIn[MAX_MSG_LENGTH];  // To contain the incoming message.
	char msgBufOut[MAX_MSG_LENGTH];  // To contain the outgoing message
	openMailboxIn("EV3_INBOX0");
	openMailboxOut("EV3_OUTBOX0");

	displayBigTextLine(1, msgBufIn);
	delay(1000);

	int newSpeed = DEFAULT_SPEED;
	go = DEFAULT_GO;

	readMailboxIn("EV3_INBOX0", msgBufIn);
	//if (strcmp(msgBufIn, "speed 0")) {
	//	newSpeed = 1 * 6;
	//} else if (strcmp(msgBufIn, "speed 1")) {
	//	newSpeed = 2 * 6;
	//} else if (strcmp(msgBufIn, "speed 2")) {
	//	newSpeed = 3 * 6;
	//} else if (strcmp(msgBufIn, "speed 3")) {
	//	newSpeed = 4 * 6;
	//} else if (strcmp(msgBufIn, "speed 4")) {
	//	newSpeed = 5 * 6;
	//} else if (strcmp(msgBufIn, "speed 5")) {
	//	newSpeed = 6 * 6;
	//} else if (strcmp(msgBufIn, "Start")) {
	//	go = true;
	//} else if (strcmp(msgBufIn, "Stop")) {
	//	go = false;
	//} else {

	//}

	DEFAULT_SPEED = newSpeed;
	BEND_SPEED = (int)(newSpeed/2);

	int dist = getUSDistance(SULTRA);
	int head = getGyroDegrees(SGYRO);

	sprintf(msgBufOut, "%i %i", dist, head);
	writeMailboxOut("EV3_OUTBOX0", msgBufOut);

	delay(100);
	return go;
}

// initialization
void init() {
	clawControl(false);
	move(0, 0);
	delay(100);
	resetGyro(SGYRO);

	displayBigTextLine(2, "WAITING FOR");
	displayBigTextLine(5, "CONNECTION");
	while(!TListen()) {
		delay(100);
	}
	eraseDisplay();
}

void correct(int goal) {
	if (getGyroDegrees(SGYRO) < goal) {
		while (getGyroDegrees(SGYRO) < goal) {
			setMotorSpeed(motorB, 5);
		}
	} else {
		while (getGyroDegrees(SGYRO) > goal) {
			setMotorSpeed(motorB, -5);
		}
	}
	move(0, 0);
}

//search
bool search() {
	int dist = 255;
	int color_reflect = 0;
	bool busy = true;
	delay(100);
	resetGyro(SGYRO);
	int heading = getGyroDegrees(SGYRO);
	clearTimer(T1);
	bool done = false;

	while (busy) {
		busy = TListen();
		dist = getUSDistance(SULTRA);
		color_reflect = getColorReflected(SCOLOR);
		setMotorSync(motorB, motorC, 0, -1*DEFAULT_SPEED);

		if (getTimerValue(T1) > 2000) {
			move(0, 0);
			correct(heading);
			delay(500);
			resetGyro(SGYRO);
			clearTimer(T1);
		}

		string out = "";
		stringFormat(out, "%1.1f", getGyroDegrees(SGYRO));

		eraseDisplay();
		displayBigTextLine(1, "SEARCHING");
		displayBigTextLine(4, out);

		if (dist < 10) {
			int turn = 1;
			if (!evenLane) {
				turn = -1;
			}
			xturnDegrees(turn * 90);
			setMotorSyncEncoder(motorB, motorC, 0, -1*ENCODER_10CM, -1*DEFAULT_SPEED);
			waitUntilMotorStop(motorB);
			xturnDegrees(turn * 90);
			evenLane = !evenLane;
			resetGyro(SGYRO);
		}

		if (color_reflect > 10) {
			move(0, 0);
			setMotorSyncEncoder(motorB, motorC, 0, ENCODER_10CM, -1*DEFAULT_SPEED);
			delay(750);
			clawControl(true);
			busy = false;
		} else {
			setMotorSync(motorB, motorC, 0, -1*DEFAULT_SPEED);
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
	int turn = 1;
	if (!evenLane) {
		turn = -1;
	}

	xturnDegrees(turn * 90);

	while (getColorReflected(SLINE) > 50) {
		move(DEFAULT_SPEED, DEFAULT_SPEED);
	}

	setMotorSyncEncoder(motorB, motorC, 0, -1*ENCODER_10CM, -1*DEFAULT_SPEED);
	waitUntilMotorStop(motorB);
	xturnDegrees(45);
	sensorReset(SLINE);

	while(followLineRight()) {
		TListen();
		if(getUSDistance(SULTRA) < 15) {
			setMotorSyncEncoder(motorB, motorC, 0, 1.5*ENCODER_10CM, DEFAULT_SPEED);
			xturnDegrees(90);
			break;
		} else {}
	}
}

// sort item
void sortItem() {
	displayBigTextLine(1, "SORTING");

	while (followSortLine()) {
		TListen();
	}

	displayBigTextLine(1, "COLOR MATCH FOUND");
	waitUntil(getButtonPress(buttonEnter));
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
