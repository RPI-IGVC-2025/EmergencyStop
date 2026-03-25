#ifndef ESTOPSERVICE_H
#define ESTOPSERVICE_H
#include <Arduino.h>
#include <FreeRTOS.h>
#include "Algos/PacketBuilder.h"

void EStopService_Init();
void EStopServiceLoop(void* pvParameters);

void triggerEStop();

static TaskHandle_t EStopServiceTask;

#endif