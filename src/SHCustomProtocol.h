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

  int brightness = 0x2F;

  // Called when starting the arduino (setup method in main sketch)
  void setup() {
		// Set LED on GPIO2 as OUTPUT
		pinMode(2, OUTPUT);
		digitalWrite(2, LOW);
		
		// 10, 10 = queues config
		if (ESP32Can.begin(ESP32Can.convertSpeed(125), CAN_TX, CAN_RX, 10, 10)) {
			// CAN bus init OK
			Serial2.println("CAN init OK");
		}
    carregaCAN(); // load can msg pack on can.h
  }

  // Called when new data is coming from computer
  void read() {
    // read the simhub protocol and store on variables
    int gameID = (FlowSerialReadStringUntil(';').toInt()); // Game "ID" if we have any specific stuff to do
    // 0=default, 1=ETS2/ATS; 2=BeamNgDrive; 3=AssettoCorsa
    int cluster = (FlowSerialReadStringUntil(';').toInt()); // Cluster ID
    // 0=Peugeot 407
    int vel = (FlowSerialReadStringUntil(';').toInt()) * 0.390909; // adjust the number to match to your dashboard
    int rpm = (FlowSerialReadStringUntil(';').toInt()) * 0.031286; // adjust the number to match to your dashboard
    int fuel = (FlowSerialReadStringUntil(';').toInt()) * 1.28;
    int wtemp = (FlowSerialReadStringUntil(';').toInt()) * 1.28;
    int oiltemp = (FlowSerialReadStringUntil(';').toInt()) * 1.28;
    int handbrake = (FlowSerialReadStringUntil(';').toInt()); // idk what to do with that since we use the on/off parkingLight
    int leftblink = (FlowSerialReadStringUntil(';').toInt());
    int rightblink = (FlowSerialReadStringUntil(';').toInt());
    int esp = (FlowSerialReadStringUntil(';').toInt());
    int babs = (FlowSerialReadStringUntil(';').toInt());
    const char *gear = FlowSerialReadStringUntil(';').c_str(); // R, N, 1-x TODO
    int parkingLight = (FlowSerialReadStringUntil(';').toInt());
    int lowBeam = (FlowSerialReadStringUntil(';').toInt());
    int highBeam = (FlowSerialReadStringUntil(';').toInt());

    int canGear = 0x00;
    if (strcmp(gear, "R") == 0) {
      canGear = 0x10;
      // 20 00 00 00 00 A0 10 00 
    } else if (strcmp(gear, "N") == 0) {
      canGear = 0x20; // not 0x00 ?
      // 20 00 00 00 00 A0 20 00 
    } else if (strcmp(gear, "1") == 0) {
      canGear = 0x90;
      // 20 00 00 00 00 A0 90 C0 
    } else if (strcmp(gear, "2") == 0) {
      canGear = 0x80;
      // 20 00 00 00 00 A0 80 C0 
    } else if (strcmp(gear, "3") == 0) {
      canGear = 0x70;
      // 20 00 00 00 00 A0 70 C0 
    } else if (strcmp(gear, "4") == 0) {
      canGear = 0x60;
      // 20 00 00 00 00 A0 60 C0 
    } else if (strcmp(gear, "5") == 0) {
      canGear = 0x50;
      // 20 00 00 00 00 A0 50 C0 
    } else if (strcmp(gear, "6") == 0) {
      canGear = 0x40;
      // 20 00 00 00 00 A0 40 C0 
    }

    // brightness    
    canMsg2.data[3] = (brightness & 0xFF);
    // rpm
    canMsg3.data[0] = (rpm & 0xFF);
    // speed
    canMsg3.data[2] = (vel & 0xFF);
    // blinks + headlights
    canMsg4.data[4] = (leftblink << 1) + (rightblink << 2) + (highBeam << 5) + (lowBeam << 6);
    // doesn't work, maybe there is a flag needed to instruct the cluster to display the gearbox ?
    canMsg4.data[6] = canGear;
    // fuel
    canMsg6.data[3] = (fuel & 0xff);
    // water temp
    canMsg1.data[1] = (wtemp & 0xff);
    // oil temp
    canMsg6.data[2] = (oiltemp & 0xff);
    // tc and abs
    canMsg5.data[3] = ((esp * 16) + (babs * 32) & 0xff);
    // park brake
    canMsg5.data[0] = (parkingLight << 2);
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

      if (rxFrame.identifier == 0x217) {
        if (brightness != rxFrame.data[0]) {
          // Doesn't work so idk... maybe timing issue with all the tryReadFrame?
          // and the cluster doesn't have time to ack with the right brightness
          // brightness = rxFrame.data[0];
          // Serial2.printf("Brightness changed from button %02x (%i)\r\n", brightness, brightness);
        }
      }
		}
	}

  // Called once per arduino loop, timing can't be predicted,
  // but it's called between each command sent to the arduino
  void loop() {
    // send can messages to the dashboard

    // Water temp
		if (!ESP32Can.writeFrame(canMsg1)) {
			// Serial2.println("Error sending CAN frame");
		}
		tryReadFrame();
		if (!ESP32Can.writeFrame(canMsg1)) {
			// Serial2.println("Error sending CAN frame");
		}
		tryReadFrame();

    if (!ESP32Can.writeFrame(canMsg2)) {
			// Serial2.println("Error sending CAN frame");
		}
		tryReadFrame();

    // Speed, RPM
    if (!ESP32Can.writeFrame(canMsg3)) {
			// Serial2.println("Error sending CAN frame");
		}
		tryReadFrame();

    // Blinkers
    if (!ESP32Can.writeFrame(canMsg4)) {
			// Serial2.println("Error sending CAN frame");
		}
		tryReadFrame();

    // ESP, ABS, Park brake
    if (!ESP32Can.writeFrame(canMsg5)) {
			// Serial2.println("Error sending CAN frame");
		}
		tryReadFrame();

    // Fuel
    if (!ESP32Can.writeFrame(canMsg6)) {
			// Serial2.println("Error sending CAN frame");
		}
		tryReadFrame();

    // idk
		if (!ESP32Can.writeFrame(canMsg7)) {
			// Serial2.println("Error sending CAN frame");
		}
		tryReadFrame();

    // idk
		if (!ESP32Can.writeFrame(canMsg8)) {
			// Serial2.println("Error sending CAN frame");
		}
		tryReadFrame();		
    
    // idk
		if (!ESP32Can.writeFrame(canMsg9)) {
			// Serial2.println("Error sending CAN frame");
		}
		tryReadFrame();	
    
    // idk
		if (!ESP32Can.writeFrame(canMsg10)) {
			// Serial2.println("Error sending CAN frame");
		}
		tryReadFrame();		
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
