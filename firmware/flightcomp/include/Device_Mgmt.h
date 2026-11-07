/**
 * @file Device_Mgmt.h
 * @brief Device function-pointer assignment and sensor orientation, in Device_Mgmt.cpp.
 */
#pragma once

#include <Arduino.h>

void setPolyMorphs();
void orientAccel();
void orientMag();
void orientGyro();
void orientHighG();
void dummyVoid();
bool dummyBool();
bool dummyBool1(float freq);
bool dummyBool2(int8_t pwr);
bool dummyBool3(uint8_t *data, uint8_t len);
