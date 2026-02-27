#include "HandshakeService.h"

#include <Arduino.h>

#include "Algos/PacketBuilder.h"
#include "Algos/Auth.h"
#include "Handlers/RadioHandler.h"
#include "System.h"

void HandshakeService_Init() {
    xTaskCreatePinnedToCore(
        HandshakeServiceLoop,   /* Task function. */
        "HandshakeServiceTask", /* name of task. */
        4096,                   /* Stack size */
        NULL,
        10,                    /* Medium Priority out of all 3 tasks */
        &HandshakeServiceTask, /* Task handle to keep track of created task */
        1);                    /* pin task to core 1 */
}

void HandshakeServiceLoop(void* pvParameters) {
    Packet pkt;
    TickType_t lastHandshakeSent = xTaskGetTickCount();
    for (;;) {
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            // Check for received packet from Robot
            if (getNextFrame(&pkt)) {
                if(verifyPacket(&pkt)) {
                    Serial.println("Received valid packet from robot!");
                    if (pkt.command == MessageType::HANDSHAKE) {
                        Serial.println("Received handshake response from remote");
                        state.isSynced = true;
                    }
                } else {
                    Serial.println("Received invalid packet!");
                }
            }

            if (state.isSynced) {
                // If we're synced, we can stop sending handshakes
                xSemaphoreGive(xMutex);  // ALWAYS give it back!
                vTaskDelete(NULL);  // Delete this task
            }

            if (xTaskGetTickCount() - lastHandshakeSent > pdMS_TO_TICKS(1000)) {
                Serial.println("Sent handshake packet");
                // Send a handshake packet every 1 second
                Packet handshakePkt;
                buildPacket(&handshakePkt, MessageType::HANDSHAKE, StatusCode::OK, state.potChannel);
                
                // Assuming there's a function to send packets, e.g., sendPacket()
                sendPacket(&handshakePkt);
                lastHandshakeSent = xTaskGetTickCount();
            }

            xSemaphoreGive(xMutex);  // ALWAYS give it back!
            Serial.println("HandshakeService: Loop iteration complete");
        }

        vTaskDelay(pdMS_TO_TICKS(25));  // Delay for 25ms to prevent task hogging
    }
}
