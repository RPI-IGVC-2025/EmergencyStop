#include <Arduino.h>
#include "Handlers/RadioHandler.h"

#include "HeartbeatService.h"

HeartbeatState heartbeatState = {
    .heartbeatEnabled = false,
    .lastHeartbeatPacket = 0
};

void HeartbeatService_Init() {
    xTaskCreatePinnedToCore(
        HeartbeatServiceLoop,   /* Task function. */
        "HandshakeServiceTask", /* name of task. */
        4096,                   /* Stack size */
        NULL, 10,               /* Medium Priority out of all 3 tasks */
        &HeartbeatServiceTask,  /* Task handle to keep track of created task */
        1);                     /* pin task to core 0*/

    heartbeatState.lastHeartbeatPacket = xTaskGetTickCount();
    heartbeatState.heartbeatEnabled = true;
}

void HeartbeatServiceLoop(void* pvParameters) {
    Packet heartbeatPkt;
    for(;;) {
        if(xTaskGetTickCount() - heartbeatState.lastHeartbeatPacket >= pdMS_TO_TICKS(1000)) {
            buildPacket(&heartbeatPkt, MessageType::HEARTBEAT, StatusCode::OK);
            sendPacket(&heartbeatPkt);
            heartbeatState.lastHeartbeatPacket = xTaskGetTickCount();
            Serial.println("Sent Heartbeat Packet");
        }
        vTaskDelay(pdMS_TO_TICKS(25));
    }
}