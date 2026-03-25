#include "OperationService.h"

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "Algos/Auth.h"
#include "Handlers/RadioHandler.h"
#include "Handlers/HeartbeatHandler.h"
#include "System.h"

void OperationService_Init() {
    updateLastHeartbeatTime(xTaskGetTickCount());
}

void OperationServiceLoop(void* pvParameters) {
    Packet pkt;
    MessageType type;
    for (;;) {
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            if(checkIncomingPacket(&pkt)) {
                uint8_t buffer[sizeof(Packet)];
                memcpy(buffer,&pkt,sizeof(Packet));

                for(int i = 0; i < sizeof(Packet); i++) {
                    Serial.print(buffer[i]);
                    Serial.print(", ");
                }
                Serial.println();

                if(pkt.command == MessageType::ESTOP) {
                    Serial.println("RECIEVED ESTOP PACKET!");
                }
                if(pkt.command == MessageType::HEARTBEAT) {
                    Serial.println("Received Heartbeat Packet!");
                    updateLastHeartbeatTime(xTaskGetTickCount());
                }
                if(pkt.command == MessageType::HANDSHAKE) {
                    // Triger Handshake for channel change
                }
            }
        }

        xSemaphoreGive(xMutex);
        vTaskDelay(pdMS_TO_TICKS(25));
    }
}

bool checkIncomingPacket(Packet* pkt) {
    if (!getNextFrame(pkt)) return false;
    if (!verifyPacket(pkt)) return false;
    return true;
}
