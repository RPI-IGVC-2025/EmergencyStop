#include "HandshakeService.h"

#include <Arduino.h>

#include "Handlers/RadioHandler.h"
#include "System.h"

void HandshakeService_Init() {
    xTaskCreatePinnedToCore(
        HandshakeServiceLoop,   /* Task function. */
        "HandshakeServiceTask", /* name of task. */
        4096,                   /* Stack size */
        NULL,
        5,                     /* Medium Priority out of all 3 tasks */
        &HandshakeServiceTask, /* Task handle to keep track of created task */
        1);                    /* pin task to core 0 */
}


