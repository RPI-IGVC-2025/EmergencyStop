#include "HandshakeService.h"

#include <Arduino.h>

#include "Algos/Auth.h"
#include "Algos/PacketBuilder.h"
#include "Handlers/RadioHandler.h"
#include "System.h"

void HandshakeService_Init() {
    xTaskCreatePinnedToCore(
        HandshakeServiceLoop,   /* Task function. */
        "HandshakeServiceTask", /* name of task. */
        4096,                   /* Stack size */
        NULL, 10,               /* Medium Priority out of all 3 tasks */
        &HandshakeServiceTask,  /* Task handle to keep track of created task */
        0);                     /* pin task to core 1 */
}

void HandshakeServiceLoop(void* pvParameters) {
    Packet pkt, handshakePkt;
    TickType_t lastHandshakeSent = xTaskGetTickCount();
    uint16_t desiredChannel = getDesiredChannel();
    for (;;) {
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            // Check for received packet from Robot
            if(checkIncomingPacket(&pkt)) {
                Serial.println("Received valid handshake packet");
                if(pkt.channel == getDesiredChannel()) {
                    state.isSynced = true;
                }
            }

            if (state.isSynced) {
                // If we're synced, we can stop sending handshakes
                xSemaphoreGive(xMutex);  // ALWAYS give it back!
                vTaskDelete(NULL);       // Delete this task
            }
            
            // Send a packet every 1 second
            if (xTaskGetTickCount() - lastHandshakeSent > pdMS_TO_TICKS(1000)) {
                Serial.println("Sent handshake packet");
                // Send a handshake packet every 1 second
                buildPacket(&handshakePkt, MessageType::HANDSHAKE,
                            StatusCode::OK, desiredChannel);
                sendPacket(&handshakePkt);
                lastHandshakeSent = xTaskGetTickCount();
            }

            xSemaphoreGive(xMutex);  // ALWAYS give it back!
            Serial.println("Loop complete");
        }

        vTaskDelay(
            pdMS_TO_TICKS(50));  // Delay for 50ms to prevent task hogging
    }
}

bool checkIncomingPacket(Packet* pkt) {
    if(!getNextFrame(pkt)) return false;
    if(!verifyPacket(pkt)) return false;   
    if(pkt->command != MessageType::HANDSHAKE) return false;
    return true;
}
