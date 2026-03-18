#include <Arduino.h>
#include "Handlers/RadioHandler.h"

#include "HeartbeatService.h"

HeartbeatState heartbeatState = {
    .heartbeatEnabled = false,
    .lastHeartbeatPacket = 0
};

void HeartbeatService_Init() {
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