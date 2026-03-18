#include <Arduino.h>

#include "Algos/PacketBuilder.h"
#include "Handlers/RadioHandler.h"
#include "Services/HandshakeService.h"
#include "System.h"

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    delay(5000);
    Serial.println("Ts is running");
    System_Init();
    Radio_Init();
    while (!state.radioReady) {
        Serial.println("Radio isnt ready");
    }
    Serial.println("Radio ready, starting handshake service...");
    HandshakeService_Init();
}

bool heartbeatIsRunning = false;
void loop() {
    if (state.isSynced && !heartbeatIsRunning) {
        Serial.println("Ts works");
        heartbeatIsRunning = true;
    }
}