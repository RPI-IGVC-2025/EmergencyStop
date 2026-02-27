#include "System.h"

#include <Arduino.h>
#include <FreeRTOS.h>

#include "Algos/ADC.h"

static TaskHandle_t SystemTask;

SemaphoreHandle_t xMutex = xSemaphoreCreateMutex();

SystemState state = {
    .batteryMv = 9000,  // Start fully charged
    .channelLocked = false,
    .isSynced = false,
    .isEstopped = false,
    .heartbeatActive = false,
    .uptimeSeconds = 0,
    .OLEDActive = false,
    .potChannel = 1
};

void System_Init() {
    xTaskCreatePinnedToCore(
        SystemLoop,   /* Task function. */
        "SystemTask", /* name of task. */
        4096,         /* Stack size */
        NULL,
        5,           /* Medium Priority out of all 3 tasks */
        &SystemTask, /* Task handle to keep track of created task */
        0);          /* pin task to core 0 */
}

void SystemLoop(void* pvParameters) {
    TickType_t prevTime = xTaskGetTickCount();
    int adcValue = 1;
    for (;;) {
        // Update uptime
        if (xTaskGetTickCount() - prevTime >= pdMS_TO_TICKS(1000)) {  // Every 1 second
            state.uptimeSeconds++;
            prevTime = xTaskGetTickCount();  // Reset prevTime to current time
        }

        // Simulate battery drain
        state.batteryMv -= 1;  // Drain 1mv per 1/20th of a second, so 50mv per second

        if (!state.channelLocked) {
            ADC_Read(&adcValue);
            if (adcValue != state.potChannel) {
                state.potChannel = adcValue;
            }
            //Serial.println(state.potChannel);
        }

        // Update ADC channel
        vTaskDelay(pdMS_TO_TICKS(50));  // Delay for 50 ms: 20hz
    }
}