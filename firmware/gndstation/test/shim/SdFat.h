/**
 * @file SdFat.h
 * @brief Host stand-in for SdFat: files keep what is written to them in memory.
 *
 * Holds only what the ground station's SD_Card.cpp uses. No file exists until a test says
 * so, and reading a file returns end of file.
 */
#pragma once

#include <stdint.h>
#include <string.h>

#include <string>

#define FILE_READ 0
#define FILE_WRITE 1
#define DEDICATED_SPI 1
#define SD_SCK_MHZ(maxMhz) (maxMhz)
#define SdSpiConfig(cs, opt, clock, spi) (0)

template <uint8_t MisoPin, uint8_t MosiPin, uint8_t SckPin>
class SoftSpiDriver
{
};

class FsFile
{
public:
  std::string text;
  bool isOpen = false;
  bool open(const char *, int)
  {
    isOpen = true;
    return true;
  }
  size_t write(const char *data, size_t n)
  {
    text.append(data, n);
    return n;
  }
  size_t println(const char *s)
  {
    text += s;
    text += "\r\n";
    return strlen(s) + 2;
  }
  int read() { return -1; }
  bool sync() { return true; }
  void flush() {}
  bool close()
  {
    isOpen = false;
    return true;
  }
};

class SdFs
{
public:
  bool begin(int) { return true; }
  bool exists(const char *) { return false; }
  FsFile open(const char *, int) { return FsFile(); }
};
