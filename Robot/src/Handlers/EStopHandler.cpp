#include "EStopHandler.h"

#include <Arduino.h>

#include "System.h"

void triggerEStop() {
    transitionTo(STATE_ESTOP_ACTIVE);
}