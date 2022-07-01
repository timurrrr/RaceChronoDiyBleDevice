// Customizations for BMW e46
// (1999-2005 model years).

#if !defined(RACECHRONO_BIY_BLE_DEVICE_E46_H)
#define RACECHRONO_BIY_BLE_DEVICE_E46_H)

uint8_t getUpdateRateDivider(uint32_t can_id) {
  // This is sent over the CAN bus 140 times per second and carries:
  // vehicle speed
  // Let's go for ~10 per second
  if (can_id == 0x153) {
    return 14;
  }

  // This is sent over the CAN bus 140 times per second and carries:
  // individual wheel speeds 
  // we want ~10.
  if (can_id == 0x1F0) {
    return 14;
  }

  // This is sent over the CAN bus 100 times per second and carries:
  // engine RPM 
  // we want ~10.
  if (can_id == 0x1F0) {
    return 10;
  }

  // This is sent over the CAN bus 100 times per second and carries:
  // Steering angle 
  // we want ~10.
  if (can_id == 0x1F5) {
    return 10;
  }

  // This is sent over the CAN bus 100 times per second and carries:
  // Coolant temp
  // Ambient pressure
  // Throttle Position
  // we want ~10.
  if (can_id == 0x329) {
    return 10;
  }

  // This is sent over the CAN bus 100 times per second and carries:
  // oil temp 
  // we want ~1.
  if (can_id == 0x545) {
    return 100;
  }

  // This is sent over the CAN bus 5 times per second and carries:
  // odometer
  // fuel level 
  // we want ~1.
  if (can_id == 0x613) {
    return 5;
  }

  // This is sent over the CAN bus 5 times per second and carries:
  // ambient air temp 
  // we want ~1.
  if (can_id == 0x615) {
    return 5;
  }

  // OBD responses should be rare, don't limit them at all if we're listening to
  // them.
  if (can_id > 0x700) {
    return 1;
  }

  return DEFAULT_UPDATE_RATE_DIVIDER;
}

#endif // RACECHRONO_BIY_BLE_DEVICE_E46_H
