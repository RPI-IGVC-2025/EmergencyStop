#ifndef HANDSHAKESERVICE_H
#define HANDSHAKESERVICE_H
#include <Arduino.h>
#include "Algos/PacketBuilder.h"
#include <FreeRTOS.h>

void HandshakeServiceLoop(void* pvParameters);

static TaskHandle_t HandshakeServiceTask;

void HandshakeService_Init();

#endif