# RaceChronoDiyBleDevice
DIY BLE device for RaceChrono, currently supports reading data from the CAN bus.

There are some optimizations in the code that are specific to the FT86 platform
cars (Subaru BRZ, Toyota 86, Scion FR-S), but it should be straightforward to
tweak the code for other cars.

## Supported Hardware

* Adafruit Feather nRF52832
* Adafruit ItsyBitsy nRF52840 Express
* 16 MHz MCP2515 breakout boards (probably MCP25625 as well)

## Prerequisites

You will need to install two libraries for Arduino:
```sh
cd ~/Documents/Arduino/libraries/  # ~/Arduino/libraries on Mac OS
git clone https://github.com/timurrrr/arduino-CAN CAN
git clone https://github.com/timurrrr/arduino-RaceChrono arduino-RaceChrono
```

It's important that you don't use the arduino-CAN library available in the
Arduino IDE built-in library manager, as it has multiple bugs, and many
operations are implemented in an ineffective way. My pull requests to address
those have not been reviewed at the time of writing.

## Testing

You don't need to always be in the car to test changes to this library. Instead,
you can use the FakeSubaruBRZ example from my fork of the `arduino-CAN` library
on a second Arduino board, and connect the two boards into a small CAN network.
