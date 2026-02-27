#include "HandshakeService.h"

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "Algos/Auth.h"
#include "Handlers/RadioHandler.h"
#include "System.h"

void HandshakeService_Init() {
    xTaskCreatePinnedToCore(
        HandshakeServiceLoop,  // Task function
        "HandshakeService",    // Name of the task
        4096,                  // Stack size in words
        NULL,                  // Task input parameter
        2,                     // Priority of the task
        &HandshakeServiceTask,
        0);  // Task handle
}

void HandshakeServiceLoop(void* pvParameters) {
    Packet pkt, handshakePkt;
    TickType_t lastHandshakeSent = xTaskGetTickCount();
    int receivedChannel = 0;
    for (;;) {
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            // Check for received packet from Robot
            if (getNextFrame(&pkt)) {
                if (verifyPacket(&pkt)) {
                    Serial.println("Received valid packet from robot!");
                    if (pkt.command == MessageType::HANDSHAKE) {
                        buildPacket(&handshakePkt, MessageType::HANDSHAKE, StatusCode::OK, receivedChannel);
                        // sendPacket(&handshakePkt);
                        state.isSynced = true;
                    } else {
                        Serial.println("Received invalid packet!");
                    }
                }

                if (state.isSynced) {
                    // If we're synced, we can stop sending handshakes
                    xSemaphoreGive(xMutex);  // ALWAYS give it back!
                    vTaskDelete(NULL);       // Delete this task
                }

                xSemaphoreGive(xMutex);  // ALWAYS give it back!
                Serial.println("HandshakeService: Loop iteration complete");
            }

            vTaskDelay(pdMS_TO_TICKS(25));  // Delay for 25ms to prevent task hogging
        }
    }
}
