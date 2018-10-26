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
#define COLOR_THRESHOLD 36
#include "EV3Mailbox.c"
#define DEFAULT_TURN_SPEED 20

int DEFAULT_SPEED = 25;
int BEND_SPEED = (int)((DEFAULT_SPEED/3)*2);
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

// true=right, false=left
int turnTicks = 210;
void turn90(bool direction) {
	move(0, 0);
	if(!direction) {
		setMotorSyncEncoder(motorB, motorC, 100, turnTicks, DEFAULT_TURN_SPEED);
	} else {
		setMotorSyncEncoder(motorB, motorC, -100, turnTicks, DEFAULT_TURN_SPEED);
	}
	waitUntilMotorStop(motorB);
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
	BEND_SPEED = (int)((newSpeed/3)*2);

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
	evenLane = true;

	while (busy) {
		busy = TListen();
		dist = getUSDistance(SULTRA);
		color_reflect = getColorReflected(SCOLOR);
		move(DEFAULT_SPEED, DEFAULT_SPEED);

		if ((getTimerValue(T1) > 2000) && (dist > 10)) {
			move(0, 0);
			correct(heading);
			resetGyro(SGYRO);
			clearTimer(T1);
		}

		string out = "";
		stringFormat(out, "%1.1f", getGyroDegrees(SGYRO));

		eraseDisplay();
		displayBigTextLine(1, "SEARCHING");
		displayBigTextLine(4, out);

		if (dist < 10) {
			bool turn = true;
			if (!evenLane) {
				turn = false;
			}

			turn90(turn);
			setMotorSyncEncoder(motorB, motorC, 0, -1*ENCODER_10CM, -1*DEFAULT_SPEED);
			waitUntilMotorStop(motorB);
			turn90(turn);
			evenLane = !evenLane;
			resetGyro(SGYRO);
		}

		if (color_reflect > 4) {
			move(0, 0);
			setMotorSyncEncoder(motorB, motorC, 0, ENCODER_10CM, -0.6*DEFAULT_SPEED);
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
	int turn = true;
	if (!evenLane) {
		turn = false;
	}

	turn90(turn);

	while (getColorReflected(SLINE) > COLOR_THRESHOLD) {
		move(DEFAULT_SPEED, DEFAULT_SPEED);
	}

	setMotorSyncEncoder(motorB, motorC, 0, -0.2*ENCODER_10CM, -1*DEFAULT_SPEED);
	waitUntilMotorStop(motorB);
	setMotorSyncEncoder(motorB, motorC, 0, 1.5*ENCODER_10CM, DEFAULT_SPEED);
	turn90(true);
	sensorReset(SLINE);

	while(followLineRight()) {
		if(getUSDistance(SULTRA) < 15) {
			setMotorSyncEncoder(motorB, motorC, 0, -1.1*ENCODER_10CM, -DEFAULT_SPEED);
			waitUntilMotorStop(motorB);
			turn90(true);
			waitUntilMotorStop(motorB);
			break;
		} else {}
	}
}

// sort item
void sortItem() {
	displayBigTextLine(1, "SORTING");
	delay(300);
	long toyColor = getColorHue(SCOLOR);
	if (toyColor > 350) toyColor -= 350;
	while (true) {
		move(DEFAULT_SPEED, DEFAULT_SPEED);
		long sortColor = getColorHue(SLINE);
		if (sortColor > 350) sortColor -= 350;
		if ((sortColor > toyColor - 10) && (sortColor < toyColor + 10)) {
			move(0, 0);
			break;
		} else {}
	}

	displayBigTextLine(1, "COLOR MATCH FOUND");
	setMotorSyncTime(motorB, motorC, 0, 300, DEFAULT_SPEED);
	turn90(false);
	clawControl(false);
	setMotorSyncEncoder(motorB, motorC, 0, 0.8*ENCODER_10CM, DEFAULT_SPEED);
	waitUntilMotorStop(motorB);
	turn90(true);
	while(followLineRight()) {
		if (getUSDistance(SULTRA) < 5) {
			break;
		}
	}
	turn90(true);


}

task main() {
	init();
	bool searching = true;
	while(searching) {
		searching = search();
		returnToBase();
		sortItem();
		resetAllSensorAutoID();
	}
}
