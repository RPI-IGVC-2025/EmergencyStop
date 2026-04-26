#include "ChannelService.h"

#include <Arduino.h>

#include "Handlers/OLEDHandler.h"
#include "Handlers/RadioHandler.h"
#include "Algos/RotaryEncoder.h"
#include "System.h"

ChannelServiceStatus channelServiceStatus = {
    .confirmSelection = false
};

// Channel Selection Tas

void SelectChannelService_Init() {
    rotaryEncoder_Init();
    pinMode(SELECTION_PIN, INPUT_PULLUP);
}

void SelectChannelServiceLoop(void* pvParameters) {
    int lastSeenSel = getRotaryPos();
    bool firstPressDetected = false;
    bool firstPressReleased = false; 

    changeOLEDUpdateDelay(50);
    Serial.println("SelectChannelService started");

    for (;;) {
        int currentSel = getRotaryPos();
        bool isButtonPressed = (digitalRead(SELECTION_PIN) == LOW);

        // Dialed turned, reset button
        if (abs(currentSel - lastSeenSel) > 0) {
            firstPressDetected = false;
            firstPressReleased = false;
            lastSeenSel = currentSel;
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
                setDesiredChannel(currentSel);
                data.channelLocked = true;
                changeOLEDUpdateDelay(1000);
                
                Serial.printf("Channel %d confirmed and locked!\n", currentSel);
                vTaskDelay(pdMS_TO_TICKS(5000));
                transitionTo(STATE_HANDSHAKING);
                vTaskDelete(NULL);
            }
        }

        // Update the OLED status (firstPressReleased means the popup is visible/armed)
        channelServiceStatus.confirmSelection = firstPressReleased;
        
        lastSeenSel = currentSel;
        vTaskDelay(pdMS_TO_TICKS(25)); 
    }
}

bool getSelectionStatus() {
    return channelServiceStatus.confirmSelection;
}
