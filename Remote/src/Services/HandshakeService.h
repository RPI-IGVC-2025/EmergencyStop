#ifndef HANDSHAKESERVICE_H
#define HANDSHAKESERVICE_H
#include <Arduino.h>
#include <FreeRTOS.h>

void HandshakeService_Init();
void HandshakeServiceLoop(void* pvParameters);

static TaskHandle_t HandshakeServiceTask;

#endif