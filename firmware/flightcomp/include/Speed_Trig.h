/**
 * @file Speed_Trig.h
 * @brief Fast table-based trigonometry in Speed_Trig.cpp.
 */
#pragma once

#include <Arduino.h>

float speedTan(int rotn);
int speedArcTan(float myValue);
float speedSin(int degree);
int speedArcSin(float myValue);
float speedCos(int degree);
int speedArcCos(float myValue);
int speedAtan2(float y, float x);
float cosSmallAngle(float rad);
float sinSmallAngle(float rad);
