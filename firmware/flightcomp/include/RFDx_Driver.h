/**
 * @file RFDx_Driver.h
 * @brief Driver for the RFD900 serial telemetry radio, in RFDx_Driver.cpp.
 */
#pragma once

#include <Arduino.h>

bool beginRFD900();
bool sendPktRFD900(uint8_t *data, uint8_t len);
