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
        // Serial.println("HandshakeService: Loop complete");
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            // Check for received packet from Robot
            if (getNextFrame(&pkt)) {
                Serial.println("Received packet");
                if (verifyPacket(&pkt)) {
                    Serial.println("Received valid packet from robot!");
                    if (pkt.command == MessageType::HANDSHAKE) {
                        receivedChannel = pkt.channel;
                        Serial.println("Received handshake packet from robot!");
                        vTaskDelay(pdMS_TO_TICKS(50));  // Short delay before responding
                        for (int i = 0; i < 10; i++) {
                            Serial.println("Sent");
                            buildPacket(&handshakePkt, MessageType::HANDSHAKE, StatusCode::OK, receivedChannel);
                            sendPacket(&handshakePkt);
                            vTaskDelay(pdMS_TO_TICKS(1000));  // Delay between handshake packets
                        }
                        state.isSynced = true;
                        while(!HC12switchChannel(receivedChannel)) {
                            vTaskDelay(pdMS_TO_TICKS(500));
                        } // Switch to the channel specified by the robot
                    } else {
                        Serial.println("Received invalid packet!");
                    }
                }

                if (state.isSynced) {
                    // If we're synced, we can stop sending handshakes
                    Serial.println("HandshakeService: Synced, ending task");
                    xSemaphoreGive(xMutex);  // ALWAYS give it back!
                    vTaskDelete(NULL);       // Delete this task
                }
            }

            Serial.println("0");
            xSemaphoreGive(xMutex);         // ALWAYS give it back!
            vTaskDelay(pdMS_TO_TICKS(50));  // Delay for 100ms to prevent task hogging
        }
    }
}
