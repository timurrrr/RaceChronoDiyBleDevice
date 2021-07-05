# Mazda MX-5 Miata (ND generation)

Here is information on some of the CAN IDs that you're most likely to use with
RaceChrono and equations to get the right scale, etc.

These CAN IDs were tested on 2019 Mazda MX-5 Miata RF, but should apply to other
model years of the ND generation as well. If you find any mistakes, or model
year specific CAN IDs, please send a pull request!

## Recommended CAN IDs:

Here are CAN IDs and RaceChrono equations for data channels that should be
enough for most people:

Channel name | CAN ID | Equation | Notes
------------ | --- | -------- | -----
Accelerator position (%) | 514 | `E/2.5` |
Brake position (%) | 120 | `max(0, 100 + bytesToInt(raw, 4, 1)) / 2.55` | Might need more work, possibly a different scale
Steering angle | 134 | `(16000 - bytesToUInt(raw, 0, 2)) * 0.1` | Positive value = turning right. You can add a `-` if you prefer it the other way around.
Speed | 514 | `bytesToInt(raw, 2, 2) / 360.0` |
Engine RPM | 514 | `bytesToUInt(raw, 0, 2) / 4` | If someone records >8192 rpms to confirm, let me know :)

### Would be nice to find CAN IDs for ...

Essentials: fuel level, coolant temperature, clutch pedal, current gear.

Advanced: tire pressures, tire temperatures, lat/long acceleration (if
available), yaw rate, individual wheel speeds.
