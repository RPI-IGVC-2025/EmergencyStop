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
    bool confirmedHandshake = false;
    bool secondHandshake = false;
    Packet pkt, handshakePkt;
    TickType_t lastHandshakeSent = xTaskGetTickCount();
    int receivedChannel = 0;
    for (;;) {
        // Serial.println("HandshakeService: Loop complete");
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            // Check for received packet from Robot
            if (checkHandshakePacket(&handshakePkt)) {
                if (handshakePkt.command == MessageType::HANDSHAKE) {
                    confirmedHandshake = true;
                    receivedChannel = handshakePkt.channel;
                    Serial.println("Received handshake packet from remote!");
                    vTaskDelay(pdMS_TO_TICKS(50));  // Short delay before responding
                }
                if(handshakePkt.command == MessageType::CONFIRM && confirmedHandshake){
                    secondHandshake = true;
                    Serial.println("Received confirm packet from remote!");
                }
            }
            
            if(confirmedHandshake && !secondHandshake && (xTaskGetTickCount() - lastHandshakeSent) > pdMS_TO_TICKS(2000)) {
                buildPacket(&pkt, MessageType::HANDSHAKE, StatusCode::OK, receivedChannel);  // Build handshake packet
                sendPacket(&pkt);  // Send handshake packet 
                Serial.println("Sent handshake packet to remote!");
                lastHandshakeSent = xTaskGetTickCount();
            }


            if (secondHandshake) {
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
    return true;
}
