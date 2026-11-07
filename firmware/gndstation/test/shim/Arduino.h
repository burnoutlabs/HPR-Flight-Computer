/**
 * @file Arduino.h
 * @brief Host stand-in for the Teensyduino core, for the ground station's native unit tests.
 *
 * Holds only what the sources under test use; the constants match Teensyduino's. Time does
 * not advance on its own: a test sets it with shimSetMicros(), and micros()/millis() read
 * it. Pins and delays do nothing. Serial accepts and discards everything. String keeps no
 * text: the sources under test only construct the event and pyro tables with it.
 */
#pragma once

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t byte;
typedef bool boolean;

#define F(string_literal) (string_literal)
#define PROGMEM

#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define DEC 10
#define HEX 16
#define BIN 2

// Test-driven clock
inline uint32_t shimMicrosNow = 0;
inline void shimSetMicros(uint32_t us) { shimMicrosNow = us; }
inline uint32_t micros() { return shimMicrosNow; }
inline uint32_t millis() { return shimMicrosNow / 1000; }

inline void delay(uint32_t) {}
inline void delayMicroseconds(uint32_t) {}
inline void pinMode(uint8_t, uint8_t) {}
inline void digitalWrite(uint8_t, uint8_t) {}
inline void noInterrupts() {}
inline void interrupts() {}

// Teensyduino's itoa (not in the C library)
inline char *itoa(int value, char *str, int radix)
{
  if (radix == 10)
  {
    sprintf(str, "%d", value);
  }
  else
  {
    sprintf(str, "%x", value);
  }
  return str;
}

struct String
{
  String(const char *) {}
};

// Serial that discards its output
struct ShimSerial
{
  void begin(unsigned long) {}
  template <typename... Args>
  size_t print(Args &&...) { return 0; }
  template <typename... Args>
  size_t println(Args &&...) { return 0; }
};
inline ShimSerial Serial;
