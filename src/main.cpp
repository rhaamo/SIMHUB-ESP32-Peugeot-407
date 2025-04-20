#include <Arduino.h>
#include <EspSimHub.h>

/**
 * Enable ESP-NOW or WiFi or use Serial
 */
#define CONNECTION_TYPE SERIAL // SERIAL or WIFI
#if CONNECTION_TYPE != SERIAL
// Emits extra events to Serial that show network communication, set to false to save memory and make faster
#define DEBUG_BRIDGE false
#endif

/**
 * WiFi configuration
 */
#if CONNECTION_TYPE == WIFI
// Less secure if you plan to commit or share your files, but saves a bunch of memory. 
//  If you hardcode credentials the device will only work in your network
#define USE_HARDCODED_CREDENTIALS false

#if USE_HARDCODED_CREDENTIALS
#define WIFI_SSID "Wifi NAME"
#define WIFI_PASSWORD "WiFi Password"
#endif

#define BRIDGE_PORT 10001 // Perle TruePort uses port 10,001 for the first serial routed to the client

#include <TcpSerialBridge2.h>
#endif // end CONNECTION_TYPE == WIFI

// Title of the device
#define DEVICE_NAME "SH-OTT-PEUGEOT-407" //{"Group":"General","Name":"DEVICE_NAME","Title":"Device name,\r\n make sure to use a unique name when using multiple arduinos","DefaultValue":"SimHub Dash","Type":"string","Template":"#define DEVICE_NAME \"{0}\""}

#include <pgmspace.h>
#include <EEPROM.h>
#include <SPI.h>
#include "Arduino.h"
#include <Wire.h>
#include "FlowSerialRead.h"

// ----------------------------------------------------- HW SETTINGS, PLEASE REVIEW ALL -------------------------------------------

#define ENABLE_MICRO_GAMEPAD 1            //{"Group":"GAMEPAD","Name":"ENABLE_MICRO_GAMEPAD","Title":"Enable arduino micro gamepad output for all the activated buttons/encoders","DefaultValue":"0","Type":"bool"}
#define MICRO_GAMEPAD_ENCODERPRESSTIME 50 //{"Name":"MICRO_GAMEPAD_ENCODERPRESSTIME","Title":"Define how long (in milliseconds) the encoder related button will be hold after an encoder movement","DefaultValue":"50","Type":"int","Condition":"ENABLE_MICRO_GAMEPAD>0","Max":100}

// J revision sketch. Don't modify this
#define VERSION 'j'

#include "SHCustomProtocol.h"
SHCustomProtocol shCustomProtocol;
#include "SHCommands.h"
unsigned long lastMatrixRefresh = 0;

void idle(bool critical) {
	shCustomProtocol.idle();
}

void buttonStatusChanged(int buttonId, byte Status) {
	arqserial.CustomPacketStart(0x03, 2);
	arqserial.CustomPacketSendByte(buttonId);
	arqserial.CustomPacketSendByte(Status);
	arqserial.CustomPacketEnd();
}



void setup()
{
#if CONNECTION_TYPE != SERIAL
#if DEBUG_BRIDGE
	Serial.begin(115200);
#endif
#endif

#if CONNECTION_TYPE != SERIAL
	ECrowneDataProxy::setup(&outgoingStream, &incomingStream);
#endif

	// Main SimHub serial
	FlowSerialBegin(19200);

	// For debugging purposes
	Serial2.begin(115200);

	Serial2.println("meow :3");

	shCustomProtocol.setup();
	arqserial.setIdleFunction(idle);
}

char loop_opt;
char xactionc;
unsigned long lastSerialActivity = 0;


void loop() {
#if CONNECTION_TYPE != SERIAL
	ECrowneDataProxy::loop();
#endif
	shCustomProtocol.loop();

	// Wait for data
	if (FlowSerialAvailable() > 0) {
		if (FlowSerialTimedRead() == MESSAGE_HEADER)
		{
			lastSerialActivity = millis();
			// Read command
			loop_opt = FlowSerialTimedRead();
			switch(loop_opt) {
				case '1': Command_Hello(); break;
				case '0': Command_Features(); break;
				case '4': Command_RGBLEDSCount(); break;
				case '2': Command_TM1638Count(); break;
				case 'B': Command_SimpleModulesCount(); break;
				case 'X': {
					String xaction = FlowSerialReadStringUntil(' ', '\n');
					if (xaction == F("list")) Command_ExpandedCommandsList();
					else if (xaction == F("mcutype")) Command_MCUType();
					else if (xaction == F("tach")) Command_TachData();
					else if (xaction == F("speedo")) Command_SpeedoData();
					else if (xaction == F("boost")) Command_BoostData();
					else if (xaction == F("temp")) Command_TempData();
					else if (xaction == F("fuel")) Command_FuelData();
					else if (xaction == F("cons")) Command_ConsData();
					else if (xaction == F("encoderscount")) Command_EncodersCount();
					break;
				}
				case 'J': Command_ButtonsCount(); break;
				case 'A': Command_Acq(); break;
				case 'N': Command_DeviceName(); break;
				case 'I': Command_UniqueId(); break;
				case '3': Command_TM1638Data(); break;
				case 'V': Command_Motors(); break;
				case 'S': Command_7SegmentsData(); break;
				case '6': Command_RGBLEDSData(); break;
				case 'R': Command_RGBMatrixData(); break;
				case 'M': Command_MatrixData(); break;
				case 'G': Command_GearData(); break;
				case 'L': Command_I2CLCDData(); break;
				case 'K': Command_GLCDData();  break; // Nokia | OLEDS
				case 'P': Command_CustomProtocolData(); break;
				case '8': Command_SetBaudrate(); break;
			}
		}
	}
	if (millis() - lastSerialActivity > 5000) {
		Command_Shutdown();
	}
}
