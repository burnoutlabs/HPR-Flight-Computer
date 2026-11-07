/**
 * @file Parse_Packet.h
 * @brief Parsing of the preflight, inflight and postflight telemetry packets, in Parse_Packet.cpp.
 */
#pragma once

#include <Arduino.h>

void preflightPacket(byte rxPacket[]);
void inflightPacket(byte rxPacket[]);
void postflightPacket(byte rxPacket[]);
