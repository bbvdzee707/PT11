#define ENCODER_10CM (10/17.6)*360
#define DEFAULT_SPEED 20
#define CORRECTION_RATE 1.1
#define GYRO_CORRECTION 1.1

void moveStraight(int speed, int time) {
	resetGyro(S2);
	clearTimer(T1);
	int heading = getGyroDegrees(S2);
	int dHeading = 0;

	while (getTimerValue(T1) < time) {
		dHeading = getGyroDegrees(S2) - heading;
		setMotorSync(motorB, motorC, -1*dHeading*CORRECTION_RATE, -1*speed);
	}
}

void turnDegrees(int degrees) {
	resetGyro(S2);
	int correction = 100;
	if (getGyroDegrees(S2) <= degrees*GYRO_CORRECTION) {
		correction *= -1;
	}

	while (abs(getGyroDegrees(S2)) < abs(degrees)) {
		setMotorSync(motorB, motorC, correction, 15);
	}
}

task main() {
	//moveStraight(DEFAULT_SPEED, 10000);
	//moveStraight(0, 0);
	turnDegrees(-180);
}
