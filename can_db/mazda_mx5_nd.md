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
Brake position (%) | 120 | `min(max(E + if(E <= 146, 255, 0) - 156, 0)/2.34, 100)` | See note below
Steering angle | 134 | `(16000 - bytesToUInt(raw, 0, 2)) * 0.1` | Positive value = turning right. You can add a `-` if you prefer it the other way around.
Speed | 514 | `bytesToInt(raw, 2, 2) / 360.0` |
Engine RPM | 514 | `bytesToUInt(raw, 0, 2) / 4` | If someone records >8192 rpms to confirm, let me know :)

*Note*
The binary representation used for the brake pedal/pressure is very weird.
In my limited testing, I've observed the values of 149...154 when not touching
the brake pedal. Most of the time it stays as 154. When progressively pressing
the brake pedal, the number goes up to 255, then wraps around to 0, and keeps
going up. I've observed values as high as 145 when heavy braking.
It feels very much like
`(cast_to_int(brake_pressure * multiplier) + offset) % 255`, as long as we allow
the `brake_pressure` to go slightly negative, and as long as the binary
representation for the maximum possible value and the binary representation for
the minimal possible values don't conflict with each other.

Now out of all 0...255 possible values, this leaves us with only 146...148
values that I haven't observed. For now, I've made an assumption that 146 is
"braking even harder than I was ever able to brake", and 147 and 148 are just
values "even more negative than I was able to observe". It is recommended to log
the `E` byte by itself in a separate channel (e.g. "Digital 1") for diagnostic
purposes so that if you ever see the "brake position" jump from 0% to 100% when
you're not braking, or from 100% to 0% when you're braking super hard, we can
look and tweak the equation.

### Would be nice to find CAN IDs for ...

Essentials: fuel level, coolant temperature, clutch pedal, current gear.

Advanced: tire pressures, tire temperatures, lat/long acceleration (if
available), yaw rate, individual wheel speeds.
