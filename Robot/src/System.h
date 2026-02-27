#ifndef SYSTEM_H
#define SYSTEM_H
#include <Arduino.h>
#include <FreeRTOS.h>

void System_Init();
void SystemLoop(void* pvParameters);

extern SemaphoreHandle_t xMutex;

typedef struct SystemState {
  volatile uint32_t batteryMv;
  volatile bool isSynced;
  volatile bool isEstopped;
  volatile unsigned long uptimeSeconds;
  volatile bool radioReady;
} SystemState;

extern SystemState state;

#endif