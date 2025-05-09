#include <ESP32-TWAI-CAN.hpp>

CanFrame canMsg1; // 0x0F6
CanFrame canMsg2; // 0x036
CanFrame canMsg3; // 0x0B6
CanFrame canMsg4; // 0x128
CanFrame canMsg5; // 0x168
CanFrame canMsg6; // 0x161
CanFrame canMsg7; // 0x228
CanFrame canMsg8; // 0x3F6
CanFrame canMsg9; // 0x1A1
CanFrame canMsg10; // 0x1A8


void carregaCAN() {
  // slow BSI datas
  // 86 00 00 00 00 08 63 20 
  // 86 64 00 00 00 08 63 20 
  // 8E A0 00 00 00 08 63 20 
  // https://autowp.github.io/#0F6
  canMsg1.identifier = 0x0F6;
  canMsg1.extd = 0;
  canMsg1.data_length_code = 8;
  canMsg1.data[0] = 0x8E; // ignition 86=134, 8E=142
  canMsg1.data[1] = 0x00; // *SH* coolant temp 64=100, A0=160
  canMsg1.data[2] = 0x00; // odometer 1
  canMsg1.data[3] = 0x00; // odometer 2
  canMsg1.data[4] = 0x00; // odometer 3
  canMsg1.data[5] = 0x10; //
  canMsg1.data[6] = 0x63; // temperature round(T/2.0-39.5) / 0...250 = -40...+85 / 63=99
  canMsg1.data[7] = 0x20; // reverse gear light + turn right/left on last bits / 20=100000

  // BSI commands
  // 00 00 06 2F 00 80 00 00 
  // 00 00 06 2F 01 80 00 00 
  // https://autowp.github.io/#036
  canMsg2.identifier = 0x036;
  canMsg2.extd = 0;
  canMsg2.data_length_code = 8;
  canMsg2.data[0] = 0x00; // ???
  canMsg2.data[1] = 0x00; // ???
  canMsg2.data[2] = 0x00; // economy mode enabled
  canMsg2.data[3] = 0x2F; // *SH* dashboard lighting (level ?)
  canMsg2.data[4] = 0x01; // ignition mode. 001 on, 010 off, 011 unknown
  canMsg2.data[5] = 0x00; // ???
  canMsg2.data[6] = 0x00; // ???
  canMsg2.data[7] = 0xA0; // ???

  // fast BSI datas
  // 00 00 00 00 00 FF 00 A0 
  // D9 00 65 8F 00 FF 00 A0 
  // https://autowp.github.io/#0B6
  canMsg3.identifier = 0x0B6;
  canMsg3.extd = 0;
  canMsg3.data_length_code = 8;
  canMsg3.data[0] = 0x00; // *SH* tach rpm / D9=217
  canMsg3.data[1] = 0x00; // ???
  canMsg3.data[2] = 0x00; // *SH* actual speed * 100 in km/h / 65=101
  canMsg3.data[3] = 0x00; // ??
  canMsg3.data[4] = 0x00; // odometer from start, cm
  canMsg3.data[5] = 0x00; // ??
  canMsg3.data[6] = 0x00; // fuel consumption, counter
  canMsg3.data[7] = 0xA0; // ??? A0=160

  // signal commands/dash lights
  // 00 00 00 00 00 A0 00 00 
  // neutral: 20 00 00 00 00 A0 20 00 
  // speed 1: 20 00 00 00 00 A0 90 C0 
  // https://autowp.github.io/#128
  canMsg4.identifier = 0x128;
  canMsg4.extd = 0;
  canMsg4.data_length_code = 8;
  canMsg4.data[0] = 0x00; // ???
  canMsg4.data[1] = 0x00; // any door or trunk open
  canMsg4.data[2] = 0x00; // ???
  canMsg4.data[3] = 0x00; // ???
  canMsg4.data[4] = 0x00; // *SH* lights
  canMsg4.data[5] = 0xA0; // low fuel level /!\ dash doesn't init if not 0xA0 ?
  canMsg4.data[6] = 0x00; // *SH* gearbox speed
  canMsg4.data[7] = 0x00; // 0xC0 will display a - on the LCD

  // commandes combinés témoins
  // 00 00 00 00 00 00 00 00 
  // 00 00 00 00 02 00 00 00 
  // https://autowp.github.io/#168
  canMsg5.identifier = 0x168;
  canMsg5.extd = 0;
  canMsg5.data_length_code = 8;
  canMsg5.data[0] = 0x00; // *SH* parking light
  canMsg5.data[1] = 0x00;
  canMsg5.data[2] = 0x00;
  canMsg5.data[3] = 0x00; // *SH* abs e esp
  canMsg5.data[4] = 0x00;
  canMsg5.data[5] = 0x00;
  canMsg5.data[6] = 0x00;
  canMsg5.data[7] = 0x00;

  // Combustivel
  // état BSI temp niveaux
  // 00 00 00 00 00 00 00 
  // 00 00 D7 64 00 00 00 
  // 00 00 62 00 00 00 00 
  // https://autowp.github.io/#161
  canMsg6.identifier = 0x161;
  canMsg6.extd = 0;
  canMsg6.data_length_code = 7;
  canMsg6.data[0] = 0x00;
  canMsg6.data[1] = 0x00;
  canMsg6.data[2] = 0x00; // *SH* oil temp - D7=215, 98 ?
  canMsg6.data[3] = 0x00; // *SH* fuel level - 00..64=00..100
  canMsg6.data[4] = 0x00;
  canMsg6.data[5] = 0x00;
  canMsg6.data[6] = 0x00;

  // 80 00 80 80 00 00 00 00 
  // https://autowp.github.io/#228
  canMsg7.identifier = 0x228;
  canMsg7.extd = 0;
  canMsg7.data_length_code = 8;
  canMsg7.data[0] = 0x80;
  canMsg7.data[1] = 0x00;
  canMsg7.data[2] = 0x80;
  canMsg7.data[3] = 0x80;
  canMsg7.data[4] = 0x00;
  canMsg7.data[5] = 0x00;
  canMsg7.data[6] = 0x00;
  canMsg7.data[7] = 0x00;

  // 00 00 00 00 00 C0 00 
  // https://autowp.github.io/#3F6
  canMsg8.identifier = 0x3F6;
  canMsg8.extd = 0;
  canMsg8.data_length_code = 7;
  canMsg8.data[0] = 0x00;
  canMsg8.data[1] = 0x00;
  canMsg8.data[2] = 0x00;
  canMsg8.data[3] = 0x00;
  canMsg8.data[4] = 0x00;
  canMsg8.data[5] = 0x00;
  canMsg8.data[6] = 0xC0;
  canMsg8.data[7] = 0x00;
  
  // 00 40 C6 00 00 00 00 00
  // 80 40 C6 00 00 00 00 00
  // 00 E0 C6 00 00 00 00 00
  // https://autowp.github.io/#1A1
  canMsg9.identifier = 0x1A1;
  canMsg9.extd = 0;
  canMsg9.data_length_code = 7;
  canMsg9.data[0] = 0x00;
  canMsg9.data[1] = 0xE0;
  canMsg9.data[2] = 0xC6;
  canMsg9.data[3] = 0x00;
  canMsg9.data[4] = 0x00;
  canMsg9.data[5] = 0x00;
  canMsg9.data[6] = 0x00;
  canMsg9.data[7] = 0x00;
  
  // 80 00 00 00 00 00 A0 00
  // https://autowp.github.io/#1A8
  canMsg10.identifier = 0x1A8;
  canMsg10.extd = 0;
  canMsg10.data_length_code = 7;
  canMsg10.data[0] = 0x80; // 0X80
  canMsg10.data[1] = 0x00; // varies
  canMsg10.data[2] = 0x00; // varies
  canMsg10.data[3] = 0x00;
  canMsg10.data[4] = 0x00;
  canMsg10.data[5] = 0x00;
  canMsg10.data[6] = 0xA0;
  canMsg10.data[7] = 0x00;
}
