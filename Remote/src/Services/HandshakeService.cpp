// Remote Side
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
                    vTaskDelay(pdMS_TO_TICKS(500));

                    // Send confirm packet.
                    Serial.println("Sending confirm packet");
                    buildPacket(&handshakePkt, MessageType::CONFIRM, StatusCode::OK, pkt.channel);
                    sendPacket(&handshakePkt);

                    vTaskDelay(pdMS_TO_TICKS(500));
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
            if (xTaskGetTickCount() - lastHandshakeSent >= pdMS_TO_TICKS(2000)) {
                vTaskDelay(pdMS_TO_TICKS(100));
                Serial.println("Sent handshake packet");
                // Send a handshake packet every 2 seconds
                buildPacket(&handshakePkt, MessageType::HANDSHAKE, StatusCode::OK, desiredChannel);
                sendPacket(&handshakePkt);
                lastHandshakeSent = xTaskGetTickCount();
            }

            // ALWAYS give it back!
            xSemaphoreGive(xMutex);
            //Serial.println("Loop complete");
        }

        vTaskDelay(pdMS_TO_TICKS(10));  // Delay for 50ms to prevent task hogging
    }
}

// HardwareSerial HC121 = getHC12();

// void HandshakeServiceLoop(void* pvParameters) {
//     Serial.println("Handshake task begin!");
//     Packet pkt, handshakePkt;
//     TickType_t lastHandshakeSent = xTaskGetTickCount();
//     uint16_t desiredChannel = getDesiredChannel();
//     bool waitingForAck = false;
//     TickType_t ackDeadline = 0;

//     for (;;) {
//         if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {

//             // --- LISTENING PHASE ---
//             // If we just sent a packet, enter a dedicated listen window
//             // and DO NOT retransmit during this window
//             if (waitingForAck) {
//                 if (checkIncomingPacket(&pkt)) {
//                     // Got the ACK - send CONFIRM and finish
//                     Serial.println("Received ACK - sending CONFIRM");
//                     vTaskDelay(pdMS_TO_TICKS(200)); // let robot settle
//                     buildPacket(&handshakePkt, MessageType::CONFIRM, 
//                                StatusCode::OK, pkt.channel);
//                     sendPacket(&handshakePkt);
                    
//                     // Wait for CONFIRM to fully transmit before switching
//                     vTaskDelay(pdMS_TO_TICKS(200));
                    
//                     for (; !HC12switchChannel(pkt.channel);) {}
//                     data.isSynced = true;
//                     xSemaphoreGive(xMutex);
//                     vTaskDelay(pdMS_TO_TICKS(7500));
//                     continue;
//                 }

//                 // Check if our listen window expired without an ACK
//                 if (xTaskGetTickCount() > ackDeadline) {
//                     Serial.println("ACK timeout - will retransmit");
//                     // Flush stale bytes before retrying
//                     while (HC121.available()) HC121.read();
//                     waitingForAck = false;
//                     // Force immediate retransmit on next iteration
//                     lastHandshakeSent = xTaskGetTickCount() - pdMS_TO_TICKS(2000);
//                 }

//                 xSemaphoreGive(xMutex);
//                 vTaskDelay(pdMS_TO_TICKS(10));
//                 continue; // Skip transmit section entirely while waiting
//             }

//             // --- TRANSMIT PHASE ---
//             // Only reached if we are NOT currently waiting for an ACK
//             if (data.isSynced) {
//                 setCurrentChannel(getDesiredChannel());
//                 transitionTo(STATE_OPERATIONAL);
//                 xSemaphoreGive(xMutex);
//                 vTaskDelete(NULL);
//             }

//             if (xTaskGetTickCount() - lastHandshakeSent >= pdMS_TO_TICKS(2000)) {
//                 // Flush before transmitting
//                 while (HC121.available()) HC121.read();
                
//                 Serial.println("Sending HANDSHAKE");
//                 buildPacket(&handshakePkt, MessageType::HANDSHAKE, 
//                            StatusCode::OK, desiredChannel);
//                 sendPacket(&handshakePkt);
//                 lastHandshakeSent = xTaskGetTickCount();
                
//                 // Enter listen window - 1500ms to receive ACK before giving up
//                 waitingForAck = true;
//                 ackDeadline = xTaskGetTickCount() + pdMS_TO_TICKS(1500);
//             }

//             xSemaphoreGive(xMutex);
//         }
//         vTaskDelay(pdMS_TO_TICKS(10));
//     }
// }

bool checkIncomingPacket(Packet* pkt) {
    if(!getNextFrame(pkt)) return false;
    if(!verifyPacket(pkt)) return false;   
    if(pkt->command != MessageType::HANDSHAKE) return false;
    return true;
}
