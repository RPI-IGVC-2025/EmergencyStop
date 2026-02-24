#include <Arduino.h>
#include "ADC.h"

void ADC_Init() {
  pinMode(POTENTIOMETER_PIN, INPUT);
}

void ADC_Read(int* value) {
  *value = (analogRead(POTENTIOMETER_PIN)/pow(2,12))*ADC_MAX_VALUE;
}