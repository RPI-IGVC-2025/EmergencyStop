#include "HandshakeService.h"

#include <Arduino.h>

#include "Algos/Auth.h"
#include "Algos/PacketBuilder.h"
#include "Handlers/RadioHandler.h"
#include "System.h"

void HandshakeService_Init() {}

void HandshakeServiceLoop(void* pvParameters) {
    Serial.println("Handshake task begin!");
    Packet pkt, handshakePkt;
    TickType_t lastHandshakeSent = xTaskGetTickCount();
    uint16_t desiredChannel = getDesiredChannel();
    for (;;) {
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            // Check for received packet from Robot
            while(checkIncomingPacket(&pkt)) {
                Serial.println("Received valid handshake packet");
                if(pkt.channel == getDesiredChannel()) {
                    for(; !HC12switchChannel(pkt.channel);) {}
                    xSemaphoreGive(xMutex); 
                    data.isSynced = true;
                    vTaskDelay(pdMS_TO_TICKS(7500));
                }
            }

            if (data.isSynced) {
                // If we're synced, we can stop sending handshakes
                setCurrentChannel(getDesiredChannel());
                transitionTo(STATE_OPERATIONAL);
                xSemaphoreGive(xMutex); 
                vTaskDelete(NULL);
            }
            
            // Send a packet every 1 second
            if (xTaskGetTickCount() - lastHandshakeSent >= pdMS_TO_TICKS(1000)) {
                Serial.println("Sent handshake packet");
                // Send a handshake packet every 1 second
                buildPacket(&handshakePkt, MessageType::HANDSHAKE, StatusCode::OK, desiredChannel);
                sendPacket(&handshakePkt);
                lastHandshakeSent = xTaskGetTickCount();
            }

            // ALWAYS give it back!
            xSemaphoreGive(xMutex);
            //Serial.println("Loop complete");
        }

        vTaskDelay(pdMS_TO_TICKS(50));  // Delay for 50ms to prevent task hogging
    }
}

bool checkIncomingPacket(Packet* pkt) {
    if(!getNextFrame(pkt)) return false;
    if(!verifyPacket(pkt)) return false;   
    if(pkt->command != MessageType::HANDSHAKE) return false;
    return true;
}
