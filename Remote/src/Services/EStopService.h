#ifndef ESTOPSERVICE_H
#define ESTOPSERVICE_H
#include <Arduino.h>
#include <FreeRTOS.h>
#include "Algos/PacketBuilder.h"

const int ESTOP_PIN = 7;

void EStopService_Init();
void EStopServiceLoop(void* pvParameters);

static TaskHandle_t EStopServiceTask;

#endif