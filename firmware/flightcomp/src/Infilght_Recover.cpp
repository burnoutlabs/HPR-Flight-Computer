/**
 * @file Infilght_Recover.cpp
 * @brief Implements a recovery system for an altimeter with detailed reset functionality in case of accidental power loss or system reset.
 */

#include <Arduino.h>
#include <EEPROM.h>

#include "Infilght_Recover.h"
#include "declarations.h"
#include "flightcomp.h"
#include "FlightStep.h"
#include "Bus_Mgmt.h"
#include "Calibration.h"
#include "SD_Card.h"
#include "Speed_Trig.h"
#include "Startup.h"

/**
 * @brief Routine to restart the system in-flight in case of accidental power loss or system reset.
 *
 * The steps for the rapidReset() routine are as follows:
 * 1. Read the settings stored in EEPROM.
 * 2. Restart the sensors.
 * 3. Find the last SD file and resume where we left off.
 * 4. Sample the sensors for up to 8 seconds.
 * 5. If conditions for restart are met, set appropriate flags and continue with the flight.
 *
 * @return True if conditions for restart are met and system is successfully reset; otherwise, false.
 */
bool rapidReset()
{

  // Read the hardware settings from EEPROM, as setup() does
  readHardwareSettingsEEPROM();
  // unlike setup(), the accelerometer bus number is read from the bus type's address (#28)
  sensors.accelBusNum = EEPROM.read(eeprom.accelBusType);

  // Set the mode of the output pins
  pinMode(pins.nullCont, INPUT);
  pinMode(pins.nullFire, INPUT);
  pinMode(pins.pyro1Cont, INPUT);
  pinMode(pins.pyro2Cont, INPUT);
  pinMode(pins.pyro3Cont, INPUT);
  pinMode(pins.pyro4Cont, INPUT);
  pinMode(pins.pyro1Fire, OUTPUT);
  pinMode(pins.pyro2Fire, OUTPUT);
  pinMode(pins.pyro3Fire, OUTPUT);
  pinMode(pins.pyro4Fire, OUTPUT);
  pinMode(pins.beep, OUTPUT);
  pinMode(pins.testRead, INPUT_PULLUP);
  pinMode(pins.testGnd, OUTPUT);
  pinMode(pins.radioCS, OUTPUT);
  // Set the pyro firing pins to LOW for safety
  digitalWrite(pins.pyro1Fire, LOW);
  digitalWrite(pins.pyro2Fire, LOW);
  digitalWrite(pins.pyro3Fire, LOW);
  digitalWrite(pins.pyro4Fire, LOW);
  digitalWrite(pins.radioCS, HIGH);
  digitalWrite(pins.accelCS, HIGH);
  digitalWrite(pins.gyroCS, HIGH);
  digitalWrite(pins.magCS, HIGH);
  digitalWrite(pins.highG_CS, HIGH);
  digitalWrite(pins.baroCS, HIGH);
  digitalWrite(pins.SD_CS, HIGH);
  Serial.println("Set Pins Low");
  // Set the pyro firing pins to LOW for safety
  digitalWrite(pins.pyro1Fire, LOW);
  digitalWrite(pins.pyro2Fire, LOW);
  digitalWrite(pins.pyro3Fire, LOW);
  digitalWrite(pins.pyro4Fire, LOW);

  // setup the ADC for sampling the battery
  analogReadResolution(16);

  // read user settings stored in EEPROM
  readEEPROMsettings();

  // Start Sensors
  restartSD();
  beginAccel();
  beginGyro();
  beginHighG();
  beginBaro();

  // restart hardware serial
  setHWSERIAL();
  if (sensors.GNSS == 3)
  {
    HWSERIAL->begin(38400);
  }
  else
  {
    HWSERIAL->begin(9600);
  }

  // set the g-trigger
  gTrigger = 2.5 * g;

  // radio setup
  if (settings.TXenable && settings.inflightRecover > 0)
  {
    beginRadio();
    // Set the radio output power & frequency
    setRadioPWR(settings.TXpwr); // 23 max setting; 20mW=13dBm, 30mW=15dBm, 50mW=17dBm, 100mW=20dBm
    setRadioFreq(settings.TXfreq);
  }

  // setup the ADC for sampling the battery
  analogReadResolution(16);

  // configure pyro outupts
  configurePyroChannels();

  // setup the radio
  if (!settings.TXenable)
  {
    if (pins.radioEN != pins.nullCont)
    {
      pinMode(pins.radioEN, OUTPUT);
      digitalWrite(pins.radioEN, LOW);
    }
    if (settings.testMode)
    {
      Serial.println(F("Telemetry OFF!"));
    }
  }
  else
  {
    // Set the radio output power & frequency
    setRadioPWR(settings.TXpwr); // 23 max setting; 20mW=13dBm, 30mW=15dBm, 50mW=17dBm, 100mW=20dBm
    setRadioFreq(settings.TXfreq);
    if (settings.testMode)
    {
      Serial.print("Radio Freq: ");
      Serial.println(settings.TXfreq, 3);
      Serial.print("Radio Power: ");
      Serial.println(settings.TXpwr);
    }
  }

  // read the bias from EEPROM
  calUnion.calByte[0] = EEPROM.read(eeprom.accelBiasX);
  calUnion.calByte[1] = EEPROM.read(eeprom.accelBiasX + 1);
  accel.biasX = calUnion.calValue;
  calUnion.calByte[0] = EEPROM.read(eeprom.accelBiasY);
  calUnion.calByte[1] = EEPROM.read(eeprom.accelBiasY + 1);
  accel.biasY = calUnion.calValue;
  calUnion.calByte[0] = EEPROM.read(eeprom.accelBiasZ);
  calUnion.calByte[1] = EEPROM.read(eeprom.accelBiasZ + 1);
  accel.biasZ = calUnion.calValue;
  calUnion.calByte[0] = EEPROM.read(eeprom.highGbiasX);
  calUnion.calByte[1] = EEPROM.read(eeprom.highGbiasX + 1);
  highG.biasX = calUnion.calValue;
  calUnion.calByte[0] = EEPROM.read(eeprom.highGbiasY);
  calUnion.calByte[1] = EEPROM.read(eeprom.highGbiasY + 1);
  highG.biasY = calUnion.calValue;
  calUnion.calByte[0] = EEPROM.read(eeprom.highGbiasZ);
  calUnion.calByte[1] = EEPROM.read(eeprom.highGbiasZ + 1);
  highG.biasZ = calUnion.calValue;
  calUnion.calByte[0] = EEPROM.read(eeprom.magBiasX);
  calUnion.calByte[1] = EEPROM.read(eeprom.magBiasX + 1);
  mag.biasX = calUnion.calValue;
  calUnion.calByte[0] = EEPROM.read(eeprom.magBiasY);
  calUnion.calByte[1] = EEPROM.read(eeprom.magBiasY + 1);
  mag.biasY = calUnion.calValue;
  calUnion.calByte[0] = EEPROM.read(eeprom.magBiasZ);
  calUnion.calByte[1] = EEPROM.read(eeprom.magBiasZ + 1);
  mag.biasZ = calUnion.calValue;

  // read the orientation variables from EEPROM
  readOrientation();

  // Find the last file and continue writing
  reOpenSD();

  // read the base altitude
  for (byte i = 0; i < 4; i++)
  {
    floatUnion.Byte[i] = (byte)EEPROM.read(eeprom.baseAlt + i);
  }
  baro.baseAlt = floatUnion.val;

  // sample sensors and alert user that the system is recovering
  uint32_t sampleStart = micros();

  // set flight type to Single Stage, no attempted recovery of complex flight types (2-stage or airstart)
  settings.fltProfile = 'S';

  // check continuity
  pyro1.contStatus = ((digitalRead(pyro1.contPin) == HIGH) ? true : false);
  pyro2.contStatus = ((digitalRead(pyro2.contPin) == HIGH) ? true : false);
  pyro3.contStatus = ((digitalRead(pyro3.contPin) == HIGH) ? true : false);
  pyro4.contStatus = ((digitalRead(pyro4.contPin) == HIGH) ? true : false);

  if (pyro1.contStatus)
  {
    if (pyro1.func == 'M')
    {
      cont.main = true;
    }
    if (pyro1.func == 'A')
    {
      cont.apogee = true;
    }
    if (pyro1.func == 'I')
    {
      cont.upperStage = true;
    }
    if (pyro1.func == 'B')
    {
      cont.boosterSep = true;
    }
    if (pyro1.func == '1')
    {
      cont.airStart1 = true;
    }
    if (pyro1.func == '2')
    {
      cont.airStart2 = true;
    }
    if (pyro1.func == 'N')
    {
      cont.noFunc = true;
    }
  }
  if (pyro2.contStatus)
  {
    if (pyro2.func == 'M')
    {
      cont.main = true;
    }
    if (pyro2.func == 'A')
    {
      cont.apogee = true;
    }
    if (pyro2.func == 'I')
    {
      cont.upperStage = true;
    }
    if (pyro2.func == 'B')
    {
      cont.boosterSep = true;
    }
    if (pyro2.func == '1')
    {
      cont.airStart1 = true;
    }
    if (pyro2.func == '2')
    {
      cont.airStart2 = true;
    }
    if (pyro2.func == 'N')
    {
      cont.noFunc = true;
    }
  }
  if (pyro3.contStatus)
  {
    if (pyro3.func == 'M')
    {
      cont.main = true;
    }
    if (pyro3.func == 'A')
    {
      cont.apogee = true;
    }
    if (pyro3.func == 'I')
    {
      cont.upperStage = true;
    }
    if (pyro3.func == 'B')
    {
      cont.boosterSep = true;
    }
    if (pyro3.func == '1')
    {
      cont.airStart1 = true;
    }
    if (pyro3.func == '2')
    {
      cont.airStart2 = true;
    }
    if (pyro3.func == 'N')
    {
      cont.noFunc = true;
    }
  }
  if (pyro4.contStatus)
  {
    if (pyro4.func == 'M')
    {
      cont.main = true;
    }
    if (pyro4.func == 'A')
    {
      cont.apogee = true;
    }
    if (pyro4.func == 'I')
    {
      cont.upperStage = true;
    }
    if (pyro4.func == 'B')
    {
      cont.boosterSep = true;
    }
    if (pyro4.func == '1')
    {
      cont.airStart1 = true;
    }
    if (pyro4.func == '2')
    {
      cont.airStart2 = true;
    }
    if (pyro4.func == 'N')
    {
      cont.noFunc = true;
    }
  }

  // Look for continuity problems
  if (!pyro1.contStatus && pyro1.func != 'N')
  {
    cont.error = true;
  }
  if (!pyro2.contStatus && pyro2.func != 'N')
  {
    cont.error = true;
  }
  if (!pyro3.contStatus && pyro3.func != 'N')
  {
    cont.error = true;
  }
  if (!pyro4.contStatus && pyro4.func != 'N')
  {
    cont.error = true;
  }

  // variables to assess before we enter the sample loop
  // uint32_t monoAltUp = sampleStart;//timestamp of first increasing altitude
  uint32_t monoAltDwn = sampleStart;  // timestamp of first decreasing altitude
  bool altRange10 = false;            // altitude observed range within 10ft
  uint32_t allAccelNeg = sampleStart; // timestamp of first negative acceleration
  bool altAbove100m = true;
  uint32_t altBelow100m = sampleStart; // timestamp of last altitude sample above 300m
  float minAltSamp = 0.0F;
  float maxAltSamp = 0.0F;
  float altSamp[300];
  uint8_t currentSamp = 0;
  // uint8_t lastSamp = 0;
  bool cone20deg = true;
  float lastAlt = 0;
  float maxRotnSpeed = 0.0F;
  bool altDelta50 = false;
  bool altDelta100 = false;
  bool accel1G = false;
  bool bufferFull = false;
  int maxAccel = -32000;
  int minAccel = 32000;
  uint32_t axialAccel = sampleStart; // timestamp of last axial acceleration above 0.25G

  // disable pyros unless user desired
  if (settings.inflightRecover != 2)
  {
    settings.pyro1Func = 'N';
    settings.pyro2Func = 'N';
    settings.pyro3Func = 'N';
    settings.pyro4Func = 'N';
  }

  // set the alttitude samples to zero so the compiler doesnt get confused
  for (uint16_t i = 0; i < sizeof(altSamp) / sizeof(altSamp[1]); i++)
  {
    altSamp[i] = 0.0;
  }

  // start 8 seconds of sampling
  while (micros() - sampleStart > 8000000UL)
  {

    //---------------------------
    // sound alarm
    //---------------------------
    if (!beep && micros() - timeLastBeepEnd > alarmBeepDelay)
    {
      digitalWrite(pins.beep, HIGH);
      beep = true;
      timeBeepStart = micros();
    }
    if (beep && micros() - timeBeepStart > alarmBeepLen)
    {
      digitalWrite(pins.beep, LOW);
      beep = false;
      timeLastBeepEnd = micros();
    }

    //---------------------------
    // sample sensors
    //---------------------------
    getAccel();
    getGyro();

    // Get a barometric event if needed
    if (micros() - baro.timeLastSamp >= baro.timeBtwnSamp)
    {
      getBaro();
      baro.timeLastSamp = micros();
      baro.newSamp = true;
    }

    //-------------------------------------------------------
    // assess flight conditions
    //-------------------------------------------------------
    if (baro.newSamp)
    {

      // process the sample
      processBaroSamp();

      // check to see of there is a 50ft change within the past 1 second
      altDelta50 = false;
      if (currentSamp > 32 && fabs(altSamp[currentSamp] - altSamp[currentSamp - 32]) > 15.25F)
      {
        altDelta50 = true;
      }

      // check to see of there is a 100ft change within the past 3 seconds
      altDelta100 = false;
      if (currentSamp > 96 && fabs(altSamp[currentSamp] - altSamp[currentSamp - 96]) > -30.0F)
      {
        altDelta100 = true;
      }

      // monotonically increasing altitude
      // if(baro.Alt + 5 < lastAlt){monoAltUp = micros();}

      // monotonically decreasing altitude
      if (baro.Alt - 5 > lastAlt)
      {
        monoAltDwn = micros();
      }

      // all altitude samples above 100m
      if (baro.Alt < 100 and baro.Alt != 0.0F)
      {
        altAbove100m = false;
      }

      // timed altitude samples below 100m
      if (baro.Alt > 100)
      {
        altBelow100m = micros();
      }

      // see if all the altitude samples are within a range of 10ft for past 5 seconds
      altRange10 = false;
      if (currentSamp > 160)
      {
        maxAltSamp = minAltSamp = altSamp[currentSamp];
        for (byte i = 1; i < 160; i++)
        {
          if (altSamp[currentSamp - i] > maxAltSamp)
          {
            maxAltSamp = altSamp[currentSamp - i];
          }
          if (altSamp[currentSamp - i] < minAltSamp)
          {
            minAltSamp = altSamp[currentSamp - i];
          }
        }
        if (maxAltSamp - minAltSamp < 3.05)
        {
          altRange10 = true;
        }
      }

      currentSamp++;
      if (currentSamp >= sizeof(altSamp) / sizeof(altSamp[0]))
      {
        currentSamp = 0;
      }
    }

    //-------------------------------------------------------
    // assess accelerometer and gyro values
    //-------------------------------------------------------

    // capture the maxiumum observed rotation rate
    if (abs(gyro.z) * gyro.gainZ > maxRotnSpeed)
    {
      maxRotnSpeed = abs(gyro.z) * gyro.gainZ;
    }
    if (abs(gyro.y) * gyro.gainY > maxRotnSpeed)
    {
      maxRotnSpeed = abs(gyro.y) * gyro.gainY;
    }
    if (abs(gyro.x) * gyro.gainX > maxRotnSpeed)
    {
      maxRotnSpeed = abs(gyro.x) * gyro.gainX;
    }

    // all samples at 1G acceleration +/- 0.05G
    float sampG = powf(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z, 0.5);
    if (accel1G && (sampG * accel.gainZ > 1.05F || sampG * accel.gainZ < 1.05F))
    {
      accel1G = false;
    }

    // timed acceleration samples negative
    if (accel.z > 0)
    {
      allAccelNeg = micros();
    }

    // acceleration ranges
    if (sampG * accel.gainZ > maxAccel)
    {
      maxAccel = sampG * accel.gainZ;
    }
    if (sampG * accel.gainZ < minAccel)
    {
      minAccel = sampG * accel.gainZ;
    }

    // axial acceleration
    if (abs(accel.x) * accel.gainX > 0.25 || abs(accel.y) * accel.gainY > 0.25)
    {
      axialAccel = micros();
    }

    // Compute the acceleromter based rotation angle
    if (cone20deg)
    {
      if (accel.y >= 0)
      {
        yawY0 = speedArcSin(min(1, (float)accel.y / (float)g));
      }
      else
      {
        yawY0 = speedArcSin(max(-1, (float)accel.y / (float)g));
      }

      if (accel.x >= 0)
      {
        pitchX0 = asin(min(1, (float)accel.x / (float)g));
      }
      else
      {
        pitchX0 = speedArcSin(max(-1, (float)accel.x / (float)g));
      }

      if (pitchX0 > 200 || yawY0 > 200)
      {
        cone20deg = false;
      }
    }

    //------------------------------------------------------
    // determine flight phase
    //------------------------------------------------------

    // PRE-FLIGHT ON THE PAD:
    // continuity on all charges
    // all samples postive acceleration at 1G +/-0.05G for 5s
    // all acceleration samples oriented vertical within a 20 degree cone for 5s
    // all barometric altitude samples within 10ft for 5 seconds
    // maxiumum rotation speed of 10 degrees / sec
    if (!cont.error && altRange10 && accel1G && cone20deg && baro.Alt < 3 && maxRotnSpeed < 10.0F && micros() - sampleStart < 3000000 && bufferFull && fabs(baro.Vel) < 5)
    {
      digitalWrite(pins.beep, LOW);
      return false;
    }

    // SUBSONIC COAST:
    // monotonically increasing barometric altitude for at least 1 second
    // range difference is greater than 50ft for 1 second
    // all altitude samples greater than 300ft above base alt
    // all vertical-axis accelerometer values are all negative
    if (baro.Vel > 10.0F && altDelta50 && micros() - allAccelNeg > 1000000)
    {
      events.preLiftoff = false;
      events.inFlight = true;
      events.liftoff = true;
      events.falseLiftoffCheck = false;
      digitalWrite(pins.beep, LOW);
      return true;
    }

    // NEAR APOGEE:
    // all accelerations < 0.25G
    // baro velocity < 10 m/s
    if (fabs(baro.Vel) < 10.0F && bufferFull && sampG && accel.gainZ < 0.25 && !altDelta50 && abs(maxAccel - minAccel) < 0.25 && maxAccel < 0.5 and minAccel > 0.5)
    {
      events.preLiftoff = false;
      events.inFlight = true;
      events.liftoff = true;
      events.boosterBurnout = true;
      events.falseLiftoffCheck = false;
      digitalWrite(pins.beep, LOW);
      return true;
    }

    // INFLIGHT DESCENDING BALLISTIC:
    // monotonically decreasing altitude for 1 seconds
    // range difference greater than 50ft for 1 second
    // all altitude samples greater than 300ft
    // vertical-axis accelerometer samples are all negative for 1 second
    // negative vertical velocity
    // quiet horizontal axes
    if (micros() - monoAltDwn > 1000000UL && baro.Vel < -10.0F && altDelta50 && altAbove100m &&
        micros() - allAccelNeg > 1000000UL && micros() - axialAccel > 1000000)
    {
      events.preLiftoff = false;
      events.inFlight = true;
      events.liftoff = true;
      events.boosterBurnout = true;
      events.falseLiftoffCheck = false;
      digitalWrite(pins.beep, LOW);
      return true;
    }

    // INFLIGHT DESCENDING UNDER DROGUE
    // all altitude samples greater than 300ft above base alt (check)
    // range difference of 100 feet over 3 seconds
    if (altDelta100 && altAbove100m)
    {
      events.preLiftoff = false;
      events.inFlight = true;
      events.liftoff = true;
      events.boosterBurnout = true;
      events.falseLiftoffCheck = false;
      events.apogee = true;
      digitalWrite(pins.beep, LOW);
      return true;
    }

    // POST-FLIGHT ON THE GROUND (default):
    // all barometric samples within 300ft of base altitude for 5 seconds (check)
    // OR
    // stable barometric altitude within a 10ft range for 5 seconds (check)
    // accelerometer values not 1G or not within 20 degree vertical cone (check)
    if (altBelow100m || (altRange10 && !cone20deg))
    {
      events.preLiftoff = false;
      events.inFlight = false;
      events.liftoff = true;
      events.boosterBurnout = true;
      events.falseLiftoffCheck = false;
      events.apogee = true;
      events.apogeeSeparation = true;
      events.mainDeploy = true;
      events.touchdown = true;
      digitalWrite(pins.beep, LOW);
      return true;
    }
  } // end sample start
  return false;
} // end rapidReset