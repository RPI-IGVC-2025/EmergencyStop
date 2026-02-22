#ifndef SYSTEM_H
#define SYSTEM_H
#include <Arduino.h>
#include <FreeRTOS.h>

void System_Init();
void SystemLoop(void* pvParameters);

SemaphoreHandle_t xMutex = xSemaphoreCreateMutex();

typedef struct SystemState {
  volatile uint32_t batteryMv;
  volatile bool isSynced;
  volatile bool isEstopped;
  volatile bool heartbeatActive;
  volatile unsigned long uptimeSeconds;
} SystemState;

SystemState state;

#endif