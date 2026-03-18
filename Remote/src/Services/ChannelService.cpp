#include "ChannelService.h"

#include <Arduino.h>

#include "Handlers/OLEDHandler.h"
#include "Handlers/RadioHandler.h"
#include "System.h"

ChannelServiceStatus channelServiceStatus = {
    .confirmSelection = false
};

// Channel Selection Task

void SelectChannelServiceLoop(void* pvParameters);

void SelectChannelService_Init() {
    pinMode(selectionButton, INPUT_PULLUP);

    xTaskCreatePinnedToCore(
        SelectChannelServiceLoop, /* Task function. */
        "SelectChannelService",   /* name of task. */
        4096,                     /* Stack size */
        NULL,
        7,                         /* Highest Priority out of all 3 tasks */
        &SelectChannelServiceTask, /* Task handle to keep track of created task */
        0);                        /* pin task to core 0 */
}

void SelectChannelServiceLoop(void* pvParameters) {
    int lastSeenADC = state.potChannel;
    bool firstPressDetected = false;
    bool firstPressReleased = false; 

    changeOLEDUpdateDelay(25);
    Serial.println("SelectChannelService started");

    for (;;) {
        int currentADC = state.potChannel;
        bool isButtonPressed = (digitalRead(selectionButton) == LOW);

        // Dialed turned, reset button
        if (abs(currentADC - lastSeenADC) > 0) {
            firstPressDetected = false;
            firstPressReleased = false;
            lastSeenADC = currentADC;
        }

        // gates
        if (!firstPressDetected) {
            // first button press
            if (isButtonPressed) {
                firstPressDetected = true;
                Serial.println("First press detected! Wait for release...");
            }
        } 
        else if (!firstPressReleased) {
            // STEP 2: Wait for them to let go (Button goes HIGH)
            if (!isButtonPressed) {
                firstPressReleased = true;
                Serial.println("Button released. Confirmation popup ARMED.");
            }
        } 
        else {
            // STEP 3: Wait for the SECOND press to lock it in
            if (isButtonPressed) {
                setDesiredChannel(currentADC);
                state.channelLocked = true;
                changeOLEDUpdateDelay(1000);
                
                Serial.printf("Channel %d confirmed and locked!\n", currentADC);
                vTaskDelete(NULL); 
            }
        }

        // Update the OLED status (firstPressReleased means the popup is visible/armed)
        channelServiceStatus.confirmSelection = firstPressReleased;
        
        lastSeenADC = currentADC;
        vTaskDelay(pdMS_TO_TICKS(25)); 
    }
}

bool getSelectionStatus() {
    return channelServiceStatus.confirmSelection;
}
