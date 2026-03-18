#include <Arduino.h>

#include "EStopService.h"

void EStopService_Init() {
    xTaskCreatePinnedToCore(
        EStopServiceLoop,   /* Task function. */
        "EStopServiceTask", /* name of task. */
        4096,                   /* Stack size */
        NULL, 10,               /* Medium Priority out of all 3 tasks */
        &EStopServiceTask,  /* Task handle to keep track of created task */
        0);                     /* pin task to core 1 */
    
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