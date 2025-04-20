# SimHub ESP32 Peugeot 407 CAN thingy

Based on:
- code base from https://github.com/eCrowneEng/ESP-SimHub
- cluster CAN code from https://github.com/fmiglior/simhub-psa-dashboard

Some references:
- https://github.com/gilles504/simhub_psa_can_arduino
- https://github.com/fmiglior/simhub-psa-dashboard
- https://autowp.github.io/
- https://www.youtube.com/watch?v=q42XOnm4GwQ (talks about some "magic unlock" that I should already be doing...)

The magic is in `SHCustomProtocol.h` and `can.h` where the packets are defined and some debug infos put in.

In the repository is a log from a working bus made with savvy can...

This is made to use one of the smol 3.3V CAN transceiver since the ESP32 already have a CAN controller builtin.

BUT REMOVES THE 120R RESISTOR ! It's the one that is right on top of CANH/L pins. With it it won't work.

Connect TX=D5 and RX=D4.

# SimHub

Uses the following NCalc code:
```text
format([SpeedKmh],0)+';'+format([Rpms],0)+';'+format([DataCorePlugin.Computed.Fuel_Percent],0)+';'+ format([WaterTemperature],0)+';'+ [Handbrake] +';' + [TurnIndicatorLeft] +';'+ [TurnIndicatorRight] +';'+ [TCActive] +';' + [ABSActive] + '\n'
```

# Peugeot 407
Tested with a Phase 1 (2008) cluster, in KM/h with RPM gauge up to 6k.

- [ ] Water temp (L gauge)
- [ ] Oil temp (R gauge)
- [ ] RPM
- [ ] Speed
- [ ] Tank level
- [ ] LCD (XXX)
 - [ ] Total distance
 - [ ] Trip distance
 - [ ] Gearbox indicator
 - [ ] Speed limit/cruise control
 - [ ] Other/Warnings
- [ ] Indicators (lighting)
 - [ ] main beam
 - [ ] dipped beam
 - [ ] front fog
 - [ ] rear fog
- [ ] Indicators (warnings)
 - [ ] front seat belts
 - [ ] fuel level
 - [ ] left turn
- [ ] Indicators (warnings)
  - [ ] parking brake
  - [ ] right turn
- [ ] Indicators (warning)
  - [ ] emission control system
  - [ ] ABS
  - [ ] passenger airbag
  - [ ] pre-heat (diesel)
- [ ] Lighting levels
- [ ] Buttons
  - [ ] Lighting
  - [ ] Reset
