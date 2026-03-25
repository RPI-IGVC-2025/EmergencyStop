#ifndef OPERATIONSERVICE_H
#define OPERATIONSERVICE_H
#include <Arduino.h>
#include "Algos/PacketBuilder.h"
#include <FreeRTOS.h>

void OperationServiceLoop(void* pvParameters);

static TaskHandle_t OperationServiceTask;

void OperationService_Init();
bool checkIncomingPacket(Packet* pkt);


#endif