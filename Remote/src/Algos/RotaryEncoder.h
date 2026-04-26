#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H
#include <Arduino.h>

const int encoder0PinA = 16; // ESP32 pin GPIO25 connected to the rotary encoder's CLK pin
const int encoder0PinB  = 15; // ESP32 pin GPIO26 connected to the rotary encoder's DT pin

void rotaryEncoder_Init();
void setRotaryPos();
int getRotaryPos();

#endif