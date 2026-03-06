#include <Arduino.h>

#include "Handlers/RadioHandler.h"
#include "ChannelService.h"
#include "System.h"
#include "Handlers/OLEDHandler.h"

ChannelServiceStatus channelServiceStatus = {
    .timeRemaining = 0
};

// Channel Selection Task

void SelectChannelServiceLoop(void* pvParameters);

void SelectChannelService_Init() {

    pinMode(selectionButton, INPUT_PULLUP);

    xTaskCreatePinnedToCore(
        SelectChannelServiceLoop,   /* Task function. */
        "SelectChannelService", /* name of task. */
        4096,       /* Stack size */
        NULL,
        7,                /* Highest Priority out of all 3 tasks */
        &SelectChannelServiceTask, /* Task handle to keep track of created task */
        0);               /* pin task to core 0 */
}

void SelectChannelServiceLoop(void* pvParameters) {
    int lastSeenADC = state.potChannel;
    bool firstSel = false;
    bool firstPressOver = false;
    TickType_t lastChangeTime = xTaskGetTickCount();
    
    changeOLEDUpdateDelay(50); 

    Serial.println("SelectChannelService started");

    for(;;) {
        int currentADC = state.potChannel;
        channelServiceStatus.timeRemaining = 5 - (pdTICKS_TO_MS(xTaskGetTickCount() - lastChangeTime)/1000); // Reset displayed time

        //Detecting Interaction
        if (!firstSel) {
            Serial.print(firstSel);
            Serial.print("  ");
            Serial.println(abs(currentADC - lastSeenADC));
            
            if (digitalRead(selectionButton) == LOW) { // if button is pressed
                firstSel = true;
                lastChangeTime = xTaskGetTickCount();
                Serial.println("Channel selection active");
            }
            channelServiceStatus.timeRemaining = 5; // Reset displayed time 
        } 
        // 2. Monitor for changes once active
        else {
            if (currentADC != lastSeenADC) {
                firstSel = false;
                lastChangeTime = xTaskGetTickCount(); // Reset the 5s timer
                lastSeenADC = currentADC;
            }

            // 3. Check for Timeout (5 seconds)
            // if (xTaskGetTickCount() - lastChangeTime > pdMS_TO_TICKS(5800)) {
            //     setDesiredChannel(currentADC);
            //     state.channelLocked = true;
            //     changeOLEDUpdateDelay(1000);
                
            //     vTaskDelay(pdMS_TO_TICKS(250)); // Small delay to ensure the channel switch command is sent before printing
            //     Serial.printf("Channel %d locked in!\n", currentADC);
            //     vTaskDelete(NULL); 
            // }

            // Check for second button press to confirm selection
            if ((digitalRead(selectionButton) == LOW) && firstSel) {
                setDesiredChannel(currentADC);
                state.channelLocked = true;
                changeOLEDUpdateDelay(1000);
                
                vTaskDelay(pdMS_TO_TICKS(250)); // Small delay to ensure the channel switch command is sent before printing
                Serial.printf("Channel %d locked in!\n", currentADC);
                vTaskDelete(NULL); 
            }
        }

        lastSeenADC = currentADC;
        vTaskDelay(pdMS_TO_TICKS(100)); // Check 10 times a second
    }
}

int getChannelSelectionTimeRemaining() {
    return channelServiceStatus.timeRemaining;
}

