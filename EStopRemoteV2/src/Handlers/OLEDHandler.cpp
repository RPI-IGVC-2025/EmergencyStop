#include "OLEDHandler.h"

#include <Arduino.h>
#include <FreeRTOS.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <task.h>

void OLEDLoop(void* pvParameters);

const int SDA_PIN = 1;
const int SCL_PIN = 2;

volatile bool OLEDup = false;

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL_PIN, /* data=*/SDA_PIN, /* reset=*/8);

void initOLED() {
    xTaskCreatePinnedToCore(
        OLEDLoop,      /* Task function. */
        "OLEDHandler", /* name of task. */
        4096,          /* Stack size */
        NULL,
        6,                /* Lowest Priority out of all 3 tasks */
        &OLEDTaskHandler, /* Task handle to keep track of created task */
        1);               /* pin task to core 0 */
    delay(500);

    u8g2.begin();
}

void OLEDLoop(void* pvParameters) {
    initSequence();

    for (;;) {
        u8g2.clearBuffer();                   // clear the internal memory
        u8g2.setFont(u8g2_font_ncenB08_tr);   // choose a suitable font
        u8g2.drawStr(0, 10, "Hello World!");  // write something to the internal memory
        u8g2.sendBuffer();                    // transfer internal memory to the display
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void initSequence() {
    
}

bool isOLEDUp() {
    return OLEDup;
}