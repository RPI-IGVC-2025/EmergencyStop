#ifndef HC12SERVICE_H
#define HC12SERVICE_H
#include <Arduino.h>
#include "Algos/PacketBuilder.h"

struct ChannelServiceStatus {
    int timeRemaining;
};

static TaskHandle_t SelectChannelServiceTask;
//static TaskHandle_t HandshakeServiceTask;

void SelectChannelService_Init();
int getChannelSelectionTimeRemaining();

#endif