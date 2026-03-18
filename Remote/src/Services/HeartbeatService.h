#ifndef HEARTBEATSERVICE_H
#define HEARTBEATSERVICE_H
#include <Arduino.h>
#include <FreeRTOS.h>
#include "Algos/PacketBuilder.h"

struct HeartbeatState {
    volatile bool heartbeatEnabled;
    volatile TickType_t lastHeartbeatPacket;
};

static TaskHandle_t HeartbeatServiceTask;

void HeartbeatService_Init();
void HeartbeatServiceLoop(void* pvParameters);

#endif