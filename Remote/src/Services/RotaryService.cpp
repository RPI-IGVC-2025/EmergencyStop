#include "RotaryService.h"

#include <Arduino.h>

#include "Algos/RotaryEncoder.h"

int rotaryCount = 0;
int* rotaryCnt = &rotaryCount;

void RotaryService_Init() {
    rotaryEncoder_Init();
}

void RotaryServiceLoop(void* pvParameters) {
    for (;;) {
        rotaryCount = getRotaryPos();
        Serial.println(rotaryCount);
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}