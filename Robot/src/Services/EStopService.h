#ifndef ESTOPSERVICE_H
#define ESTOPSERVICE_H
#include <Arduino.h>
#include "Algos/PacketBuilder.h"
#include <FreeRTOS.h>

void EStopServiceLoop(void* pvParameters);

static TaskHandle_t EStopServiceTask;

void EStopService_Init();


#endif