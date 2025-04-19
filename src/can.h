#include <ESP32-TWAI-CAN.hpp>

CanFrame canMsg1; // 0x0F6
CanFrame canMsg2; // 0x036
CanFrame canMsg3; // 0x0B6
CanFrame canMsg4; // 0x128
CanFrame canMsg5; // 0x168
CanFrame canMsg6; // 0x161

// struct can_frame canMsg1; // 0x0F6
// struct can_frame canMsg2; // 0x036
// struct can_frame canMsg3; // 0x0B6
// struct can_frame canMsg4; // 0x128
// struct can_frame canMsg5; // 0x168
// struct can_frame canMsg6; // 0x161


void carregaCAN() {
  // slow BSI datas
  // canMsg1.can_id = 0x0F6;
  // canMsg1.can_dlc = 8;
  // 86 00 00 00 00 08 63 20 
  // 86 64 00 00 00 08 63 20 
  // 8E A0 00 00 00 08 63 20 
  // https://autowp.github.io/#0F6
  canMsg1.identifier = 0x0F6;
  canMsg1.extd = 0;
  canMsg1.data_length_code = 8;
  canMsg1.data[0] = 0x8E; // ignition 86=134, 8E=142
  canMsg1.data[1] = 0x00; // coolant temp 64=100, A0=160
  canMsg1.data[2] = 0x00; // odometer 1
  canMsg1.data[3] = 0x00; // odometer 2
  canMsg1.data[4] = 0x00; // odometer 3
  canMsg1.data[5] = 0x10; // ???
  canMsg1.data[6] = 0x63; // temperature round(T/2.0-39.5) / 0...250 = -40...+85 / 63=99
  canMsg1.data[7] = 0x20; // reverse gear light + turn right/left on last bits / 20=100000

  // BSI commands
  // canMsg2.can_id = 0x036;
  // canMsg2.can_dlc = 8;
  // 00 00 06 2F 00 80 00 00 
  // 00 00 06 2F 01 80 00 00 
  // https://autowp.github.io/#036
  canMsg2.identifier = 0x036;
  canMsg2.extd = 0;
  canMsg2.data_length_code = 8;
  canMsg2.data[0] = 0x00; // ???
  canMsg2.data[1] = 0x00; // ???
  canMsg2.data[2] = 0x00; // economy mode enabled
  canMsg2.data[3] = 0x25; // dashboard lighting enabled
  canMsg2.data[4] = 0x01; // ignition mode. 001 on, 010 off, 011 unknown
  canMsg2.data[5] = 0x00; // ???
  canMsg2.data[6] = 0x00; // ???
  canMsg2.data[7] = 0xA0; // ???

  // fast BSI datas
  // canMsg3.can_id = 0x0B6;
  // canMsg3.can_dlc = 8;
  // 00 00 00 00 00 FF 00 A0 
  // D9 00 65 8F 00 FF 00 A0 
  // https://autowp.github.io/#0B6
  canMsg3.identifier = 0x0B6;
  canMsg3.extd = 0;
  canMsg3.data_length_code = 8;
  canMsg3.data[0] = 0x00; // tach rpm / D9=217
  canMsg3.data[1] = 0x00; // ???
  canMsg3.data[2] = 0x00; // actual speed * 100 in km/h / 65=101
  canMsg3.data[3] = 0x00; // ??
  canMsg3.data[4] = 0x00; // odometer from start, cm
  canMsg3.data[5] = 0x00; // ??
  canMsg3.data[6] = 0x00; // fuel consumption, counter
  canMsg3.data[7] = 0xA0; // ??? A0=160

  // signal commands/dash lights
  // canMsg4.can_id = 0x128;
  // canMsg4.can_dlc = 8;
  // 00 00 00 00 00 A0 00 00 
  // https://autowp.github.io/#128
  canMsg4.identifier = 0x128;
  canMsg4.extd = 0;
  canMsg4.data_length_code = 8;
  canMsg4.data[0] = 0x00; // driver seat belt warning + parking break
  canMsg4.data[1] = 0x00; // any door or trunk open
  canMsg4.data[2] = 0x00; // ???
  canMsg4.data[3] = 0x00; // ???
  canMsg4.data[4] = 0x00; // (lights ?)
  canMsg4.data[5] = 0x00; // low fuel level
  canMsg4.data[6] = 0x00; // seat belts blinks
  canMsg4.data[7] = 0x01; // dashboard backlighting

  // commandes combinés témoins
  // canMsg5.can_id = 0x168;
  // canMsg5.can_dlc = 8;
  // 00 00 00 00 00 00 00 00 
  // 00 00 00 00 02 00 00 00 
  // https://autowp.github.io/#168
  canMsg5.identifier = 0x168;
  canMsg5.extd = 0;
  canMsg5.data_length_code = 8;
  canMsg5.data[0] = 0x00;
  canMsg5.data[1] = 0x00;
  canMsg5.data[2] = 0x00;
  canMsg5.data[3] = 0x00; // abs e esp
  canMsg5.data[4] = 0x00; // 02=2
  canMsg5.data[5] = 0x00;
  canMsg5.data[6] = 0x00;
  canMsg5.data[7] = 0x00;

  // Combustivel
  // état BSI temp niveaux
  // canMsg6.can_id = 0x161;
  // canMsg6.can_dlc = 7;
  // 00 00 00 00 00 00 00 
  // 00 00 D7 64 00 00 00 
  // 00 00 62 00 00 00 00 
  // https://autowp.github.io/#161
  canMsg6.identifier = 0x161;
  canMsg6.extd = 0;
  canMsg6.data_length_code = 7;
  canMsg6.data[0] = 0x00;
  canMsg6.data[1] = 0x00;
  canMsg6.data[2] = 0x00; // D7=215, 98 ?
  canMsg6.data[3] = 0x00; // fuel level? 00..64=00..100
  canMsg6.data[4] = 0x00;
  canMsg6.data[5] = 0x00;
  canMsg6.data[6] = 0x00;
}
