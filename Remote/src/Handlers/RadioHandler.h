#ifndef RADIOHANDLER_H
#define RADIOHANDLER_H
#include <Arduino.h>
#include "Algos/PacketBuilder.h"

struct RadioState {
    volatile uint16_t currentChannel;
};

void Radio_Init();
void processValidatedPacket(Packet *pkt);
bool getNextFrame(Packet* outPkt);

#endif