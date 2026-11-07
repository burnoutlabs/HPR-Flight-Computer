/**
 * @file TinyGPS++.h
 * @brief Host stand-in for TinyGPSPlus, holding only the members the sources under test read.
 *
 * Values are plain fields that a test sets directly; nothing parses NMEA.
 */
#pragma once

#include <stdint.h>

struct TinyGPSSpeed
{
  double metersPerSecond = 0.0;
  double mps() { return metersPerSecond; }
  double mph() { return metersPerSecond * 2.23693629; }
};

struct TinyGPSCourse
{
  double degrees = 0.0;
  double deg() { return degrees; }
};

struct TinyGPSLocation
{
  double latitude = 0.0;
  double longitude = 0.0;
  double lat() { return latitude; }
  double lng() { return longitude; }
};

struct TinyGPSTime
{
  uint8_t hours = 0;
  uint8_t minutes = 0;
  uint8_t seconds = 0;
  uint8_t centiseconds = 0;
  uint8_t hour() { return hours; }
  uint8_t minute() { return minutes; }
  uint8_t second() { return seconds; }
  uint8_t centisecond() { return centiseconds; }
};

struct TinyGPSPlus
{
  TinyGPSLocation location;
  TinyGPSSpeed speed;
  TinyGPSCourse course;
  TinyGPSTime time;
};
