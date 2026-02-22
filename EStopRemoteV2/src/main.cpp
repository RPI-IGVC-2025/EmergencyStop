#include <Arduino.h>
#include "System.h"
#include "Handlers/RadioHandler.h"

void setup() {
  Serial.begin(115200);
  System_Init();
  Radio_Init();
  
}

void loop() {
 
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}