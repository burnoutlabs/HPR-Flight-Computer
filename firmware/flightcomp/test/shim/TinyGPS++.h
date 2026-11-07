/**
 * @file TinyGPS++.h
 * @brief Host stand-in for TinyGPSPlus, holding only the members the sources under test read.
 *
 * Values are plain fields that a test sets directly; nothing parses NMEA.
 */
#pragma once

struct TinyGPSSpeed
{
  double metersPerSecond = 0.0;
  double mps() { return metersPerSecond; }
};

struct TinyGPSLocation
{
  double latitude = 0.0;
  double longitude = 0.0;
  double lat() { return latitude; }
  double lng() { return longitude; }
};

struct TinyGPSPlus
{
  TinyGPSLocation location;
  TinyGPSSpeed speed;
};
