/**
 * @file gndstation.h
 * @brief Setup, the receive loop and the radio interrupt handlers in gndstation.cpp.
 */
#pragma once

#include <Arduino.h>
#include <TinyGPS++.h>

// GPS Setup (defined in gndstation.cpp)
extern TinyGPSPlus GPS;

void setup();
void loop();
void clearIRQ1();
void clearIRQ2();
