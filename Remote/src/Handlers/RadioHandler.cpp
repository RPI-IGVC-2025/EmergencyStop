#include "RadioHandler.h"

#include <Arduino.h>

#include "Algos/Auth.h"
#include "Algos/PacketBuilder.h"
#include "System.h"

HardwareSerial HC12(1);

const int hc12_rx = 18;
const int hc12_tx = 17;

RadioState radioState = {
    .currentChannel = 1
};   

void Radio_Init() {
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
    // Process the packet based on its command and status
    switch (pkt->command) {
        case MessageType::ESTOP:
            state.isEstopped = true;
            break;
        case MessageType::HEARTBEAT:
            state.heartbeatActive = true;
            break;
        case MessageType::HANDSHAKE:
            state.isSynced = true;
            break;
        default:
            // Handle other message types or ignore
            break;
    }
}

void sendHC12Command(char* command) {
    HC12.println(command);
}

void switchChannel(uint16_t newChannel) {
    // Implement the logic to switch the HC-12 to the new channel
    // This typically involves sending specific AT commands to the HC-12 module
    // For example, you might send "AT+C001" for channel 1, "AT+C002" for channel 2, etc.
    char command[10];
    snprintf(command, sizeof(command), "AT+C%03d", newChannel);
    HC12.println(command);
    radioState.currentChannel = newChannel;
}
