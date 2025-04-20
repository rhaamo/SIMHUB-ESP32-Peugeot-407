#ifndef __SHCUSTOMPROTOCOL_H__
#define __SHCUSTOMPROTOCOL_H__

#include <Arduino.h>
#include <can.h>
#include <ESP32-TWAI-CAN.hpp>

// Default for ESP32
#define CAN_TX 5
#define CAN_RX 4

class SHCustomProtocol {
private:
public:
  /*
  CUSTOM PROTOCOL CLASS
  SEE https://github.com/zegreatclan/SimHub/wiki/Custom-Arduino-hardware-support

  GENERAL RULES :
          - ALWAYS BACKUP THIS FILE, reinstalling/updating SimHub would overwrite it with the default version.
          - Read data AS FAST AS POSSIBLE in the read function
          - NEVER block the arduino (using delay for instance)
          - Make sure the data read in "read()" function READS ALL THE DATA from the serial port matching the custom protocol definition
          - Idle function is called hundreds of times per second, never use it for slow code, arduino performances would fall
          - If you use library suspending interrupts make sure to use it only in the "read" function when ALL data has been read from the serial port.
                  It is the only interrupt safe place

  COMMON FUNCTIONS :
          - FlowSerialReadStringUntil('\n')
                  Read the incoming data up to the end (\n) won't be included
          - FlowSerialReadStringUntil(';')
                  Read the incoming data up to the separator (;) separator won't be included
          - FlowSerialDebugPrintLn(string)
                  Send a debug message to simhub which will display in the log panel and log file (only use it when debugging, it would slow down arduino in run
  conditions)

  */

  // Called when starting the arduino (setup method in main sketch)
  void setup() {
		// Set LED on GPIO2 as OUTPUT
		pinMode(2, OUTPUT);
		digitalWrite(2, LOW);
		
		// 10, 10 = queues config
		if (ESP32Can.begin(ESP32Can.convertSpeed(125), CAN_TX, CAN_RX, 10, 10)) {
			// CAN bus init OK
			// digitalWrite(2, HIGH);
		}
		// mcp2515.reset();
		// mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
		// mcp2515.setNormalMode();

    carregaCAN(); // load can msg pack on can.h
  }

  // Called when new data is coming from computer
  void read() {
    // EXAMPLE 1 - read the whole message and sent it back to simhub as debug message
    // Protocol formula can be set in simhub to anything, it will just echo it
    // -------------------------------------------------------
    //	String message = FlowSerialReadStringUntil('\n');
    //	FlowSerialDebugPrintLn("Message received : " + message);

    /*
    // -------------------------------------------------------
    // EXAMPLE 2 - reads speed and gear from the message
    // Protocol formula must be set in simhub to
    // format([DataCorePlugin.GameData.NewData.SpeedKmh],'0') + ';' + isnull([DataCorePlugin.GameData.NewData.Gear],'N')
    // -------------------------------------------------------
*/

    // read the simhub protocol and store on variables
    // SpeedKmh float
    // Rpms float
    // DataCorePlugin.Computed.Fuel_Percent float 
    // WaterTemperature float
    // Handbrake int 0/100
    // TurnIndicatorLeft 0/1
    // TurnIndicatorRight 0/1
    // TCActive 0/1?
    // ABSActive 0/1?
    // use this protocol: format([SpeedKmh],0)+';'+format([Rpms],0)+';'+format([DataCorePlugin.Computed.Fuel_Percent],0)+';'+ format([WaterTemperature],0)+';'
		// + [Handbrake] +';' + [TurnIndicatorLeft] +';'+ [TurnIndicatorRight]  +';'+ [TCActive] +';' + [ABSActive] + '\n'

    int vel = (FlowSerialReadStringUntil(';').toInt()) * 0.390909; // adjust the number to match to your dashboard
    int rpm = (FlowSerialReadStringUntil(';').toInt()) * 0.031286; // adjust the number to match to your dashboard
    int fuel = (FlowSerialReadStringUntil(';').toInt()) * 1.28;
    int wtemp = (FlowSerialReadStringUntil(';').toInt()) * 1.28;
    int handbrake = (FlowSerialReadStringUntil(';').toInt());
    int leftblink = (FlowSerialReadStringUntil(';').toInt());
    int rightblink = (FlowSerialReadStringUntil(';').toInt());
    int esp = (FlowSerialReadStringUntil(';').toInt());
    int babs = (FlowSerialReadStringUntil('\n').toInt());

    // rpm
    canMsg3.data[0] = (rpm & 0xFF);
    // speed
    canMsg3.data[2] = (vel & 0xFF);
    // blinks
    canMsg4.data[4] = ((leftblink * 2) + (rightblink * 4) & 0xFF);
    // fuel
    canMsg6.data[3] = (fuel & 0xff);
    // water temp
    canMsg1.data[1] = (wtemp & 0xff);
    // tc and abs
    canMsg5.data[3] = ((esp * 16) + (babs * 32) & 0xff);
    // park brake
    canMsg5.data[0] = ((handbrake * 4) & 0xFF);
  }

  // Some frames received from the dashboard:
  // Initially (probably because "not init"):
  // id: 217, len: 8:, data: 92 10 00 00 00 FF 00 7F
  // Then:
  // id: 217, len: 8:, data: 92 00 00 00 00 FF FF FF
  // Changed to when pressed check/0000:
  // id: 217, len: 8:, data: 92 20 00 00 00 FF 00 7F
  // Then back to:
  // id: 217, len: 8:, data: 92 00 00 00 00 FF FF FF
  // Pressing the +/- button sends:
  // id: 217, len: 8:, data: A2 00 00 00 00 FF FF FF
  // With the first data element cycling through:
  // 02 12 22 32 42 52 62 72 82 92 A2 B2 C2 D2 E2 F2
  //  2 18 34 40            ...                   242
  // So for ID 217:
  // DATA1=brightness
  // DATA2=00 when showing the big gear indication in the middle, =20 when showing it on the top right
  // DATA7=FF when showing the big gear indication in the middle, =00 when showing it on the top right
  // DATA8=FF when showing the big gear indication in the middle, =7F when showing it on the top right
  // DATA6,7,8 could be related to the odometer ?
  // DATA2,3,4,5 starts increasing or moving as soon as the cluster starts doing stuff (like moving the needles)

	void tryReadFrame() {
		CanFrame rxFrame;
		if (ESP32Can.readFrame(rxFrame, 0)) {
			digitalWrite(2, HIGH);
      Serial2.printf("Received CAN frame id: %03X, len: %i:, data:", rxFrame.identifier, rxFrame.data_length_code);
      for (int i=0; i<rxFrame.data_length_code; i++) {
        Serial2.printf(" %02X", rxFrame.data[i]);
      }
      Serial2.printf("\r\n");
		}
	}

  // Called once per arduino loop, timing can't be predicted,
  // but it's called between each command sent to the arduino
  void loop() {
    // send can messages to the dashboard
		//// put led off if we have an error while writing the CAN frame

    // Water temp
		if (!ESP32Can.writeFrame(canMsg1)) {
			// digitalWrite(2, LOW);
		}
		tryReadFrame();
		if (!ESP32Can.writeFrame(canMsg1)) {
			// digitalWrite(2, LOW);
		}
		tryReadFrame();

    if (!ESP32Can.writeFrame(canMsg2)) {
			// digitalWrite(2, LOW);
		}
		tryReadFrame();

    // Speed, RPM
    if (!ESP32Can.writeFrame(canMsg3)) {
			// digitalWrite(2, LOW);
		}
		tryReadFrame();

    // Blinkers
    if (!ESP32Can.writeFrame(canMsg4)) {
			// digitalWrite(2, LOW);
		}
		tryReadFrame();

    // ESP, ABS, Park brake
    if (!ESP32Can.writeFrame(canMsg5)) {
			// digitalWrite(2, LOW);
		}
		tryReadFrame();

    // Fuel
    if (!ESP32Can.writeFrame(canMsg6)) {
			// digitalWrite(2, LOW);
		}
		tryReadFrame();

    // idk
		if (!ESP32Can.writeFrame(canMsg7)) {
			// digitalWrite(2, LOW);
		}
		tryReadFrame();

    // idk
		if (!ESP32Can.writeFrame(canMsg8)) {
			// digitalWrite(2, LOW);
		}
		tryReadFrame();

		// mcp2515.sendMessage(&canMsg1);
    // mcp2515.sendMessage(&canMsg1);
    // mcp2515.sendMessage(&canMsg2);
    // mcp2515.sendMessage(&canMsg3);
    // mcp2515.sendMessage(&canMsg4);
    // mcp2515.sendMessage(&canMsg5);
    // mcp2515.sendMessage(&canMsg6);
		
  }

  // Called once between each byte read on arduino,
  // THIS IS A CRITICAL PATH :
  // AVOID ANY TIME CONSUMING ROUTINES !!!
  // PREFER READ OR LOOP METHOS AS MUCH AS POSSIBLE
  // AVOID ANY INTERRUPTS DISABLE (serial data would be lost!!!)
  void idle() {
  }
};

#endif
