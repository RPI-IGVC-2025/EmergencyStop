#include "System.h"

#include <Arduino.h>
#include <FreeRTOS.h>

#include "Algos/ADC.h"
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
    .potChannel = 1,
    .radioReady = false
};

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
}

void SystemLoop(void* pvParameters) {
    TickType_t prevTime = xTaskGetTickCount();
    int adcValue = 1;
    for (;;) {
        // Update uptime
        if (xTaskGetTickCount() - prevTime >= pdMS_TO_TICKS(1000)) {  // Every 1 second
            data.uptimeSeconds++;
            prevTime = xTaskGetTickCount();  // Reset prevTime to current time
        }

        // Simulate battery drain
        data.batteryMv -= 1;  // Drain 1mv per 1/20th of a second, so 50mv per second

        if (!data.channelLocked) {
            ADC_Read(&adcValue);
            if (adcValue != data.potChannel) {
                data.potChannel = adcValue;
            }
            // Serial.println(state.potChannel);
        }

        // Update ADC channel
        vTaskDelay(pdMS_TO_TICKS(5));  // Delay for 5 ms: 20hz
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
            xTaskCreatePinnedToCore(RotaryServiceLoop, "RotarySelect", 2048, NULL, 3, &RotaryServiceTask, 0);
            xTaskCreatePinnedToCore(SelectChannelServiceLoop, "ChannelSelect", 4096, NULL, 3, &SelectChannelServiceTask, 1);
            break;
        case STATE_HANDSHAKING:
            vTaskDelete(&RotaryServiceTask);
            // Instead of an Init function that stays alive,
            // just start the task here.
            HandshakeService_Init();
            xTaskCreatePinnedToCore(HandshakeServiceLoop, "Handshake", 4096, NULL, 3, &HandshakeServiceTask, 1);
            break;

        case STATE_OPERATIONAL:
            vTaskDelete(&RotaryServiceTask);
            EStopService_Init();
            HeartbeatService_Init();
            xTaskCreatePinnedToCore(HeartbeatServiceLoop, "Heartbeat", 2048, NULL, 3, &HeartbeatServiceTask, 1);
            xTaskCreatePinnedToCore(EStopServiceLoop, "EStop", 2048, NULL, 5, &EStopServiceTask, 1);
            break;
    }
}