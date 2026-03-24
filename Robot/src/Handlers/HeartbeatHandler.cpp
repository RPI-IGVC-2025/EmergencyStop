#include <Arduino.h>

#include "HeartbeatHandler.h"

HeartbeatData hbData = {
    .lastHeartbeatTime = 0
};

void updateLastHeartbeatTime(TickType_t newTime) {
    hbData.lastHeartbeatTime = newTime;
}

TickType_t getLastHeartbeatTime() {
    return hbData.lastHeartbeatTime;
}