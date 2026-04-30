#include <Arduino.h>
#include "System.h"

void Battery_Init() {
    pinMode(BATTERY_PIN, INPUT);
    analogReadResolution(12); // Set ADC resolution to 12 bits (0-4096)
    analogSetAttenuation(ADC_11db);
}

int returnMillivolts() {
    // // Read the raw ADC value (0-4095 for 12-bit ADC)
    // int adcValue = analogRead(BATTERY_PIN);
    // Serial.println(String(adcValue));
    // //Serial.println("Raw ADC Value: " + String(adcValue));
    
    // // Convert the ADC value to millivolts
    // // Assuming a reference voltage of 3.3V and a voltage divider that scales the battery voltage to this range
    // // For example, if the battery voltage is scaled down by a factor of 2, the formula would be:
    // // batteryVoltage = (adcValue / 4095.0) * 3.3 * 2
    // // Adjust the scaling factor as needed based on your specific hardware setup
    // float batteryVoltage = ((float)adcValue / 4095.0 ) * (3300.0) * (5.7);
    
    // return (int)(batteryVoltage);

    // Read the pin voltage directly in millivolts using the ESP32's hardware calibration
    int pinMv = analogReadMilliVolts(BATTERY_PIN);

    // If the function returns 0 or an error, handle it gracefully
    if (pinMv < 0) {
        return 0;
    }

    // Multiply by the actual voltage divider ratio (5.7)
    float batteryMv = (float)pinMv * 5.7;

    // Optional: Print to verify the ADC calibration
    // Serial.printf("Pin mV: %d | Calculated Battery mV: %.2f\n", pinMv, batteryMv);

    return (int)batteryMv;
}