#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H
#include <Arduino.h>

const int encoder0PinA = 11; // ESP32 pin GPIO11 connected to the rotary encoder's CLK pin
const int encoder0PinB  = 12; // ESP32 pin GPIO12 connected to the rotary encoder's DT pin

void rotaryEncoder_Init();
void setRotaryPos();
int getRotaryPos();

#endif