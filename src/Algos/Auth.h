#ifndef AUTH_H
#define AUTH_H
#include <Arduino.h>
#include "PacketBuilder.h"

bool verifyPacket(Packet* pkt);
bool verifySequence(Packet* pkt);
bool verifyNonce(Packet* pkt);
bool verifyHash(Packet* pkt);

#endif