/**
 * @file Rotation.h
 * @brief Rotation, active stabilization and return-to-base routines in Rotation.cpp.
 */
#pragma once

#include <Arduino.h>

// getQuatRotn() runs in double precision on the Teensy 4.x FPU and in single
// precision on the other boards
#if defined(__IMXRT1062__)
void getQuatRotn(double dx, double dy, double dz, double gyroGain);
#else
void getQuatRotn(float dx, float dy, float dz, float gyroGain);
#endif

void getDCM2DRotn(long dx, long dy, long dz, float gyroGain);
void setCanards();
void setRTB();
float calcGPSdist(float Alon, float Alat, float Blon, float Blat);
void magRotn();
