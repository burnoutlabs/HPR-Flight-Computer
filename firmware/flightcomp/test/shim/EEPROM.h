/**
 * @file EEPROM.h
 * @brief Host stand-in for Teensyduino's EEPROM library: an in-memory byte array.
 *
 * Sized to the Teensy 4.1's 4284 bytes of emulated EEPROM, the largest of the
 * supported boards. Starts erased (0xFF), as a new part does; shimErase()
 * returns it to that state between tests.
 */
#pragma once

#include <stdint.h>
#include <string.h>

struct ShimEEPROM
{
  static const int size = 4284;
  uint8_t bytes[size];

  ShimEEPROM() { shimErase(); }
  void shimErase() { memset(bytes, 0xFF, sizeof(bytes)); }
  uint8_t read(int address) { return bytes[address]; }
  void write(int address, uint8_t value) { bytes[address] = value; }
  void update(int address, uint8_t value) { bytes[address] = value; }
  uint16_t length() { return size; }
  template <typename T>
  T &get(int address, T &value)
  {
    memcpy(&value, &bytes[address], sizeof(T));
    return value;
  }
  template <typename T>
  const T &put(int address, const T &value)
  {
    memcpy(&bytes[address], &value, sizeof(T));
    return value;
  }
};
inline ShimEEPROM EEPROM;
