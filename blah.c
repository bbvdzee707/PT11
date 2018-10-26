
task main()
{
	setMotorSyncEncoder(motorB, motorC, 0, -800, -25);
while(true) {
	string out;
	stringFormat(out, "%i", getColorHue(S1));
	displayBigTextLine(1, out);
}

}
