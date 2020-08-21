#include <CAN.h>
#include <RaceChrono.h>

// Connections:
//  BOARD | MCP2515
//    USB   VCC
//    GND   GND
//  Pin 7   CS
//  Pin 9   INT  // Unconnected is fine too, we don't use interrupts here.
//     MO   SI
//     MI   SO
//    SCK   SCK
const int CS_PIN = 7;
const int IRQ_PIN = 9;

const long QUARTZ_CLOCK_FREQUENCY = 16 * 1E6;  // 16 MHz.
const uint32_t SPI_FREQUENCY = 8 * 1E6;  // 8 MHz.
const long BAUD_RATE = 500 * 1E3;  // 500k.

bool isCanBusReaderActive = false;
long lastCanMessageReceivedMs;

// We use RaceChronoPidMap to keep track of stuff for each PID.
// In this implementation, we're going to ignore "update intervals" requested by
// RaceChrono, and instead send every Nth CAN message we receive, per PID, where
// N is different for different PIDs.
struct PidExtra {
  // Only send one out of |updateRateDivider| packets per PID.
  // By default, send 1 out of 10 messages for each PID.
  uint8_t updateRateDivider = 10;

  // Varies between 0 and |updateRateDivider - 1|.
  uint8_t skippedUpdates = 0;
};
RaceChronoPidMap<PidExtra> pidMap;

uint32_t loop_iteration = 0;

// Forward declarations to help put code in a natural reading order.
void waitForConnection();
void bufferNewPacket(uint32_t pid, uint8_t *data, uint8_t data_length);
void handleOneBufferedPacket();
void flushBufferedPackets();
void resetSkippedUpdatesCounters();

void dumpMapToSerial() {
  Serial.println("Current state of the PID map:");

  uint16_t updateIntervalForAllEntries;
  bool areAllPidsAllowed =
      pidMap.areAllPidsAllowed(&updateIntervalForAllEntries);
  if (areAllPidsAllowed) {
    Serial.print("  All PIDs are allowed.");
  }

  if (pidMap.isEmpty()) {
    if (areAllPidsAllowed) {
      Serial.println("  Map is empty.");
      Serial.println("");
    } else {
      Serial.println("  No PIDs are allowed.");
      Serial.println("");
    }
    return;
  }

  struct {
    void operator() (void *entry) {
      uint32_t pid = pidMap.getPid(entry);
      const PidExtra *extra = pidMap.getExtra(entry);

      Serial.print("  ");
      Serial.print(pid);
      Serial.print(" (0x");
      Serial.print(pid, HEX);
      Serial.print("), sending 1 out of ");
      Serial.print(extra->updateRateDivider);
      Serial.println(" messages.");
    }
  } dumpEntry;
  pidMap.forEach(dumpEntry);

  Serial.println("");
}

class UpdateMapOnRaceChronoCommands : public RaceChronoBleCanHandler {
public:
  void allowAllPids(uint16_t updateIntervalMs) {
    Serial.print("Command: ALLOW ALL PIDS, update interval: ");
    Serial.print(updateIntervalMs);
    Serial.println(" ms.");

    pidMap.allowAllPids(updateIntervalMs);

    dumpMapToSerial();
  }

  void denyAllPids() {
    Serial.println("Command: DENY ALL PIDS.");

    pidMap.reset();

    dumpMapToSerial();
  }

  void allowPid(uint32_t pid, uint16_t updateIntervalMs) {
    Serial.print("Command: ALLOW PID ");
    Serial.print(pid);
    Serial.print(" (0x");
    Serial.print(pid, HEX);
    Serial.print("), requested update interval: ");
    Serial.print(updateIntervalMs);
    Serial.println(" ms.");

    if (!pidMap.allowOnePid(pid, updateIntervalMs)) {
      Serial.println("WARNING: unable to handle this request!");
    }

    // Customization for Subaru BRZ / Toyota 86 / Scion FR-S:
    void *entry = pidMap.getEntryId(pid);
    if (entry != nullptr) {
      PidExtra *pidExtra = pidMap.getExtra(entry);
      switch (pid) {
      case 0xD0:
      case 0xD1:
        pidExtra->updateRateDivider = 2;
        pidExtra->skippedUpdates = 0;
        break;
      case 0x140:
        pidExtra->updateRateDivider = 4;
        pidExtra->skippedUpdates = 0;
        break;
       case 0x360:
        pidExtra->updateRateDivider = 20;
        pidExtra->skippedUpdates = 0;
        break;
      }
    }

    dumpMapToSerial();
  }

  void handleDisconnect() {
    Serial.println("Resetting the map.");

    pidMap.reset();

    dumpMapToSerial();
  }
} raceChronoHandler;


void setup() {
  uint32_t startTimeMs = millis();
  Serial.begin(115200);
  while (!Serial && millis() - startTimeMs < 5000) {
  }

  Serial.println("Setting up BLE...");
  RaceChronoBle.setUp("BLE CAN device demo", &raceChronoHandler);
  RaceChronoBle.startAdvertising();

  Serial.println("BLE is set up, waiting for an incoming connection.");
  waitForConnection();
}

void waitForConnection() {
  uint32_t iteration = 0;
  bool lastPrintHadNewline = false;
  while (!RaceChronoBle.waitForConnection(1000)) {
    Serial.print(".");
    if ((++iteration) % 10 == 0) {
      lastPrintHadNewline = true;
      Serial.println();
    } else {
      lastPrintHadNewline = false;
    }
  }

  if (!lastPrintHadNewline) {
    Serial.println();
  }

  Serial.println("Connected.");
}

bool startCanBusReader() {
  Serial.println("Connecting to the CAN bus...");
  CAN.setClockFrequency(QUARTZ_CLOCK_FREQUENCY);
  CAN.setSPIFrequency(SPI_FREQUENCY);
  CAN.setPins(CS_PIN, IRQ_PIN);

  boolean result = CAN.begin(BAUD_RATE, /* stayInConfigurationMode= */ true);
  if (!result) {
    Serial.println("ERROR: Unable to start the CAN bus listener.");
    return false;
  }

  Serial.println("Success!");

  // These values are customized for Subaru BRZ / Toyota 86 / Scion FR-S.
  // TODO: generalize this? Figure out a good way to build this from the list of
  // requested PIDs?
  if (!CAN.setFilterRegisters(
    /* mask0= */   0b11111111111 /* full match only */,
    /* filter0= */ 0xD1,
    /* filter1= */ 0x140,

    /* mask1= */   0b11111111111 /* full match only */,
    /* filter2= */ 0xD0,
    /* filter3= */ 0x360,
    /* filter4= */ 0x360,  // Repeated filters don't matter.
    /* filter5= */ 0x360,
    /* allowRollover= */ false)) {
    Serial.println("WARNING: Unable to set filter registers.");
    Serial.println("Trying to continue without filtering...");
    if (!CAN.switchToNormalMode()) {
      Serial.println("WARNING: Unable to switch to normal mode!");
      return false;
    }
  }

  isCanBusReaderActive = true;
  return true;
}

void stopCanBusReader() {
  CAN.end();
  isCanBusReaderActive = false;
}

void loop() {
  loop_iteration++;

  // First, verify that we have both Bluetooth and CAN up and running.

  // Not clear how heavy is the Bluefruit.connected() call. Only check the
  // connectivity every 100 iterations to avoid stalling the CAN bus loop.
  if ((loop_iteration % 100) == 0 && !Bluefruit.connected()) {
    Serial.println("RaceChrono disconnected!");
    raceChronoHandler.handleDisconnect();
    stopCanBusReader();

    Serial.println("Waiting for a new connection.");
    waitForConnection();
  }

  while (!isCanBusReaderActive) {
    if (startCanBusReader()) {
      flushBufferedPackets();
      resetSkippedUpdatesCounters();
      lastCanMessageReceivedMs = millis();
      break;
    }
    delay(1000);
  }

  uint32_t timeNowMs = millis();

  // I've observed that MCP2515 has a tendency to just stop responding for some
  // weird reason. Just kill it if we don't have any data for 50 ms. The timeout
  // might need to be tweaked for some cars.
  if (timeNowMs - lastCanMessageReceivedMs > 50) {
    Serial.println("ERROR: CAN bus timeout, aborting.");
    stopCanBusReader();
    return;
  }

  // Sending data over Bluetooth takes time, and MCP2515 only has two buffers
  // for received messages. Once a buffer is full, further messages are dropped.
  // Instead of relying on the MCP2515 buffering, we aggressively read messages
  // from the MCP2515 and put them into our memory.
  //
  // TODO: It might be more efficient to use interrupts to read data from
  // MCP2515 as soon as it's available instead of polling all the time. The
  // interrupts don't work out of the box with nRF52 Adafruit boards though, and
  // need to be handled carefully wrt synchronization between the interrupt
  // handling and processing of received data.
  int packetSize;
  while ((packetSize = CAN.parsePacket()) > 0) {
    if (CAN.packetRtr()) {
      // Ignore RTRs as they don't contain any data.
      continue;
    }

    uint32_t pid = CAN.packetId();
    uint8_t data[8];
    int data_length = 0;
    while (data_length < packetSize && data_length < sizeof(data)) {
      int byte_read = CAN.read();
      if (byte_read == -1) {
        break;
      }

      data[data_length++] = byte_read;
    }

    if (data_length == 0) {
      // Nothing to send here. Can this even happen?
      continue;
    }

    bufferNewPacket(pid, data, data_length);
    lastCanMessageReceivedMs = millis();
  }

  handleOneBufferedPacket();
}

struct BufferedMessage {
  uint32_t pid;
  uint8_t data[8];
  uint8_t length;
};

// Circular buffer to put received messages, used to buffer messages in memory
// (which is relatively abundant) instead of relying on the very limited
// buffering ability of the MCP2515.
const uint8_t NUM_BUFFERS = 16;  // Must be a power of 2, but less than 256.
BufferedMessage buffers[NUM_BUFFERS];
uint8_t bufferToWriteTo = 0;
uint8_t bufferToReadFrom = 0;

void bufferNewPacket(uint32_t pid, uint8_t *data, uint8_t data_length) {
  if (bufferToWriteTo - bufferToReadFrom == NUM_BUFFERS) {
    Serial.println("WARNING: Receive buffer overflow, dropping one message.");

    // In case of a buffer overflow, drop the oldest message in the buffer, as
    // it's likely less useful than the newest one.
    bufferToReadFrom++;
  }

  BufferedMessage *message = &buffers[bufferToWriteTo % NUM_BUFFERS];
  message->pid = pid;
  memcpy(message->data, data, data_length);
  message->length = data_length;
  bufferToWriteTo++;
}

void handleOneBufferedPacket() {
  if (bufferToReadFrom == bufferToWriteTo) {
    // No buffered messages.
    return;
  }

  BufferedMessage *message = &buffers[bufferToReadFrom % NUM_BUFFERS];
  uint32_t pid = message->pid;
  void *entry = pidMap.getEntryId(pid);
  if (entry != nullptr) {
    // TODO: we could do something smart here. For example, if there are more
    // messages pending with the same PID, we could count them towards
    // |skippedUpdates| in a way that we only send the latest one, but maintain
    // roughly the desired rate.
    PidExtra *extra = pidMap.getExtra(entry);
    if (extra->skippedUpdates == 0) {
      RaceChronoBle.sendCanData(pid, message->data, message->length);
    }

    extra->skippedUpdates++;
    if (extra->skippedUpdates >= extra->updateRateDivider) {
      // The next message with this PID will be sent.
      extra->skippedUpdates = 0;
    }
  }

  bufferToReadFrom++;
}

void flushBufferedPackets() {
  bufferToWriteTo = 0;
  bufferToReadFrom = 0;
}

void resetSkippedUpdatesCounters() {
  struct {
    void operator() (void *entry) {
      PidExtra *extra = pidMap.getExtra(entry);
      extra->skippedUpdates = 0;
    }
  } resetSkippedUpdatesCounter;
  pidMap.forEach(resetSkippedUpdatesCounter);
}