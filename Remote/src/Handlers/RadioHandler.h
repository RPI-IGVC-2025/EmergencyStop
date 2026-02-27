#ifndef RADIOHANDLER_H
#define RADIOHANDLER_H
#include <Arduino.h>
#include "Algos/PacketBuilder.h"

struct RadioState {
    volatile uint16_t desiredChannel;
    volatile uint16_t currentChannel;
};

void Radio_Init();
bool getNextFrame(Packet* outPkt);
void HC12switchChannel(uint16_t newChannel);
void HC12setDefault();
void HC12sendCommand(char* command);

uint16_t getCurrentChannel();
void setDesiredChannel(uint16_t newChannel);

void sendPacket(Packet* pkt);
void clearPacket(Packet* pkt);

#endif