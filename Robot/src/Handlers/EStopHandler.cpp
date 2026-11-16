#include "EStopHandler.h"

#include <Arduino.h>

#include "Handlers/LEDHandler.h"
#include "System.h"

void triggerEStop() {
    // for(int i = 0; i < 20; i++) {
        //Serial.println("E-STOP TRIGGERED!");
    //     delay(10);
    // }
    setEstopLED(true);
    digitalWrite(ESTOP_PIN, LOW); // Set E-Stop pin LOW to trigger E-Stop
    transitionTo(STATE_ESTOP_ACTIVE);
}