#include <Arduino.h>

#include "Handlers/RadioHandler.h"
#include "HC12Service.h"
#include "System.h"

void SelectChannelServiceLoop(void* pvParameters);

void SelectChannelService_Init() {
    xTaskCreatePinnedToCore(
        SelectChannelServiceLoop,   /* Task function. */
        "SelectChannelService", /* name of task. */
        4096,       /* Stack size */
        NULL,
        7,                /* Highest Priority out of all 3 tasks */
        &SelectChannelServiceTask, /* Task handle to keep track of created task */
        0);               /* pin task to core 0 */
}

void SelectChannelServiceLoop(void* pvParameters) {
    int initialADC = state.potChannel;
    for(;;) {
        if(state.potChannel != initialADC) {
            
        }
    }
}