#ifndef BLAKE_H
#define BLAKE_H
#include <Arduino.h>
#include "PacketBuilder.h"

struct SecretKey {
  const char* key;
  uint16_t length;
};

void generateBLAKE(Packet* pkt);

#endif