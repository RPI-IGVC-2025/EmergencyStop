#include <Arduino.h>

#include "EStopService.h"
#include "System.h"

void EStopService_Init() {    
    pinMode(ESTOP_PIN, INPUT_PULLUP);
}

void EStopServiceLoop(void* pvParameters) {
    for(;;) {
        if(digitalRead(ESTOP_PIN) == LOW) {
            Serial.println("EStopPressed");
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}