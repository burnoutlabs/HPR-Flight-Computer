/**
 * @file SPI.h
 * @brief Host stand-in for the Teensy SPI library: the types, object and constants that
 * Bus_Mgmt.h names.
 */
#pragma once

#include <stdint.h>

#define MSBFIRST 1
#define SPI_MODE0 0x00

struct SPISettings
{
  SPISettings() {}
  SPISettings(uint32_t, uint8_t, uint8_t) {}
};

struct SPIClass
{
};
extern SPIClass SPI;
