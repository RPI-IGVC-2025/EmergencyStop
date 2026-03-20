#include <Arduino.h>

#include "Algos/PacketBuilder.h"
#include "Handlers/RadioHandler.h"
#include "Services/HandshakeService.h"
#include "System.h"

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    System_Init();
    Radio_Init();

    transitionTo(STATE_HANDSHAKING);
}

void loop(){}