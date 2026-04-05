#include "OperationService.h"

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "Algos/Auth.h"
#include "Handlers/EStopHandler.h"
#include "Handlers/HeartbeatHandler.h"
#include "Handlers/RadioHandler.h"
#include "System.h"

void OperationService_Init() {
    updateLastHeartbeatTime(xTaskGetTickCount());
}

void OperationServiceLoop(void* pvParameters) {
    Packet pkt;
    MessageType type;
    for (;;) {
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            if (checkIncomingPacket(&pkt)) {
                uint8_t buffer[sizeof(Packet)];
                memcpy(buffer, &pkt, sizeof(Packet));

                for (int i = 0; i < sizeof(Packet); i++) {
                    Serial.print(buffer[i]);
                    Serial.print(", ");
                }
                Serial.println();

                if (pkt.command == MessageType::ESTOP) {
                    Serial.println("RECIEVED ESTOP PACKET!");
                    triggerEStop();
                }
                if (pkt.command == MessageType::HEARTBEAT) {
                    Serial.println("Received Heartbeat Packet!");
                    updateLastHeartbeatTime(xTaskGetTickCount());
                    setHeartbeatActive();
                }
                if (pkt.command == MessageType::HANDSHAKE) {
                    // Triger Handshake for channel change
                }
            }
        }

        if(getHeartbeatActive()) {
            checkHeartbeatTimeout();
        }

        xSemaphoreGive(xMutex);
        vTaskDelay(pdMS_TO_TICKS(25));
    }
}

bool checkIncomingPacket(Packet* pkt) {
    if (!getNextFrame(pkt)) return false;
    if (!verifyPacket(pkt)) {
        swissCheeseCheck(pkt, getCurrentHashValid());
        return false;
    } else {
        swissCheeseCheck(pkt, true);
    }
    return true;
}

uint8_t EStopVote = 0;

void swissCheeseCheck(Packet* pkt, bool hashValid) {
    if (!hashValid) {
        Serial.println("Boolean works!");
        if (pkt->command == MessageType::ESTOP) {
            EStopVote++;
            Serial.println(EStopVote);
            if (EStopVote >= 2) {
                Serial.println("Swiss Cheese Check Failed: Multiple ESTOP Packets with Invalid Hash");
                triggerEStop();
            }
        }
    } else {
        EStopVote = 0;
    }
}

void checkHeartbeatTimeout() {
    TickType_t currentTime = xTaskGetTickCount();
    if (currentTime - getLastHeartbeatTime() > pdMS_TO_TICKS(5000)) {
        Serial.println("Heartbeat Timeout!");
        triggerEStop();
    }
}
