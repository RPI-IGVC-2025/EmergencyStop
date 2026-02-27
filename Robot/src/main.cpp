#include <Arduino.h>
#include "Algos/PacketBuilder.h"
#include "Handlers/RadioHandler.h"  
#include "System.h"
#include "Services/HandshakeService.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(5000);
  System_Init();
  Radio_Init();
  while(!state.radioReady){}
  Serial.println("Radio ready, starting handshake service...");
  HandshakeService_Init();
}

void loop() {
  // put your main code here, to run repeatedly:
}