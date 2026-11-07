/**
 * @file UBLOX_GNSS_Config.h
 * @brief UBLOX GNSS receiver configuration, in UBLOX_GNSS_Config.cpp.
 */
#pragma once

#include <Arduino.h>

void UBLOXrestorDefaults();
void UBLOXconfig();
void UBLOXpowerSave();
void calcChecksum(byte *checksumPayload, byte payloadSize);
void sendUBX(byte *UBXmsg, byte msgLength);
byte getUBX_ACK(byte *msgID);
void printHex(uint8_t *data, uint8_t length);
