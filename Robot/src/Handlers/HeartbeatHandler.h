#ifndef HEARTBEATHANDLER_H
#define HEARTBEATHANDLER_H
#include <Arduino.h>
#include "Algos/PacketBuilder.h"
#include <FreeRTOS.h>
#include <task.h>

struct HeartbeatData {
    volatile TickType_t lastHeartbeatTime;
};

void updateLastHeartbeatTime(TickType_t newTime);
TickType_t getLastHeartbeatTime();

#endif