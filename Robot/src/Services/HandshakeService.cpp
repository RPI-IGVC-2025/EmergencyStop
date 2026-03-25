#include "HandshakeService.h"

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "Algos/Auth.h"
#include "Handlers/RadioHandler.h"
#include "System.h"

void HandshakeService_Init() {

}

// Add logic for a remote response

void HandshakeServiceLoop(void* pvParameters) {
    Packet pkt, handshakePkt;
    TickType_t lastHandshakeSent = xTaskGetTickCount();
    int receivedChannel = 0;
    for (;;) {
        // Serial.println("HandshakeService: Loop complete");
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            // Check for received packet from Robot
            if (checkHandshakePacket(&pkt)) {
                receivedChannel = pkt.channel;
                Serial.println("Received handshake packet from robot!");
                vTaskDelay(pdMS_TO_TICKS(50));  // Short delay before responding
                for (int i = 0; i < 10; i++) {
                    Serial.println("Sent");
                    buildPacket(&handshakePkt, MessageType::HANDSHAKE, StatusCode::OK, receivedChannel);
                    sendPacket(&handshakePkt);
                    vTaskDelay(pdMS_TO_TICKS(500 + random(0, 500)));  // Delay between handshake packets
                }
                data.isSynced = true;
                while (!HC12switchChannel(receivedChannel)) {
                    vTaskDelay(pdMS_TO_TICKS(500));
                }  // Switch to the channel specified by the robot

                if (data.isSynced) {
                    // If we're synced, we can stop sending handshakes
                    Serial.println("HandshakeService: Synced, ending task");
                    xSemaphoreGive(xMutex);  // ALWAYS give it back!
                    transitionTo(STATE_OPERATIONAL);
                    vTaskDelete(NULL);  // Delete this task
                }
            }

            Serial.println("0");
            xSemaphoreGive(xMutex);         // ALWAYS give it back!
            vTaskDelay(pdMS_TO_TICKS(50));  // Delay for 100ms to prevent task hogging
        }
    }
}

bool checkHandshakePacket(Packet* pkt) {
    if (!getNextFrame(pkt)) return false;
    if (!verifyPacket(pkt)) return false;
    if (pkt->command != MessageType::HANDSHAKE) return false;
    return true;
}
