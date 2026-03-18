#ifndef ADC_H
#define ADC_H
#include <Arduino.h>

const int ADC_MAX_VALUE = 16;

void ADC_Read(int* value);

#endif