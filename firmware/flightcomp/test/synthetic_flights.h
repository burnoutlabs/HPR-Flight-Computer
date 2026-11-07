/**
 * @file synthetic_flights.h
 * @brief Simple vertical flight profiles for driving the flight step on the host.
 *
 * @details
 * A point mass flies straight up and down: motor burns, quadratic drag, a drogue from shortly
 * after its true apogee and a main below a set height, then rest on the pad. The profile is
 * integrated once at 1 ms steps and looked up per sample. It is a test input, not a model of any
 * vehicle; the logged flights are in the replay suite.
 */
#pragma once

#include <math.h>

#include <vector>

#include "flight_harness.h"

/** One motor burn: constant thrust acceleration over [start, start + duration). */
struct Burn
{
  float start;
  float duration;
  float thrust; // m/s^2
};

struct ProfileSpec
{
  std::vector<Burn> burns;
  float dragAscent = 0.0002F; // 1/m
  float drogueDelay = 1.0F;   // s after the true apogee
  float drogueDrag = 0.0061F; // terminal about 40 m/s
  float mainHeight = 250.0F;  // m above the pad
  float mainDrag = 0.2F;      // terminal about 7 m/s
  float padAltitude = 1400.0F;
  float duration = 200.0F;          // s from the start of the profile
  float pitchRate = 0.0F;           // deg/s about X while the motor burns (tilt input to the rotation code)
  std::vector<Burn> padBumps;       // short accelerations while on the pad (no lift)
};

/** Standard-atmosphere pressure at an altitude above sea level. */
inline float standardPressure(float altitude) { return 101325.0F * powf(1.0F - 2.25577e-5F * altitude, 5.25588F); }

class SyntheticFlight
{
public:
  explicit SyntheticFlight(const ProfileSpec &spec) : spec_(spec)
  {
    const double dt = 0.001;
    double h = 0.0, v = 0.0, apogeeTime = -1.0;
    bool launched = false, landed = false, mainOut = false;
    const size_t n = (size_t)(spec.duration / dt) + 1;
    force_.resize(n);
    height_.resize(n);
    burning_.resize(n);
    for (size_t i = 0; i < n; i++)
    {
      const double t = i * dt;
      double thrust = 0.0;
      for (const Burn &b : spec.burns)
      {
        if (t >= b.start && t < b.start + b.duration)
        {
          thrust += b.thrust;
        }
      }
      double drag = spec.dragAscent;
      if (apogeeTime >= 0.0 && t > apogeeTime + spec.drogueDelay)
      {
        drag = spec.drogueDrag;
        if (mainOut || h < spec.mainHeight)
        {
          mainOut = true;
          drag = spec.mainDrag;
        }
      }
      double a = thrust - 9.80665 - drag * v * fabs(v);
      if (!launched && thrust > 9.80665)
      {
        launched = true;
      }
      if (!launched || landed)
      {
        a = 0.0;
        v = 0.0;
      }
      double f = a + 9.80665;
      for (const Burn &b : spec.padBumps)
      {
        if (t >= b.start && t < b.start + b.duration)
        {
          f += b.thrust;
        }
      }
      force_[i] = (float)f;
      height_[i] = (float)h;
      burning_[i] = thrust > 0.0;
      if (launched && !landed)
      {
        const double vPrev = v;
        v += a * dt;
        h += v * dt;
        if (apogeeTime < 0.0 && vPrev > 0.0 && v <= 0.0)
        {
          apogeeTime = t;
        }
        if (h <= 0.0 && apogeeTime >= 0.0)
        {
          h = 0.0;
          v = 0.0;
          landed = true;
        }
      }
    }
  }

  Truth at(uint32_t micros) const
  {
    size_t i = micros / 1000;
    if (i >= force_.size())
    {
      i = force_.size() - 1;
    }
    Truth t;
    t.specificForce = force_[i];
    t.pressurePa = standardPressure(spec_.padAltitude + height_[i]);
    t.gyroX = burning_[i] ? spec_.pitchRate : 0.0F;
    t.gyroY = 0.0F;
    t.gyroZ = burning_[i] ? 2.0F * spec_.pitchRate : 0.0F;
    return t;
  }

  uint32_t durationMicros() const { return (uint32_t)(spec_.duration * 1e6F); }

private:
  ProfileSpec spec_;
  std::vector<float> force_;
  std::vector<float> height_;
  std::vector<bool> burning_;
};
