#include "OLEDHandler.h"

#include <Arduino.h>
#include <FreeRTOS.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <task.h>

#include "System.h"

void OLEDTask(void* pvParameters);

const int SDA_PIN = 1;
const int SCL_PIN = 2;

volatile bool OLEDup = false;

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL_PIN, /* data=*/SDA_PIN, /* reset=*/8);

OLEDState oledState = {
    .handShakeComplete = false
};

void OLED_Init() {
    Wire.setClock(400000);  // Boost to 400kHz
    u8g2.begin();

    xTaskCreatePinnedToCore(
        OLEDTask,   /* Task function. */
        "OLEDTask", /* name of task. */
        4096,       /* Stack size */
        NULL,
        6,                /* Lowest Priority out of all 3 tasks */
        &OLEDTaskHandler, /* Task handle to keep track of created task */
        1);               /* pin task to core 0 */
    delay(500);
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void OLEDTask(void* pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);  // Exact 1s period
    initSequence();
    state.uptimeSeconds = 0; // Reset uptime counter at the start of the OLED task
    state.OLEDActive = true; // Signal that OLED is active

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        //if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            drawInfoScreen();
            //xSemaphoreGive(xMutex);  // ALWAYS give it back!
        //}
    }
}

void drawInfoScreen() {
    if(!state.channelLocked) {
        drawChannelSelectScreen();
        return;
    }

    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);

    // Border lines
    u8g2.drawLine(76, 18, 76, 63);
    u8g2.drawLine(0, 18, 126, 18);

    drawNetwork();
    drawTimeElapsed();
    drawBattery();

    // Estop Text
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(7, 30, "ESTOP IDLE");

    // EStop Icon
    u8g2.drawXBMP(29, 38, 15, 16, IMAGE_IDLE);

    // Handshake
    u8g2.setFont(u8g2_font_5x8_tr);
    u8g2.drawStr(81, 29, "Handshake");
    // Handshake hand
    u8g2.drawXBMP(93, 36, 17, 16, IMAGE_HANDSHAKE);
    // Handshake dots
    u8g2.setFont(u8g2_font_5x7_mf);
    u8g2.drawStr(95, 60, formatDots(xTaskGetTickCount()));

    u8g2.sendBuffer();
}

void drawTimeElapsed() {
    // Time Elapsed Icon
    u8g2.drawXBMP(1, 1, 15, 16, IMAGE_UPTIME);
    // Time Elapsed Text
    u8g2.setFont(u8g2_font_6x13_tr);
    u8g2.drawStr(19, 14, formatTime(state.uptimeSeconds));
}

void drawNetwork() {
    // Network Icon
    if (state.isSynced) {
        u8g2.drawXBMP(80, 1, 15, 16, IMAGE_CONNECTED);
    } else {
        u8g2.drawXBMP(80, 1, 15, 16, IMAGE_NOT_CONNECTED);
    }
}

void drawBattery() {
        // Battery Icon
    switch(state.batteryMv) {
        case 8800 ... 9000:
            u8g2.drawXBMP(100, 2, 24, 16, IMAGE_BATTERY_FULL);
            break;
        case 8600 ... 8799:
            u8g2.drawXBMP(100, 2, 24, 16, IMAGE_BATTERY_5BAR);
            break;
        case 8400 ... 8599:
            u8g2.drawXBMP(100, 2, 24, 16, IMAGE_BATTERY_4BAR);
            break;
        default:
            u8g2.drawXBMP(100, 2, 24, 16, IMAGE_BATTERY_1BAR);
            break;
    }
}

void drawChannelSelectScreen() {
    //
}

void initSequence() {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // Layer 1
    u8g2.drawXBMP(0, 9, 130, 56, IMAGE_JERRY);

    // Layer 2
    u8g2.drawLine(0, 9, 127, 9);

    // Layer 3
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(33, 7, "RPI ROBOTICS");

    u8g2.sendBuffer();
    
    vTaskDelay(pdMS_TO_TICKS(7500));
    clearScreen();
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void clearScreen() {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    u8g2.sendBuffer();
}

char* formatTime(unsigned long seconds) {
    static char buffer[9];  // HH:MM:SS\0
    unsigned long hours = seconds / 3600;
    unsigned long minutes = (seconds % 3600) / 60;
    unsigned long secs = seconds % 60;
    snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%02lu", hours, minutes, secs);
    return buffer;
}

char* formatDots(TickType_t ticks) {
    static char buffer[4];                            // "..." and null terminator
    int numDots = (ticks / pdMS_TO_TICKS(1000)) % 4;  // Cycle through 0, 1, 2, 3 dots
    for (int i = 0; i < numDots; i++) {
        buffer[i] = '.';
    }
    buffer[numDots] = '\0';  // Null terminate the string
    return buffer;
}