# Mazda MX5 2005-2015 (third generation, including NC/NCFL/NCFLFL or MK3/MK3.5/MK3.75)

Here is information on some of the PIDs that you're most likely to use with
RaceChrono and equations to get the right scale, etc.

## PID 0x081

Update frequency: 100 times per second.

`0x081` contain data related to steering angle. This PID is available only in
cars with DSC module, so all cars from 2007 onwards.
This frame is only 4 bytes long.

Byte | Meaning
---- | -------
0 | unidentified
1 | on/off (EF/6F or 11101111/01101111)
2 | together with byte 3 represent the signed values for the steering angle
3 | together with byte 2 represent the signed values for the steering angle

Channel name | Equation | Notes
------------ | -------- | -----
Steering angle | `bytesToIntLe(raw, 2, 2)` | positive values are from centered position (0) to right, negative otherwise

## PID 0x081

Update frequency: 100 times per second.

`0x085` contain data related to braking. This PID is available only in
cars with DSC module, so all cars from 2007 onwards.
This frame is 8 bytes long.

Byte | Meaning
---- | -------
0 | together with byte 1 represent the pressure of the braking system
1 | together with byte 0 represent the pressure of the braking system
2 | brake siwtch ON/OFF, information contained in bit 0
3 | Not changing
4 | Not changing
5 | Not changing
6 | Not changing
7 | Not changing

Channel name | Equation | Notes
------------ | -------- | -----
Brake pressure | `(3.4518689053*bytesToIntLe(raw, 0, 2)-327.27)/1000.00` | multipliers are desumed by data from Leisutrhound user on miata.net, may be wrong. Unit is kPa
Brake percentage | `min(0.2*(bytesToIntLe(raw, 0, 2)-102),100` | this is an alternative to get brake percentage. When not pressed the value is 102, after braking and releasing it goes briefli down to 99
Digital | `bitsToUint(raw,17,1)` | can't find in RaceChrono the proper channel for brake switch.


## Typical histogram of PIDs

Here's what the distribution of PIDs looks like in the CAN bus while idling in a
parking lot:

 PID | Decimal PID | Number of packets received over a 10 second period
---- | --- | ---
