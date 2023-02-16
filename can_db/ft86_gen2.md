# FT86 gen2 cars (2022 Subaru BRZ, Toyota GR86)

For details on how to connect to the CAN bus, as well as more information on the
information available on the CAN bus, see https://github.com/timurrrr/ft86/blob/main/can_bus/gen2.md

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
Coolant temperature | 837 | `E - 40` |
Engine RPM | 64 | `bitsToUIntLe(raw, 16, 14)` |
Engine oil temperature | 837 | `D - 40` |
Speed | 313 | `bitsToUIntLe(raw, 16, 13) * 0.015694` | You may want to check the multiplier against an external GPS device, especially if running larger/smaller diameter tires
Steering angle | 312 | `bytesToIntLe(raw, 2, 2) * -0.1` | Positive value = turning left. You can remove the `-` if you prefer it the other way around.

### Advanced CAN IDs

Below is a table with a few more data channels that might be useful for more
detailed analysis. When adding more channels, be aware that it will increase
the file size of your RaceChrono sessions, and might negatively affect the
update rate of the more essential channels, due to limited Bluetooth bandwidth.
However, the communication protocol in RaceChrono is smart enough to optimize
Bluetooth usage if multiple channels share the same CAN ID. As a general rule,
if a new channel has the same CAN ID as an existing channel (such as "Yaw rate"
using the same CAN ID 312 as "Steering angle"), then adding it should not affect
the update rates. Adding a channel based on a new CAN ID (such as "Clutch
position") will likely negatively affect the update rates of all other channels.

Channel name | CAN ID | Equation | Notes
------------ | --- | -------- | -----
Clutch position (%) | 577 | `(F & 0x80) / 1.28` | `100` is "clutch pedal depressed", `0` is "clutch pedal released"
Lateral acceleration | 315 | `bytesToIntLe(raw, 6, 1) * 0.2` |
Longitudinal acceleration | 315 | `bytesToIntLe(raw, 7, 1) * -0.1` |
Combined acceleration | 315 | `sqrt(pow2(bytesToIntLe(raw, 6, 1) * 0.2) + pow2(bytesToIntLe(raw, 7, 1) * 0.1))` |
Yaw rate | 312 | `bytesToIntLe(raw, 4, 2) * -0.2725` | Calibrated against the gyroscope in RaceBox Mini. Gen1 used 0.286478897 instead.

---

If you found this page useful, consider donating so I can buy beer/boba:
 
[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/donate?business=ZKULAWZFJKCES&item_name=Donation+to+support+the+RaceChronoDiyBleDevice+project+on+GitHub&currency_code=USD)
