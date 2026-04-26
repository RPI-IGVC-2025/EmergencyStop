#include "RotaryEncoder.h"

#include <Arduino.h>

int encoder0Pos = 0;
int encoder0PinALast = LOW;
int n = LOW;

void rotaryEncoder_Init() {
  pinMode (encoder0PinA, INPUT_PULLUP);
  pinMode (encoder0PinB, INPUT_PULLUP);
}

/* Read Quadrature Encoder
   Connect Encoder to Pins encoder0PinA, encoder0PinB, and +5V.

   Sketch by max wolf / www.meso.net
   v. 0.1 - very basic functions - mw 20061220
*/
// Thank you to max wolf for this rotary encoder code.

void setRotaryPos() {
  n = digitalRead(encoder0PinA);
  if ((encoder0PinALast == LOW) && (n == HIGH)) {
    if (digitalRead(encoder0PinB) == LOW) {
      encoder0Pos--;
      if (encoder0Pos < 1) encoder0Pos = 16;
    } else {
      encoder0Pos++;
      if (encoder0Pos > 16) encoder0Pos = 1;
    }
  }
  encoder0PinALast = n;
}

int getRotaryPos() {
  return encoder0Pos;
}
