#include "HeartbeatService.h"

#include <Arduino.h>

#include "Handlers/RadioHandler.h"
#include "System.h"

HeartbeatState heartbeatState = {
    .heartbeatEnabled = false,
    .lastHeartbeatPacket = 0};

void HeartbeatService_Init() {
    heartbeatState.lastHeartbeatPacket = xTaskGetTickCount();
    heartbeatState.heartbeatEnabled = true;
    data.heartbeatActive = heartbeatState.heartbeatEnabled;
}

void HeartbeatServiceLoop(void* pvParameters) {
    Packet heartbeatPkt;
    for (;;) {
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            if (xTaskGetTickCount() - heartbeatState.lastHeartbeatPacket >= pdMS_TO_TICKS(1000)) {
                buildPacket(&heartbeatPkt, MessageType::HEARTBEAT, StatusCode::OK);
                sendPacket(&heartbeatPkt);
                heartbeatState.lastHeartbeatPacket = xTaskGetTickCount();
                Serial.println("Sent Heartbeat Packet");
            }
            xSemaphoreGive(xMutex); 
            vTaskDelay(pdMS_TO_TICKS(25));
        }
    }
}