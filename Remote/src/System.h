#ifndef SYSTEM_H
#define SYSTEM_H
#include <Arduino.h>
#include <FreeRTOS.h>

void System_Init();
void SystemLoop(void* pvParameters);

extern SemaphoreHandle_t xMutex;

typedef struct SystemState {
  volatile uint32_t batteryMv;
  volatile bool channelLocked;
  volatile bool isSynced;
  volatile bool isEstopped;
  volatile bool heartbeatActive;
  volatile unsigned long uptimeSeconds;
  volatile bool OLEDActive;
  volatile uint16_t potChannel;
} SystemState;

extern SystemState state;

#endif