#include "RotaryService.h"

#include <Arduino.h>

#include "Algos/RotaryEncoder.h"

void RotaryService_Init() {
    rotaryEncoder_Init();
}

void RotaryServiceLoop(void* pvParameters) {
    for (;;) {
        setRotaryPos();
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}