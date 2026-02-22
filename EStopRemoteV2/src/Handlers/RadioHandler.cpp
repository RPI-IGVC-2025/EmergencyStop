#include "RadioHandler.h"

#include <Arduino.h>

#include "Auth.h"
#include "Packet.h"
#include "System.h"

HardwareSerial HC12(1);

const int hc12_rx = 18;
const int hc12_tx = 17;

void initRadio() {
    HC12.begin(9600, SERIAL_8N1, hc12_rx, hc12_tx);
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

void processValidatedPacket(Packet* pkt) {
    if (!verifyPacket(pkt)) {
        return;
    }

    switch (pkt->command) {
        case 'H':
            completeHandshake();
        default:
            Serial.println("wtf");
    }
}
