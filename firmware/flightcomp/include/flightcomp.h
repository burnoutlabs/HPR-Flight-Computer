/**
 * @file flightcomp.h
 * @brief The flight loop, and the beeper, settings and timer helpers in flightcomp.cpp.
 */
#pragma once

#include <Arduino.h>
#include <PWMServo.h>
#include <TinyGPS++.h>

// Set code version
const float codeVersion = 4.72;

// GPS Setup (defined in flightcomp.cpp)
extern TinyGPSPlus GPS;

// Servo Setup (defined in flightcomp.cpp)
extern PWMServo canardYaw1;
extern PWMServo canardYaw2;
extern PWMServo canardPitch3;
extern PWMServo canardPitch4;
extern PWMServo actionServo5;
extern PWMServo actionServo6;
extern PWMServo actionServo7;
extern PWMServo actionServo8;

void loop(void);
void parseBeep(uint32_t value, uint8_t array[], uint8_t arrayLen);
void readEEPROMsettings();
void clearIRQ();
void timerBuildPkt();
void timerSyncPkt();
