#ifndef PACKETBUILDER_H
#define PACKETBUILDER_H
#include <Arduino.h>

const int SYNC_BYTE = 0x17; // Example sync byte
const int FOOT_BYTE = 0x9C; // Example foot byte

typedef struct __attribute__((packed)) Packet {
  uint8_t syncByte; // A constant value to indicate the start of a packet
  uint8_t command;   // ESTOP, etc.
  uint32_t seqNum;   // The raw sequence count
  uint32_t nonce;    // Your 10*x logic value
  uint8_t hash[32];  // BLAKE2s output of (command + seq + nonce)
  uint8_t status;    // Used for diagnostics (0x00 = OK, 0x01 = Button Press, 0x02 = Heartbeat failure, 0x03 = HC-12 Failure, etc.)'
  uint8_t footByte; // A constant value to indicate the end of a packet
} Packet;

enum MessageType {
  ESTOP = 'S',
  HEARTBEAT = 'C',
  HANDSHAKE = 'H'
  // ...
};

enum class StatusCode : uint8_t {
  OK = 0x00,
  BUTTON_PRESS = 0x01,
  HEARTBEAT_FAILURE = 0x02,
  HC12_FAILURE = 0x03
};

void buildPacket(Packet* pkt, MessageType type, StatusCode code);
void resetNonces();
uint32_t generateNonce();

#endif