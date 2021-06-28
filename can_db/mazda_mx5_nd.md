# Mazda MX-5 Miata (ND generation)

Here is information on some of the PIDs that you're most likely to use with
RaceChrono and equations to get the right scale, etc.

These PIDs were tested on 2019 Mazda MX-5 Miata RF, but should apply to other
model years of the ND generation as well. If you find any mistakes, or model
year specific PIDs, please send a pull request!

## Recommended PIDs:

Here are PIDs and RaceChrono equations for data channels that should be enough
for most people:

Channel name | PID | Equation | Notes
------------ | --- | -------- | -----
Accelerator position (%) | 514 | `E/2.5` |
Brake position (%) | 120 | `max(0, 100 + bytesToInt(raw, 4, 1)) / 2.55` | Might need more work, possibly a different scale
Steering angle | 134 | `(16000 - bytesToUInt(raw, 0, 2)) * 0.1` | Positive value = turning right. You can add a `-` if you prefer it the other way around.
Speed | 514 | `bytesToInt(raw, 2, 2) / 360.0` |
Engine RPM | 514 | `bytesToUInt(raw, 0, 2) / 4` | If someone records >8192 rpms to confirm, let me know :)

### Would be nice to find PIDs for ...

Essentials: fuel level, coolant temperature, clutch pedal, current gear.

Advanced: tire pressures, tire temperatures, lat/long acceleration (if
available), yaw rate, individual wheel speeds.
