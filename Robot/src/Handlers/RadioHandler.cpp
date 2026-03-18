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
    .currentChannel = 1};

void Radio_Init() {
    HC12.begin(9600, SERIAL_8N1, hc12_rx, hc12_tx);
    pinMode(SET_PIN, OUTPUT);
    digitalWrite(SET_PIN, HIGH);
    delay(1000);  // Wait for HC12 to initialize
    while (!HC12setDefault()) {
        Serial.println("Failed to set HC12 to default settings. Retrying...");
        delay(300);
    }  // Default to channel 1 on startup
    state.radioReady = true;
}

bool getNextFrame(Packet* outPkt) {
    // 1. Bail immediately if there's no data at all
    if (HC12.available() == 0) {
        return false;
    }

    // 2. THE INCINERATOR: Burn ALL trash until a Sync Byte is found
    // Notice this is a WHILE loop, and it doesn't care about sizeof(Packet)
    while (HC12.available() > 0 && HC12.peek() != SYNC_BYTE) {
        HC12.read(); // Throw it away
    }

    // 3. Check what's left. If we burned the trash and don't have a full packet yet, wait.
    if (HC12.available() < sizeof(Packet)) {
        // It's completely normal to hit this. It means the packet is 
        // literally flying through the air right now and we are waiting for the rest of it.
        return false; 
    }

    // 4. If we reach here, we GUARANTEE the first byte is SYNC_BYTE 
    // AND we have exactly enough bytes to pull a full packet.
    uint8_t tempBuf[sizeof(Packet)];
    HC12.readBytes(tempBuf, sizeof(Packet));

    // 5. Verify footer
    if (tempBuf[sizeof(Packet) - 1] != FOOT_BYTE) {
        Serial.println("Alignment lost - Footer mismatch");
        return false;
    }

    memcpy(outPkt, tempBuf, sizeof(Packet));
    return true;
}

void sendPacket(Packet* pkt) {
    uint8_t buffer[sizeof(Packet)];
    memcpy(buffer, pkt, sizeof(Packet));

    for (int i = 0; i < sizeof(Packet); i++) {
        Serial.print(buffer[i]);
        Serial.print(",");
    }
    Serial.println();

    HC12.write(buffer, sizeof(Packet));

    clearPacket(pkt);
}

void clearPacket(Packet* pkt) {
    memset(pkt, 0, sizeof(Packet));
}

bool HC12setDefault() {
    char command[16];
    int len = snprintf(command, sizeof(command), "AT+DEFAULT");
    if (memcmp(HC12sendCommand(command), "OK+DEFAULT", len) == 0) {
        return true;
    }
    return false;
}

bool HC12switchChannel(uint16_t newChannel) {
    char command[16];
    int len = snprintf(command, sizeof(command), "AT+C%03d", newChannel);
    char response[16];
    len = snprintf(response, sizeof(response), "OK+C%03d", newChannel);
    if (memcmp(HC12sendCommand(command), response, len) == 0) {
        return true;
    }
    return false;
}

char* HC12sendCommand(char* command) {
    digitalWrite(SET_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(100));  // Entrance delay

    // Clear any "garbage" in the buffer before sending
    while (HC12.available()) HC12.read();

    HC12.print(command);

    Serial.print("Sent to HC12: ");
    Serial.println(command);

    vTaskDelay(pdMS_TO_TICKS(500));  // Generous buffer

    if (HC12.available()) {
        String response = "";
        while (HC12.available()) {
            response = HC12.readString();
            Serial.print("HC12 Response: ");
            Serial.println(response);

            digitalWrite(SET_PIN, HIGH);
            vTaskDelay(pdMS_TO_TICKS(100));  // Exit delay
        }
        return strdup(response.c_str());
    } else {
        Serial.println("HC12 SILENT - No response.");
        return strdup("");
    }
}

uint16_t getCurrentChannel() {
    return radioState.currentChannel;
}

void setDesiredChannel(uint16_t newChannel) {
    radioState.desiredChannel = newChannel;
}
