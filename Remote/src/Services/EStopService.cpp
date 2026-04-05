#include "EStopService.h"

#include <Arduino.h>

#include "Algos/PacketBuilder.h"
#include "Handlers/RadioHandler.h"
#include "System.h"

uint8_t EStopLowCounts = 0;
TickType_t previousTime;

void EStopService_Init() {
    pinMode(ESTOP_PIN, INPUT_PULLUP);
    previousTime = 0;
    data.EStopPrimed = true;
}

void EStopServiceLoop(void* pvParameters) {
    Packet pkt;
    for (;;) {
        if (!data.EStopActive) {
            previousTime = xTaskGetTickCount();
            if (digitalRead(ESTOP_PIN) == LOW) {
                EStopLowCounts++;
            } else {
                EStopLowCounts = 0;
            }

            if (EStopLowCounts > 3) {
                triggerEStop();
            }
        } else {
            if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
                if (xTaskGetTickCount() - previousTime >= pdMS_TO_TICKS(250)) {
                    buildPacket(&pkt, MessageType::ESTOP, StatusCode::BUTTON_PRESS);
                    pkt.hash[5] = 0xAB; // Placeholder hash value for testing
                    sendPacket(&pkt);
                    Serial.println("Sent ESTOP Packet");
                }
                xSemaphoreGive(xMutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void triggerEStop() {
    data.EStopActive = true;
    transitionTo(STATE_ESTOP_ACTIVE);
}