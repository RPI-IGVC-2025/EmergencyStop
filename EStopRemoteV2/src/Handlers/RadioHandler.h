#ifndef RADIOHANDLER_H
#define RADIOHANDLER_H
#include <Arduino.h>
#include "Packet.h"

void initRadio();
void processValidatedPacket(Packet *pkt);
bool getNextFrame(Packet* outPkt);

#endif