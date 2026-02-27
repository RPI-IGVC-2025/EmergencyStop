#include <string.h>
#include "PacketBuilder.h"
#include "BLAKE.h"

int minNonce = 0;
uint32_t counter = 0;
uint16_t noncePool[10];
int nonceIndex = 0;
unsigned long seed = esp_random();  // Initial seed value for random numbers;

void buildPacket(Packet* pkt, MessageType type, StatusCode code, uint8_t channel) {
  // Actual Command being sent

  pkt->command = (uint8_t)type;

  // Numbers, Sequence and Nonces
  pkt->syncByte = SYNC_BYTE;
  pkt->footByte = FOOT_BYTE;

  pkt->seqNum = counter;
  pkt->nonce = generateNonce();
  counter++;

  pkt->status = (uint8_t)code;

  if(IS_REMOTE) {
    pkt->ownerID = OWNER_ID_REMOTE;
  } else {
    pkt->ownerID = OWNER_ID_ROBOT;
  }

  pkt->channel = channel;

  // Hashing
  generateBLAKE(pkt);
}

// Overloaded function for when no channel is needed
void buildPacket(Packet* pkt, MessageType type, StatusCode code) {
  buildPacket(pkt, type, code, 0);
}


// Methods for Nonces and Sequencing

unsigned long customRandom() {
  // LCG formula: X_n+1 = (a * X_n + c) mod m
  // Common values for a, c, and m are used here.
  // These values should be carefully chosen for good randomness properties.
  seed = (seed * 1664525 + 1013904223) & 0xFFFFFFFF;
  // % 4294967296; // Modulo 2^32
  return seed;
}

// Function to get a pseudo-random number within a specified range
unsigned long customRandomRange(unsigned long minVal, unsigned long maxValExclusive) {
  unsigned long randomNumber = customRandom();
  return minVal + (randomNumber % (maxValExclusive - minVal));
}

void resetNonces() {
  for (int i = 0; i < 10; i++) {
    noncePool[i] = minNonce + i;
  }
  // Shuffle the pool
  for (int i = 9; i > 0; i--) {
    int j = customRandomRange(0, i);
    if (j > i) j = i;
    uint16_t temp = noncePool[i];
    noncePool[i] = noncePool[j];
    noncePool[j] = temp;
  }
  nonceIndex = 0;
}

uint32_t generateNonce() {
  if (nonceIndex >= 10) {
    minNonce += 10;
    resetNonces();
  }
  return noncePool[nonceIndex++];
}
