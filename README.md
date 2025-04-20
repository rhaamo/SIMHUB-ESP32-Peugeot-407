# SimHub ESP32 Peugeot 407 CAN thingy

Based on:
- code base from https://github.com/eCrowneEng/ESP-SimHub
- cluster CAN code from https://github.com/fmiglior/simhub-psa-dashboard

Some references:
- https://github.com/gilles504/simhub_psa_can_arduino
- https://github.com/fmiglior/simhub-psa-dashboard
- https://autowp.github.io/
- https://www.youtube.com/watch?v=q42XOnm4GwQ (talks about some "magic unlock" that I should already be doing...)

Doesn't really seems to work, probably missing some can magic...
If an ESP32RET is put on the other end the log seems ok, the cluster just doesn't like the frames or something is missing.
Also `ESP32Can.writeFrame` quickly returns false but with an ESP32RET on the end, it still sends frames properly.... 

The magic is in `SHCustomProtocol.h` and `can.h` where the packets are defined and some debug infos put in.

In the repository is a log from a working bus made with savvy can...

This is made to use one of the smol 3.3V CAN transceiver since the ESP32 already have a CAN controller builtin.

Connect TX=D5 and RX=D4.

# SimHub

Uses the following NCalc code:
```text
format([SpeedKmh],0)+';'+format([Rpms],0)+';'+format([DataCorePlugin.Computed.Fuel_Percent],0)+';'+ format([WaterTemperature],0)+';'+ [Handbrake] +';' + [TurnIndicatorLeft] +';'+ [TurnIndicatorRight] +';'+ [TCActive] +';' + [ABSActive] + '\n'
```
