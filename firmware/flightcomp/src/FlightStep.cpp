/**
 * @file FlightStep.cpp
 * @brief Barometer sample processing, which feeds the flight step (FlightStep.h).
 *
 * @details
 * processBaroSamp() turns each new barometer altitude into the pad altitude (before liftoff and
 * after landing), the altitude above the pad, its maximum, the smoothed altitude, the barometric
 * velocity and the touchdown counter. loop() calls it whenever a barometer sample is new, before
 * flightStep(). It is defined here, not in flightcomp.cpp, so the host tests can build it.
 */
#include <Arduino.h>

#include "declarations.h"
#include "FlightStep.h"

void processBaroSamp()
{

  //----------------------------
  // process preliftoff variables
  //----------------------------
  static float sumBaseAlt = 0.0F;
  static uint8_t baseAltPosn = 0;
  static float baseAltBuff[30] = {0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F,
                                  0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F,
                                  0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F};
  if (events.preLiftoff || events.touchdown || events.timeOut)
  {
    sumBaseAlt -= baseAltBuff[baseAltPosn];
    sumBaseAlt += baro.rawAlt;
    baseAltBuff[baseAltPosn] = baro.rawAlt;
    baseAltPosn++;
    const uint8_t sizeBaseAltBuff = sizeof(baseAltBuff) / sizeof(baseAltBuff[0]);
    if (baseAltPosn >= sizeBaseAltBuff)
    {
      baseAltPosn = 0;
    }
    baro.baseAlt = sumBaseAlt / sizeBaseAltBuff;
    radio.baseAlt = (int16_t)baro.baseAlt;
    baro.newSamp = false;
  }

  //--------------------------
  // Update in-flight variables
  //--------------------------
  baro.Alt = baro.rawAlt - baro.baseAlt;
  // update the maximum altitude reporting variable
  if (baro.Alt > baro.maxAlt && !events.apogee)
  {
    baro.maxAlt = baro.Alt;
  }

  //----------------------------
  // Smoothed barometric altitude
  //----------------------------
  static float rawAltSum = 0.0;
  static float rawAltBuff[10] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  static byte rawAltPosn = 0;
  // Update the rolling sum
  rawAltSum -= rawAltBuff[rawAltPosn];
  rawAltBuff[rawAltPosn] = baro.Alt;
  rawAltSum += rawAltBuff[rawAltPosn];
  // update the position counter
  rawAltPosn++;
  static const float sizeRawAltBuff = sizeof(rawAltBuff) / sizeof(rawAltBuff[0]);
  if (rawAltPosn >= (byte)sizeRawAltBuff)
  {
    rawAltPosn = 0;
  }
  // calculate the smoothed barometric altitude
  baro.smoothAlt = rawAltSum / sizeRawAltBuff;

  //---------------------------
  // barometric derived velocity
  //---------------------------
  static uint8_t altAvgPosn = 0;
  static int16_t baroVelPosn = 0;
  static float altAvgBuff[30] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  static unsigned long baroTimeBuff[30] = {0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL,
                                           0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL,
                                           0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL};
  // if we are past apogee, use the maximum difference between samples in the buffer, otherwise set the difference to 10 samples
  if (events.apogee)
  {
    baroVelPosn = altAvgPosn;
  }
  else
  {
    baroVelPosn = altAvgPosn - 10;
    if (baroVelPosn < 0)
    {
      baroVelPosn = (int)((sizeof(altAvgBuff) / sizeof(altAvgBuff[0])) - (10 - altAvgPosn));
    }
  }
  // calculate the barometric derived velocity from the moving averages
  baro.Vel = (baro.smoothAlt - altAvgBuff[baroVelPosn]) / ((float)(baro.timeLastSamp - baroTimeBuff[baroVelPosn]) * mlnth);
  // update variables
  if (baro.Vel > baro.maxVel)
  {
    baro.maxVel = baro.Vel;
  }
  radio.vel = baro.Vel;
  radio.alt = baro.smoothAlt;
  altAvgBuff[altAvgPosn] = baro.smoothAlt;
  baroTimeBuff[altAvgPosn] = baro.timeLastSamp;
  altAvgPosn++;
  if (altAvgPosn >= (byte)(sizeof(altAvgBuff) / sizeof(altAvgBuff[0])))
  {
    altAvgPosn = 0;
  }
  if (events.apogeeFire || settings.testMode)
  {
    fusionVel = baro.Vel;
    fusionAlt = baro.smoothAlt;
  }

  //----------------------------
  // Barometric touchdown trigger
  //----------------------------
  (events.mainDeploy && fabs(baro.Vel) < 1.0F) ? baroTouchdown++ : baroTouchdown = 0;
}

void pushFlightAction(FlightActions &actions, FlightAction kind, uint8_t arg)
{
  if (actions.count < FLIGHT_ACTIONS_MAX)
  {
    actions.kind[actions.count] = kind;
    actions.arg[actions.count] = arg;
    actions.count++;
  }
}

void firePyros(char event, FlightActions &actions)
{
  pyroFire = true;
  if (pyro1.func == event)
  {
    pins.firePin = pyro1.firePin;
    pushFlightAction(actions, FlightAction::PinHigh, pyro1.firePin);
    pyro1.fireStatus = true;
    pyro1.fireStart = fltTime.timeCurrent;
  }
  if (pyro2.func == event)
  {
    pins.firePin = pyro2.firePin;
    pushFlightAction(actions, FlightAction::PinHigh, pyro2.firePin);
    pyro2.fireStatus = true;
    pyro2.fireStart = fltTime.timeCurrent;
  }
  if (pyro3.func == event)
  {
    pins.firePin = pyro3.firePin;
    pushFlightAction(actions, FlightAction::PinHigh, pyro3.firePin);
    pyro3.fireStatus = true;
    pyro3.fireStart = fltTime.timeCurrent;
  }
  if (pyro4.func == event)
  {
    pins.firePin = pyro4.firePin;
    pushFlightAction(actions, FlightAction::PinHigh, pyro4.firePin);
    pyro4.fireStatus = true;
    pyro4.fireStart = fltTime.timeCurrent;
  }
}

void pulsePyro(FlightActions &actions)
{
  static boolean pyro1fire = false;
  static boolean pyro2fire = false;
  static boolean pyro3fire = false;
  static boolean pyro4fire = false;

  if (pyro1.fireStatus)
  {
    if (pyro1fire)
    {
      pushFlightAction(actions, FlightAction::PinLow, pyro1.firePin);
      pyro1fire = false;
    }
    else
    {
      pushFlightAction(actions, FlightAction::PinHigh, pyro1.firePin);
      pyro1fire = true;
    }
  }
  if (pyro2.fireStatus)
  {
    if (pyro2fire)
    {
      pushFlightAction(actions, FlightAction::PinLow, pyro2.firePin);
      pyro2fire = false;
    }
    else
    {
      pushFlightAction(actions, FlightAction::PinHigh, pyro2.firePin);
      pyro2fire = true;
    }
  }
  if (pyro3.fireStatus)
  {
    if (pyro3fire)
    {
      pushFlightAction(actions, FlightAction::PinLow, pyro3.firePin);
      pyro3fire = false;
    }
    else
    {
      pushFlightAction(actions, FlightAction::PinHigh, pyro3.firePin);
      pyro3fire = true;
    }
  }
  if (pyro4.fireStatus)
  {
    if (pyro4fire)
    {
      pushFlightAction(actions, FlightAction::PinLow, pyro4.firePin);
      pyro4fire = false;
    }
    else
    {
      pushFlightAction(actions, FlightAction::PinHigh, pyro4.firePin);
      pyro4fire = true;
    }
  }
}
