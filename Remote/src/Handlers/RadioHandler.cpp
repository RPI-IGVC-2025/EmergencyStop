#include "RadioHandler.h"

#include <Arduino.h>

#include "Algos/Auth.h"
#include "Algos/PacketBuilder.h"
#include "System.h"

HardwareSerial HC12(1);

const int hc12_rx = 18;
const int hc12_tx = 17;

const int SET_PIN = 5;

RadioState radioState = {
    .desiredChannel = 1,
    .currentChannel = 1
};

void Radio_Init() {
    HC12.begin(9600, SERIAL_8N1, hc12_rx, hc12_tx);
    pinMode(SET_PIN, OUTPUT);
    digitalWrite(SET_PIN, HIGH);
    HC12setDefault(); // Default to channel 1 on startup
}

bool getNextFrame(Packet* outPkt) {
    // If there aren't even enough bytes for a full packet, bail immediately
    if (HC12.available() < sizeof(Packet))
        return false;

    // Hunt for Syncbyte
    if (HC12.peek() != SYNC_BYTE) {
        HC12.read();
        return false;
    }

    // Read the frame
    uint8_t tempBuf[sizeof(Packet)];
    HC12.readBytes(tempBuf, sizeof(Packet));

    // Verify footer
    if (tempBuf[sizeof(Packet) - 1] != FOOT_BYTE) {
        return false;
    }

    memcpy(outPkt, tempBuf, sizeof(Packet));
    return true;
}

void sendPacket(Packet* pkt) {
    uint8_t buffer[sizeof(Packet)];
    memcpy(buffer, pkt, sizeof(Packet));

    HC12.write(buffer, sizeof(Packet));

    clearPacket(pkt);
}

void clearPacket(Packet* pkt) {
    memset(pkt, 0, sizeof(Packet));
}

void HC12setDefault() {
    char command[16];
    int len = snprintf(command, sizeof(command), "AT+DEFAULT");
    HC12sendCommand(command);
}

void HC12switchChannel(uint16_t newChannel) {
    char command[16];
    int len = snprintf(command, sizeof(command), "AT+C%03d", newChannel);
    HC12sendCommand(command);
}

void HC12sendCommand(char* command) {
    digitalWrite(SET_PIN, LOW);  
    vTaskDelay(pdMS_TO_TICKS(100)); // Entrance delay
    
    // Clear any "garbage" in the buffer before sending
    while(HC12.available()) HC12.read();

    HC12.print(command);
    
    Serial.print("Sent to HC12: "); Serial.println(command);

    vTaskDelay(pdMS_TO_TICKS(500)); // Generous buffer

    if (HC12.available()) {
        while (HC12.available()) {
            String response = HC12.readString();
            Serial.print("HC12 Response: "); Serial.println(response);
        }
    } else {
        Serial.println("HC12 SILENT - No response.");
    }

    digitalWrite(SET_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(100)); // Exit delay
}

uint16_t getCurrentChannel() {
    return radioState.currentChannel;
}

void setDesiredChannel(uint16_t newChannel) {
    radioState.desiredChannel = newChannel;
}
