#include <Arduino.h>
#include "OperationService.h"
#include "System.h"

void OperationService_Init() {

}

void OperationServiceLoop(void* pvParameters) {
    for(;;) {
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            
        }

        xSemaphoreGive(xMutex); 
        vTaskDelay(pdMS_TO_TICKS(25));
    }
}