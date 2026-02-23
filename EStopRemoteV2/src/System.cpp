#include <Arduino.h>
#include <FreeRTOS.h>
#include "System.h"

static TaskHandle_t SystemTask;

SemaphoreHandle_t xMutex = xSemaphoreCreateMutex();

SystemState state = {
    .batteryMv = 0, // Start fully charged
    .isSynced = false,
    .isEstopped = false,
    .heartbeatActive = false,
    .uptimeSeconds = 0,
    .OLEDActive = false
};

void System_Init() {
    xTaskCreatePinnedToCore(
        SystemLoop, /* Task function. */
        "SystemTask",    /* name of task. */
        4096,            /* Stack size */
        NULL,
        5,               /* Medium Priority out of all 3 tasks */
        &SystemTask,     /* Task handle to keep track of created task */
        1);              /* pin task to core 0 */
}

void SystemLoop(void* pvParameters) {
    for (;;) {
        // Update uptime
        state.uptimeSeconds++;

        // Simulate battery drain
        state.batteryMv = 9000 - (state.uptimeSeconds * 5); // Drain 5mV per 1 seconds

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
}