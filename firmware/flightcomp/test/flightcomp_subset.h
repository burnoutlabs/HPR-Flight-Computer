/**
 * @file flightcomp_subset.h
 * @brief Compiles the real Speed_Trig.ino and Rotation.ino sources for the host.
 *
 * The sketch's .ino tabs are one translation unit on the board, so this does the
 * same for the part under test: the shims in test/shim stand in for the Teensy
 * core and libraries, globals.cpp supplies the real globals, and the two tabs
 * are included as they are, not copied. Include it from exactly one .cpp per test
 * suite: it defines globals.
 *
 * Define __IMXRT1062__ before including it to select the Teensy 4.x (double)
 * path of getQuatRotn(); leave it undefined for the Teensy 3.x (float) path.
 */
#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <PWMServo.h>
#include <TinyGPS++.h>

#include "../src/declarations.h"
#include "../src/globals.cpp"

// Objects that flightcomp.ino defines and Rotation.ino uses.
TinyGPSPlus GPS;
PWMServo canardYaw1;
PWMServo canardYaw2;
PWMServo canardPitch3;
PWMServo canardPitch4;

// The Arduino builder generates prototypes for every .ino function; Rotation.ino
// calls calcGPSdist() before defining it.
float calcGPSdist(float Alon, float Alat, float Blon, float Blat);

#include "../src/Speed_Trig.ino"

#include "../src/Rotation.ino"
