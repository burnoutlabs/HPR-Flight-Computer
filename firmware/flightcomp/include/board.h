/**
 * @file board.h
 * @brief Board-dependent library choices, and the declarations whose type depends on them.
 *
 * @details
 * The supported Teensy boards differ in their I2C and SD card libraries. Every declaration
 * shared between source files whose type depends on the board is here, so each file makes
 * the same choice. Board #if blocks inside a function stay in that function.
 */
#pragma once

#include <Arduino.h>

// Teensy 3.X
#if defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__MK20DX256__)
// i2c_t3 is needed to enable the different pin options of bus0
#include <i2c_t3.h>
typedef i2c_t3 TwoWire;
// Teensy 4.X
#else
#include <Wire.h>
#endif

// SD card library and files (defined in SD.cpp)
#if defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(ARDUINO_TEENSY41)
// Teensy 3.5, 3.6, 4.0, 4.1
#include <SdFat.h>

// SDIO Setup
extern SdFs SD;
extern FsFile outputFile;
extern FsFile settingsFile;
extern FsFile gpsFile;

#else
// Teensy 3.2 or 4.0
#include <SD.h>

extern File outputFile;
extern File settingsFile;
extern File gpsFile;
#endif

// Timer for the radio (defined in flightcomp.cpp)
#if defined(STM32_CORE_VERSION)
extern TIM_TypeDef *Instance;
extern HardwareTimer *radioTimer;
extern HardwareTimer *syncTimer;
#else
extern IntervalTimer radioTimer;
extern IntervalTimer syncTimer;
#endif
