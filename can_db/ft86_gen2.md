# FT86 gen2 cars (2022 Subaru BRZ, Toyota GR86)

For details on how to connect to the CAN bus, as well as more information on the
information available on the CAN bus, see https://github.com/timurrrr/ft86/blob/main/can_bus/gen2.md

Note: the gen2 CAN IDs require some code changes to this CAN bus reader project
(such as different rate limiting parameters) that haven't been pushed to GitHub
yet. Stay tuned!

Special thanks to Ken Houseal for his early legwork on finding some of these
data mappings!

## Recommended CAN IDs:

Here are CAN IDs and RaceChrono equations for data channels that should be enough
for most people:

Channel name | CAN ID | Equation | Notes
------------ | --- | -------- | -----
Accelerator position (%) | 64 | `E / 2.55` |
Brake position (%) | 313 | `min(F / 0.7, 100)` | The 0.7 divider seems to be a good value to get 100% at pressure slightly higher than those you're likely to use on the track for cars with no aero. You can use 0.8 or 0.9 if you see 100% too often.
Brake pressure | 313 | `F * 128` | Coefficient taken from 1st gen cars, seems to match fine?
Steering angle | 312 | `bytesToIntLe(raw, 2, 2) * 0.1` | Positive value = turning right. You can add a `-` if you prefer it the other way around.
Speed | 313 | `bitsToUIntLe(raw, 16, 13) * 0.015694` | You may want to check the multiplier against an external GPS device, especially if running larger/smaller diameter tires
Engine RPM | 64 | `bitsToUIntLe(raw, 16, 14)` |
Coolant temperature | 837 | `E - 40` |
Engine oil temperature | 837 | `D - 40` |

### Advanced CAN IDs

Not required for most people, but if you're a data junkie like me you might
enjoy the extra insight.

Channel name | CAN ID | Equation | Notes
------------ | --- | -------- | -----
Yaw rate | 312 | `bytesToIntLe(raw, 4, 2) * -0.2725` | Calibrated against the gyroscope in RaceBox Mini. Gen1 used 0.286478897 instead.
