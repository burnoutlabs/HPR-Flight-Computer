/**
 * @file GPS.h
 * @brief UBLOX GPS configuration and the distance calculation, in GPS.cpp.
 */
#pragma once

#include <Arduino.h>

void restoreGPSdefaults();
void configGPS();
void GPSpowerSaveMode();
void calcChecksum(byte *checksumPayload, byte payloadSize);
void sendUBX(byte *UBXmsg, byte msgLength);
byte getUBX_ACK(byte *msgID);
void printHex(uint8_t *data, uint8_t length);
float calcGPSdist(float Alat, float Alon, float Blat, float Blon);
