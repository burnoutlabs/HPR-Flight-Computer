/**
 * @file Arduino.h
 * @brief Host stand-in for the Teensyduino core, for the native unit tests.
 *
 * Holds only what the firmware sources under test use. The constants match
 * Teensyduino's wiring.h. Time does not advance on its own: a test sets it
 * with shimSetMicros() / shimAdvanceMicros(), and micros()/millis() read it.
 * Serial accepts and discards everything.
 */
#pragma once

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t byte;
typedef bool boolean;

#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

#define F(string_literal) (string_literal)
#define PROGMEM

#define lowByte(w) ((uint8_t)((w) & 0xFF))
#define highByte(w) ((uint8_t)((w) >> 8))

// No interrupts on the host
inline void noInterrupts() {}
inline void interrupts() {}

// Test-driven clock
inline uint32_t shimMicrosNow = 0;
inline void shimSetMicros(uint32_t us) { shimMicrosNow = us; }
inline void shimAdvanceMicros(uint32_t us) { shimMicrosNow += us; }
inline uint32_t micros() { return shimMicrosNow; }
inline uint32_t millis() { return shimMicrosNow / 1000; }

// Serial that discards its output
struct ShimSerial
{
  void begin(unsigned long) {}
  template <typename... Args>
  size_t print(Args &&...) { return 0; }
  template <typename... Args>
  size_t println(Args &&...) { return 0; }
  int available() { return 0; }
  int read() { return -1; }
  explicit operator bool() const { return true; }
};
inline ShimSerial Serial;
