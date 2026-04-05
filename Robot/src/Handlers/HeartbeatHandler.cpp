#include "HeartbeatHandler.h"

#include <Arduino.h>

HeartbeatData hbData = {
    .lastHeartbeatTime = 0,
    .heartbeatActive = false};

void updateLastHeartbeatTime(TickType_t newTime) {
    hbData.lastHeartbeatTime = newTime;
}

TickType_t getLastHeartbeatTime() {
    return hbData.lastHeartbeatTime;
}

void setHeartbeatActive() {
    if (!hbData.heartbeatActive) {
        hbData.heartbeatActive = true;
    }
}

bool getHeartbeatActive() {
    return hbData.heartbeatActive;
}