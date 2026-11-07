/**
 * @file Parse_Packet.h
 * @brief Handling of each received telemetry packet, in Parse_Packet.cpp.
 */
#pragma once

#include <Arduino.h>

bool processPacket();
void preflightPacket(byte rxPacket[]);
void inflightPacket(byte rxPacket[]);
void postflightPacket(byte rxPacket[]);
