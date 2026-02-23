#include "HandshakeService.h"

#include <Arduino.h>

#include "Algos/PacketBuilder.h"
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
        1);                    /* pin task to core 0 */
}

void HandshakeServiceLoop(void* pvParameters) {
    Packet pkt;
    TickType_t lastHandshakeSent = xTaskGetTickCount();
    for (;;) {
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            if (getNextFrame(&pkt)) {
                processValidatedPacket(&pkt);
            }

            if (state.isSynced) {
                // If we're synced, we can stop sending handshakes
                vTaskDelete(NULL);  // Delete this task
            }

            if (xTaskGetTickCount() - lastHandshakeSent > pdMS_TO_TICKS(1000)) {
                Serial.println("Sent handshake packet");
                // Send a handshake packet every 1 second
                Packet handshakePkt;
                buildPacket(&handshakePkt, MessageType::HANDSHAKE, StatusCode::OK);
                // Assuming there's a function to send packets, e.g., sendPacket()
                // sendPacket(&handshakePkt);
                lastHandshakeSent = xTaskGetTickCount();
            }

            xSemaphoreGive(xMutex);  // ALWAYS give it back!
            Serial.println("HandshakeService: Loop iteration complete");
        }

        vTaskDelay(pdMS_TO_TICKS(50));  // Delay for 100ms to prevent task hogging
    }
}
