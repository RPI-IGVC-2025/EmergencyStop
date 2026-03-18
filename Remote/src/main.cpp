#include <Arduino.h>

#include "Handlers/OLEDHandler.h"
#include "Handlers/RadioHandler.h"
#include "Services/ChannelService.h"
#include "Services/HandshakeService.h"
#include "Services/HeartbeatService.h"
#include "Services/EStopService.h"
#include "System.h"

void setup() {
   Serial.begin(115200);

    System_Init();
    OLED_Init();
    Radio_Init();
    
    transitionTo(STATE_SELECTING_CHANNEL);
}

void loop() {
}