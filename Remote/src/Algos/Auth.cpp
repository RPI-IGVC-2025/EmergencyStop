#include <Arduino.h>
#include "Auth.h"
#include "Blake.h"
#include "PacketBuilder.h"

uint16_t usedMask = 0;
uint32_t prevMinNonce = 0;

bool verifyPacket(Packet *pkt)
{
    return verifySequence(pkt) && verifyNonce(pkt) && verifyHash(pkt);
}

bool verifySequence(Packet *pkt)
{
    static uint32_t lastSeqNum = 0;
    if (pkt->seqNum <= lastSeqNum)
    {
        return false; // Reject if sequence number is not strictly increasing
        Serial.println("Sequence number check failed");
    }
    lastSeqNum = pkt->seqNum;
    return true;
}

bool verifyNonce(Packet *pkt)
{

    // 1. Determine the base of the current 10-block range
    uint32_t currentMinNonce = (pkt->seqNum / 10) * 10;

    // 2. If we moved to a new block, reset the tracking mask
    if (currentMinNonce > prevMinNonce)
    {
        usedMask = 0;
        prevMinNonce = currentMinNonce;
    }
    // Safety: If for some godforsaken reason we get an old block, reject it
    else if (currentMinNonce < prevMinNonce)
    {
        Serial.println("Nonce block check failed");
        return false;
    }

    // 3. Calculate index (should be 0-9)
    int32_t index = (int32_t)pkt->nonce - (int32_t)currentMinNonce;

    // 4. Bounds Check: Ensure the nonce is actually within the 10-block range
    if (index < 0 || index >= 10)
    {
        Serial.println("Out of bounds nonce");
        return false; // Out of bounds nonce
    }

    // 5. Bitmask Check: (1 << index) creates the "stencil" for this specific nonce
    if (!(usedMask & (1 << index)))
    {
        // If the bit was 0 (NOT used), mark it as used and return 1 (Valid)
        usedMask |= (1 << index);
        return true;
    }
    else
    {
        // Bit was already 1, this is a replay
        Serial.println("Nonce replay detected");
        return false;
    }
}

bool verifyHash(Packet *pkt) {
    uint8_t recievedHash[32];
    memcpy(recievedHash, pkt->hash, 32);

    memset(pkt->hash, 0, 32); // Clear the hash field for recomputation

    // Reconstruct the data that was hashed: command + seqNum + nonce
    generateBLAKE(pkt);

    // Compute the BLAKE2s hash of the reconstructed data

    // Compare the computed hash with the hash in the packet
    if(memcmp(recievedHash, pkt->hash, 32) == 0) {
        return true; // Hash matches, packet is authentic
    } else {
        Serial.println("Hash verification failed");
        return false; // Hash does not match, packet may be tampered with
    }
}