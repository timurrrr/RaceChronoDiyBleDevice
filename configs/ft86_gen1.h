// Customizations for 1st generation Subaru BRZ / Toyota 86 / Scion FR-S
// (2013-2020 model years).

#if !defined(RACECHRONO_BIY_BLE_DEVICE_FT86_GEN1_H)
#define RACECHRONO_BIY_BLE_DEVICE_FT86_GEN1_H

const long BAUD_RATE = 500 * 1e3;  // 500k.

uint8_t getUpdateRateDivider(uint32_t can_id) {
  switch (can_id) {
    // This is sent over the CAN bus 50 times per second and includes brake
    // system pressure. It's useful to have this at the highest update rate
    // possible, as braking can be very short, e.g. at autocross.
    case 0xD1:
      return 1;

    // These are sent over the CAN bus 100 times per second, we want 25.
    case 0x18:
    case 0x140:
    case 0x141:
    case 0x142:
      return 4;

    // These are sent over the CAN bus 50 times per second, we want 25.
    case 0xD0:
    case 0xD2:
    case 0xD3:
    case 0xD4:
    case 0x144:
    case 0x152:
    case 0x156:
    case 0x280:
      return 2;

    // 0x360 is sent over the CAN bus 20 times per second, we want 1.
    case 0x360:
      return 20;

    default:
      return DEFAULT_UPDATE_RATE_DIVIDER;
  }
}

#endif // RACECHRONO_BIY_BLE_DEVICE_FT86_GEN1_H
