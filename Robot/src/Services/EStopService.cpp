#include <Arduino.h>
#include "EStopService.h"

#include "Handlers/EStopHandler.h"
#include "Handlers/HeartbeatHandler.h"
#include "System.h"

void EStopService_Init() {

}

void EStopServiceLoop(void* pvParameters) {
    for(;;) {
        if(getLastHeartbeatTime() - xTaskGetTickCount() >= pdMS_TO_TICKS(3000)) {
            triggerEStop();
        }

        vTaskDelay(pdMS_TO_TICKS(25));
    }
}