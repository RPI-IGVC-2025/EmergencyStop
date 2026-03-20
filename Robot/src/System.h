#ifndef SYSTEM_H
#define SYSTEM_H
#include <Arduino.h>
#include <FreeRTOS.h>

void System_Init();
void SystemLoop(void* pvParameters);

extern SemaphoreHandle_t xMutex;

typedef struct SystemData {
  bool IS_REMOTE;
  volatile uint32_t batteryMv;
  volatile bool isSynced;
  volatile bool isEstopped;
  volatile unsigned long uptimeSeconds;
  volatile bool radioReady;
} SystemData;

extern SystemData data;

 enum SystemState {
    STATE_BOOTING,
    STATE_HANDSHAKING,
    STATE_OPERATIONAL,
    STATE_ESTOP_ACTIVE
};

extern SystemState robotState;

void transitionTo(SystemState newState);

#endif