// Customizations for 2nd generation Subaru BRZ / Toyota GR86
// (2022+ model years).

#if !defined(RACECHRONO_BIY_BLE_DEVICE_FT86_GEN2_H)
#define RACECHRONO_BIY_BLE_DEVICE_FT86_GEN2_H)

uint8_t getUpdateRateDivider(uint32_t can_id) {
  // This is sent over the CAN bus 50 times per second and includes brake
  // system pressure. It's useful to have this at the highest update rate
  // possible, as braking can be very short, e.g. at autocross.
  if (can_id == 313) {
    return 1;
  }

  // This is sent over the CAN bus 10 times per second, we want 1.
  if (can_id == 837) {
    return 10;
  }

  // These are sent over the CAN bus 100 times per second, we want 25.
  if (can_id < 0x100) {
    return 4;
  }

  // These are sent over the CAN bus 50 times per second, we want 25.
  if (can_id < 0x200) {
    return 2;
  }

  // OBD responses should be rare, don't limit them at all if we're listening to
  // them.
  if (can_id > 0x700) {
    return 1;
  }

  return DEFAULT_UPDATE_RATE_DIVIDER;
}

#endif // RACECHRONO_BIY_BLE_DEVICE_FT86_GEN1_H
