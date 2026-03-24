#include <Arduino.h>
#include "EStopService.h"

#include "Handlers/HeartbeatHandler.h"

void EStopService_Init() {

}

void EStopServiceLoop(void* pvParameters) {
    for(;;) {
        if(getLastHeartbeatTime() - xTaskGetTickCount() >= pdMS_TO_TICKS(3000)) {
            
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}