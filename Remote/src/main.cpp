#include <Arduino.h>
#include "System.h"
#include "Handlers/RadioHandler.h"
#include "Handlers/OLEDHandler.h"
#include "Services/HandshakeService.h"

void setup() {
  Serial.begin(115200);
  System_Init();
  OLED_Init();
  while(!state.OLEDActive){}
  //HandshakeService_Init();
}

void loop() {

}