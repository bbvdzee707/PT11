
task main()
{
	int powerwanted = 100;
	int motorDegree = 0;
	setMotorSpeed(motorA, -30);
	delay(1000);
	setMotorSpeed(motorA, 0);
	resetMotorEncoder(motorA);
	string text = "";
	string text2 = "Degree: 0";
	while (true) {
		stringFormat(text2, "Degree: %1.0f", getMotorEncoder(motorA));
		stringFormat(text, "Power: %1.0f", powerwanted);
		displayBigTextLine(1, text);
		displayBigTextLine(4, text2);
		if (getButtonPress(buttonUp)) {
			powerwanted += 5;
		}	else if (getButtonPress(buttonDown)) {
			powerwanted -= 5;
		} else if (getButtonPress(buttonRight)) {
			setMotorSpeed(motorA, powerwanted);
		} else if (getButtonPress(buttonLeft)) {
			setMotorSpeed(motorA, -1*powerwanted);
		} else {
			setMotorSpeed(motorA, 0);
		}
		delay(200);
	}
}
