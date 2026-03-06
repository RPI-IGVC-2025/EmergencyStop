#ifndef HANDSHAKESERVICE_H
#define HANDSHAKESERVICE_H
#include <Arduino.h>
#include <FreeRTOS.h>
#include "Algos/PacketBuilder.h"

void HandshakeService_Init();
void HandshakeServiceLoop(void* pvParameters);
bool checkIncomingPacket(Packet* pkt);

static TaskHandle_t HandshakeServiceTask;

#endif