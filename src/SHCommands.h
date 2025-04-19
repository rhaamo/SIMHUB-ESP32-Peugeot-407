#define MESSAGE_HEADER 0x03

void Command_Hello() {
	FlowSerialTimedRead();
	delay(10);
	FlowSerialPrint(VERSION);
	FlowSerialFlush();
}

void Command_SetBaudrate() {
	SetBaudrate();
}

void Command_ButtonsCount() {
	FlowSerialWrite(0);
	FlowSerialFlush();
}

void Command_TM1638Count() {
	FlowSerialWrite((byte)(0));
	FlowSerialFlush();
}

void Command_SimpleModulesCount() {
	FlowSerialWrite(0);
	FlowSerialFlush();
}

void Command_Acq() {
	FlowSerialWrite(0x03);
	FlowSerialFlush();
}

void Command_DeviceName() {
	FlowSerialPrint(DEVICE_NAME);
	FlowSerialPrint("\n");
	FlowSerialFlush();
}

void Command_UniqueId() {
	auto id = getUniqueId();
	FlowSerialPrint(id);
	FlowSerialPrint("\n");
	FlowSerialFlush();
}

void Command_MCUType() {
	// Fakes an Arduino Mega
	FlowSerialPrint(SIGNATURE_0);
	FlowSerialPrint(SIGNATURE_1);
	FlowSerialPrint(SIGNATURE_2);
	FlowSerialFlush();
}

void Command_EncodersCount() {
}

void Command_SpeedoData() {
}

void Command_TachData() {
}

void Command_BoostData() {
}

void Command_TempData() {
}

void Command_ConsData() {
}

void Command_FuelData() {
}

void Command_GLCDData() {
}

void Command_ExpandedCommandsList() {
	FlowSerialPrintLn("mcutype");
	FlowSerialPrintLn("keepalive");
	FlowSerialPrintLn();
	FlowSerialFlush();
}

void Command_TM1638Data() {
}

void Command_Features() {
	delay(10);

	// Matrix
	// FlowSerialPrint("M");

	// LCD
	// FlowSerialPrint("L");
	// FlowSerialPrint("K");

	// Gear
	FlowSerialPrint("G");

	// Name
	FlowSerialPrint("N");

	// UniqueID
	FlowSerialPrint("I");

	// Additional buttons
	FlowSerialPrint("J");

	// Custom Protocol Support
	FlowSerialPrint("P");

	// Xpanded support
	FlowSerialPrint("X");

	// RGB MATRIX
	// FlowSerialPrint("R");

	// Adafruit motor shields
	// FlowSerialPrint("V");

	FlowSerialPrint("\n");
	FlowSerialFlush();
}

void Command_Motors() {
}

void Command_Shutdown() {
}

void Command_7SegmentsData() {
}

void Command_RGBLEDSCount() {
	FlowSerialWrite((byte)(0));
	FlowSerialFlush();
}

void Command_RGBLEDSData()
{
	// Acq !
	FlowSerialWrite(0x15);
}

void Command_RGBMatrixData() {
	// Acq !
	FlowSerialWrite(0x15);
}

void Command_MatrixData() {
}

void Command_GearData() {
	char gear = FlowSerialTimedRead();
}

void Command_I2CLCDData() {
}

void Command_CustomProtocolData() {
	shCustomProtocol.read();
	FlowSerialWrite(0x15);
}
