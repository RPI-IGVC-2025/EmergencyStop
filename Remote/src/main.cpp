#include <Arduino.h>

#include "Handlers/OLEDHandler.h"
#include "Handlers/RadioHandler.h"
#include "Services/ChannelService.h"
#include "Services/HandshakeService.h"
#include "Services/HeartbeatService.h"
#include "Services/EStopService.h"
#include "System.h"

void MainService(void* pvParameters);

static TaskHandle_t MainServiceTask;

void setup() {
    xTaskCreatePinnedToCore(
        MainService,   /* Task function. */
        "MainService", /* name of task. */
        4096,          /* Stack size */
        NULL,
        7,                /* Highest Priority out of all 3 tasks */
        &MainServiceTask, /* Task handle to keep track of created task */
        0);               /* pin task to core 0 */
}

void MainService(void* pvParameters) {
    Serial.begin(115200);
    System_Init();
    OLED_Init();
    for (; !state.OLEDActive;) {
      vTaskDelay(pdMS_TO_TICKS(50));
    }
    Radio_Init();
    // HandshakeService_Init();
    SelectChannelService_Init();
    for(; !state.channelLocked;) {
      vTaskDelay(pdMS_TO_TICKS(50));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));  // Small delay to ensure channel lock is displayed on OLED before we start handshakes
    HandshakeService_Init();
    for(; !state.isSynced; ) {
      vTaskDelay(pdMS_TO_TICKS(50));
    }
    Serial.println("MAIN SERVICES STARTED!");
    HeartbeatService_Init();
    EStopService_Init();
}

void loop() {
}