#include "EStopHandler.h"

#include <Arduino.h>

#include "System.h"

void triggerEStop() {
    for(int i = 0; i < 20; i++) {
        Serial.println("E-STOP TRIGGERED!");
        delay(10);
    }
    transitionTo(STATE_ESTOP_ACTIVE);
}