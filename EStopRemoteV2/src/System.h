#ifndef SYSTEM_H
#define SYSTEM_H
#include <Arduino.h>
#include <FreeRTOS.h>

SemaphoreHandle_t xMutex = xSemaphoreCreateMutex();

typedef struct SystemState {
  volatile uint32_t batteryMv;
  volatile bool isSynced;
  volatile bool isEstopped;
  volatile bool heartbeatActive;
  volatile uint32_t uptimeSeconds;
} Packet;

SystemState state;

#endif