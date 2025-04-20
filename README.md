# SimHub ESP32 Peugeot 407 CAN thingy

Based on:
- code base from https://github.com/eCrowneEng/ESP-SimHub
- cluster CAN code from https://github.com/fmiglior/simhub-psa-dashboard
- https://autowp.github.io/

The magic is in `SHCustomProtocol.h` and `can.h` where the packets are defined and some debug infos put in.

In the repository is a log from a working bus made with savvy can...

This is made to use one of the smol 3.3V CAN transceiver since the ESP32 already have a CAN controller builtin.

BUT REMOVES THE 120R RESISTOR ! It's the one that is right on top of CANH/L pins. With it it won't work.

Connect TX=D5 and RX=D4.

Use TX2 (pins D16&D17) on an external FTDI to uses the Serial2 output (will shows received frames, and later more infos on what the fw is doing).

# SimHub

Uses the following custom protocol JAVASCRIPT code:
```javascript
// Game specific
var game = $prop('DataCorePlugin.CurrentGame');
var gameID = 0; // no specifics
if (game == 'ETS2' || game == 'ATS') {
	gameID = 1;
} else if (game == 'BeamNgDrive') {
	gameID = 2;
} else if (game == 'AssettoCorsa') {
	gameID = 3;
}
// Cluster
var cluster = 0; // Peugeot 407

// Game datas
var speed = format($prop('SpeedKmh'), 0);
var rpms = format($prop('Rpms'), 0);
var fuel = format($prop('DataCorePlugin.Computed.Fuel_Percent'), 0);
var waterTemp = format($prop('WaterTemperature'), 0);
var oilTemperature = format($prop('OilTemperature'), 0);
var handbrake = $prop('Handbrake');
var turnLeft = $prop('TurnIndicatorLeft');
var turnRight = $prop('TurnIndicatorRight');
var tcActive = $prop('TCActive');
var absActive = $prop('ABSActive');
var gear = $prop('Gear');
var parkingLight = $prop('DataCorePlugin.GameRawData.light_Parkingbrake');
var lowBeam = $prop('DataCorePlugin.GameRawData.light_LowBeam');
var highBeam = $prop('DataCorePlugin.GameRawData.light_HighBeam');

return `${gameID};${cluster};${speed};${rpms};${fuel};${waterTemp};${oilTemperature};${handbrake};${turnLeft};${turnRight};${tcActive};${absActive};${gear};${parkingLight};${lowBeam};${highBeam};\n`
```

# Peugeot 407
Tested with a Phase 1 (2008) cluster, in KM/h with RPM gauge up to 6k.

- [x] Water temp (L gauge) (works but need calibration)
- [x] Oil temp (R gauge) (works but need calibration)
- [x] RPM
- [x] Speed
- [x] Tank level (need cal?)
- [ ] LCD
 - [ ] Total distance
 - [ ] Trip distance
 - [ ] Gearbox indicator
 - [ ] Speed limit/cruise control
 - [ ] Other/Warnings
- [ ] Indicators (lighting)
 - [x] main beam
 - [x] dipped beam
 - [ ] front fog
 - [ ] rear fog
- [ ] Indicators (warnings)
 - [ ] front seat belts
 - [ ] fuel level
 - [x] left turn
- [ ] Indicators (warnings)
  - [ ] parking brake
  - [x] right turn
- [ ] Indicators (warning)
  - [ ] emission control system
  - [ ] ABS (might work, need test/checks)
  - [ ] passenger airbag
  - [ ] pre-heat (diesel)
- [ ] Lighting levels
- [ ] Buttons
  - [ ] Lighting
  - [ ] Reset
