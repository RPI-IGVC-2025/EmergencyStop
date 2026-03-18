#ifndef CHANNELSERVICE_H
#define CHANNELSERVICE_H
#include <Arduino.h>
#include "Algos/PacketBuilder.h"

struct ChannelServiceStatus {
    bool confirmSelection;
};

static TaskHandle_t SelectChannelServiceTask;
//static TaskHandle_t HandshakeServiceTask;

void SelectChannelServiceLoop(void* pvParameters);

void SelectChannelService_Init();
bool getSelectionStatus();

#endif