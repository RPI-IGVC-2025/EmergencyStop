#include <Arduino.h>
#include "System.h"
#include "Handlers/RadioHandler.h"
#include "Handlers/OLEDHandler.h"
#include "Services/HandshakeService.h"
#include "Services/ChannelService.h"

void setup() {
  Serial.begin(115200);
  System_Init();
  OLED_Init();
  while(!state.OLEDActive){}
  Radio_Init();
  //HandshakeService_Init();
  SelectChannelService_Init();
  while(!state.channelLocked) {}
  vTaskDelay(pdMS_TO_TICKS(4000)); // Small delay to ensure channel lock is displayed on OLED before we start handshakes
  HandshakeService_Init();
}

void loop() {

}