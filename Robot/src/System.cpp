#include <Arduino.h>
#include <FreeRTOS.h>
#include "System.h"

#include "Services/HandshakeService.h"
#include "Services/OperationService.h"

static TaskHandle_t SystemTask;

 SemaphoreHandle_t xMutex;

SystemState robotState = STATE_BOOTING;

SystemData data = {
    .IS_REMOTE = false,
    .batteryMv = 0, // Start fully charged
    .isSynced = false,
    .isEstopped = false,
    .uptimeSeconds = 0,
    .radioReady = false
};

void System_Init() {
    xMutex = xSemaphoreCreateMutex();
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
        data.uptimeSeconds++;

        // Simulate battery drain
        //state.batteryMv = 9000 - (state.uptimeSeconds * 5); // Drain 5mV per 1 seconds

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
}

void transitionTo(SystemState newState) {
    robotState = newState;

    // 2. Start the NEW state
    switch (robotState) {
        case STATE_ESTOP_ACTIVE:
            break;
        case STATE_HANDSHAKING:
            // Instead of an Init function that stays alive,
            // just start the task here.
            HandshakeService_Init();
            xTaskCreatePinnedToCore(HandshakeServiceLoop, "Handshake", 4096, NULL, 5, &HandshakeServiceTask, 1);
            break;
        case STATE_OPERATIONAL:
            OperationService_Init();
            xTaskCreatePinnedToCore(OperationServiceLoop, "Operation", 4096, NULL, 5, &OperationServiceTask, 1);
            break;
    }
}