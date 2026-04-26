#ifndef ROTARYSERVICE_H
#define ROTARYSERVICE_H
#include <Arduino.h>
#include <FreeRTOS.h>

static TaskHandle_t RotaryServiceTask;

void RotaryService_Init();
void RotaryServiceLoop(void* pvParameters);



#endif