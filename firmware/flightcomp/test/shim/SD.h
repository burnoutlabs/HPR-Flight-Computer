/**
 * @file SD.h
 * @brief Host stand-in for the Teensy SD library: the File type that board.h and SD_Card.h name.
 *
 * Writes are discarded. The host tests stub the SD card functions (SD.cpp) themselves.
 */
#pragma once

#include <stddef.h>

struct File
{
  size_t write(const char *, size_t length) { return length; }
  void flush() {}
  void close() {}
  explicit operator bool() const { return true; }
};
