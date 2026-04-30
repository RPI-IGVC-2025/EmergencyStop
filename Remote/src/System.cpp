#include "System.h"

#include <Arduino.h>
#include <FreeRTOS.h>

#include "Algos/Battery.h"
#include "Handlers/OLEDHandler.h"
#include "Handlers/RadioHandler.h"
#include "Services/ChannelService.h"
#include "Services/EStopService.h"
#include "Services/HandshakeService.h"
#include "Services/HeartbeatService.h"
#include "Services/RotaryService.h"

static TaskHandle_t SystemTask;

SemaphoreHandle_t xMutex = xSemaphoreCreateMutex();

SystemData data = {
    .EStopActive = false,
    .EStopPrimed = false,
    .batteryMv = 9000,  // Start fully charged
    .channelLocked = false,
    .isSynced = false,
    .isEstopped = false,
    .heartbeatActive = false,
    .heartbeatCount = 0,
    .uptimeSeconds = 0,
    .OLEDActive = false,
    .radioReady = false};

SystemState remoteState = STATE_BOOTING;

void System_Init() {
    xTaskCreatePinnedToCore(
        SystemLoop,   /* Task function. */
        "SystemTask", /* name of task. */
        4096,         /* Stack size */
        NULL,
        2,           /* Medium Priority out of all 3 tasks */
        &SystemTask, /* Task handle to keep track of created task */
        0);          /* pin task to core 0 */

    xTaskCreatePinnedToCore(
        RotaryServiceLoop, 
        "RotarySelect", 
        2048, 
        NULL, 
        3, 
        &RotaryServiceTask, 
        0);
    vTaskSuspend(RotaryServiceTask);

    Battery_Init();
}

void SystemLoop(void* pvParameters) {
    TickType_t xFrequency;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    int batteryMVSum = 0;
    for (;;) {
        xFrequency = pdMS_TO_TICKS(1000);  // Exact 1s period
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        // Update uptime
        // if (xTaskGetTickCount() - prevTime >= pdMS_TO_TICKS(1000)) {  // Every 1 second
            data.uptimeSeconds++;
            for(int i = 0; i < 3; i++) {  // Average over 3 readings for stability
                batteryMVSum += returnMillivolts();
                vTaskDelay(pdMS_TO_TICKS(10));  // Small delay between readings
            }
            data.batteryMv = batteryMVSum / 3;  // Calculate average
            batteryMVSum = 0;  // Reset sum for next round
            Serial.println("mV: " + String(data.batteryMv));
        // }

    }
}

// System.cpp
void transitionTo(SystemState newState) {
    remoteState = newState;

    // 2. Start the NEW state
    switch (remoteState) {
        case STATE_ESTOP_ACTIVE:
            break;
        case STATE_SELECTING_CHANNEL:
            SelectChannelService_Init();
            vTaskResume(RotaryServiceTask);
            xTaskCreatePinnedToCore(SelectChannelServiceLoop, "ChannelSelect", 4096, NULL, 3, &SelectChannelServiceTask, 1);
            break;
        case STATE_HANDSHAKING:
            vTaskSuspend(RotaryServiceTask);
            // Instead of an Init function that stays alive,
            // just start the task here.
            HandshakeService_Init();
            xTaskCreatePinnedToCore(HandshakeServiceLoop, "Handshake", 4096, NULL, 3, &HandshakeServiceTask, 1);
            break;

        case STATE_OPERATIONAL:
            vTaskSuspend(RotaryServiceTask);
            EStopService_Init();
            HeartbeatService_Init();
            xTaskCreatePinnedToCore(HeartbeatServiceLoop, "Heartbeat", 2048, NULL, 3, &HeartbeatServiceTask, 1);
            xTaskCreatePinnedToCore(EStopServiceLoop, "EStop", 2048, NULL, 5, &EStopServiceTask, 1);
            break;
    }
}