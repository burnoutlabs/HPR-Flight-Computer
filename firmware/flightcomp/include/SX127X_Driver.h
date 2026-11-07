/**
 * @file SX127X_Driver.h
 * @brief Driver for the SX127X LoRa radio, in SX127X_Driver.cpp.
 */
#pragma once

#include <Arduino.h>

bool beginSX127X();
void sleepSX127X();
bool setPwrSX127X(int8_t pwr);
bool sendPktSX127X(uint8_t *data, uint8_t len);
bool config70cmSX127X();
bool setFreqSX127X(float freq);
bool setModeSX127X(uint8_t mode);
void clearFlagsSX127X();
