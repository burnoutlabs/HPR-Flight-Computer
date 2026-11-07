/**
 * @file Calibration.h
 * @brief Accelerometer, magnetometer, barometer and canard calibration routines in Calibration.cpp.
 */
#pragma once

#include <Arduino.h>

void accelCalibrate();
void magCalibrate();
void writeCalibration(int16_t inValue, byte eepromStart);
void setOrientation();
void exchangeAxes(char A, char B);
void flipAxis(char A);
void readOrientation();
void baroCalibrate();
void setCanardTrim();
void wiggleServo(byte servo);
