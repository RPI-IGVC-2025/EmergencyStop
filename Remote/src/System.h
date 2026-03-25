#ifndef SYSTEM_H
#define SYSTEM_H
#include <Arduino.h>
#include <FreeRTOS.h>

const int ESTOP_PIN = 7;
const int SELECTION_PIN = 6;
const int POTENTIOMETER_PIN = 4;

void System_Init();
void SystemLoop(void* pvParameters);

extern SemaphoreHandle_t xMutex;

enum SystemState {
    STATE_BOOTING,
    STATE_SELECTING_CHANNEL,
    STATE_HANDSHAKING,
    STATE_OPERATIONAL,
    STATE_ESTOP_ACTIVE
};

void transitionTo(SystemState newState);

extern SystemState remoteState;

typedef struct SystemData {
  volatile bool EStopActive;
  volatile bool EStopPrimed;
  volatile uint32_t batteryMv;
  volatile bool channelLocked;
  volatile bool isSynced;
  volatile bool isEstopped;
  volatile bool heartbeatActive;
  volatile unsigned long uptimeSeconds;
  volatile bool OLEDActive;
  volatile uint16_t potChannel;
  volatile bool radioReady;
} SystemData;

extern SystemData data;

#endif