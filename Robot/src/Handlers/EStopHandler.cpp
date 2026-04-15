#include "EStopHandler.h"

#include <Arduino.h>

#include "System.h"

void triggerEStop() {
    for(int i = 0; i < 20; i++) {
        Serial.println("E-STOP TRIGGERED!");
        delay(10);
    }
    digitalWrite(ESTOP_PIN, HIGH); // Set E-Stop pin HIGH to trigger E-Stop
    transitionTo(STATE_ESTOP_ACTIVE);
}