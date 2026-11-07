/**
 * @file PWMServo.h
 * @brief Host stand-in for Teensyduino's PWMServo: records the last angle written.
 */
#pragma once

#include <stdint.h>

struct PWMServo
{
  int lastWrite = -1;
  uint8_t attach(int) { return 1; }
  void write(int angle) { lastWrite = angle; }
};
