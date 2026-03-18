#ifndef RADIOHANDLER_H
#define RADIOHANDLER_H
#include <Arduino.h>
#include "Algos/PacketBuilder.h"

struct RadioState {
    volatile uint8_t desiredChannel;
    volatile uint8_t currentChannel;
};

void Radio_Init();
bool getNextFrame(Packet* outPkt);
bool HC12setDefault();
bool HC12switchChannel(uint8_t newChannel);
char* HC12sendCommand(char* command);

uint8_t getCurrentChannel();
void setDesiredChannel(uint8_t newChannel);
uint8_t getDesiredChannel();
void setCurrentChannel(uint8_t newChannel);

void sendPacket(Packet* pkt);
void clearPacket(Packet* pkt);

#endif