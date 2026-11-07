  // detect liftoff
  if (!events.liftoff && accel.z > gTrigger && !events.touchdown && !events.timeOut)
  {
    if (settings.testMode)
    {
      Serial.println(' ');
      Serial.println(F("Simulated Liftoff Detected!"));
    }
    // set liftoff clock times
    fltTime.padTime = fltTime.tmClockPrev = fltTime.tmClock;
    fltTime.liftoff = fltTime.tmClock;
    // update events
    events.preLiftoff = false;
    events.liftoff = true;
    events.inFlight = true;
    // set radio variables
    radio.event = Liftoff;
    radio.alt = 0;
    radioTimer.end();
    radioTimer.begin(timerBuildPkt, pktInterval.inflight / pktInterval.samplesPerPkt);
    noInterrupts();
    buildPkt = true;
    interrupts();
    // set GNSS liftoff values
    gnss.liftoff.hour = GPS.time.hour();
    gnss.liftoff.minute = GPS.time.minute();
    gnss.liftoff.second = GPS.time.second();
    gnss.liftoff.mili = GPS.time.centisecond();
    gnss.maxAlt = 0.0F;
    // if live pyros are detected, turn on the alarm to alert the user that the system is active and may fire pyros
    if (pyro1.contStatus || pyro2.contStatus || pyro3.contStatus || pyro4.contStatus || settings.testMode)
    {
      livePyroAlarm = true;
      Serial.println("Live-pyro alarm activated");
    }
    // store base alt in EEPROM for inflight recovery
    floatUnion.val = baro.baseAlt;
    if (settings.inflightRecover != 0)
    {
      for (byte i = 0; i < 4; i++)
      {
        EEPROM.update(eeprom.baseAlt + i, floatUnion.Byte[i]);
      }
      EEPROM.update(eeprom.lastEvent, radio.event);
    }
  }

  if (events.liftoff)
  {

    // capture the cycles between samples
    if (settings.testMode)
    {
      cyclesBtwn++;
    }

    // set cycle timestamp
    fltTime.dt = fltTime.tmClock - fltTime.tmClockPrev;
    fltTime.timeCurrent += fltTime.dt;
    fltTime.tmClockPrev = fltTime.tmClock;

    // Update the moving average which greatly improves accuracy with minimal latency
    if (highG.newSamp)
    {
      highGsum -= highGfilter[filterPosn];
      highGfilter[filterPosn] = highG.z;
      highGsum += highGfilter[filterPosn];
      filterPosn++;
      if (filterPosn >= sizeHighGfilter)
      {
        filterPosn = 0;
        if (!filterFull)
        {
          filterFull = true;
        }
      }
      if (!filterFull)
      {
        highGsmooth = highGsum / filterPosn;
      }
      else
      {
        highGsmooth = highGsum / sizeHighGfilter;
      }
    }

    // Compute the current g-load. Use the high-G accelerometer if the IMU is pegged and a high-G accelerometer is present
    if (abs(accel.z) < accel.ADCmax)
    {
      accelNow = (float)(accel.z - g) * accel.gainZ;
    }
    else if (sensors.highG != 0)
    {
      accelNow = (float)(highG.z - high1G) * highG.gainZ;
    }
    else
    {
      accelNow = (float)(accel.z - g) * accel.gainZ;
    }

    // Integrate velocity and altitude data prior to apogee
    if (!events.apogeeFire)
    {

      // Capture the max acceleration
      if (accelNow > maxG)
      {
        maxG = accelNow;
      }

      // calculate the new acceleration based velocity
      // this makes the apogee event mach immune
      accelVel += accelNow * (float)fltTime.dt * mlnth;

      // calculate the new acceleration based altitude
      accelAlt += accelVel * (float)fltTime.dt * mlnth;

      // calculate the new sensor fusion based velocity
      fusionVel += accelNow * (float)fltTime.dt * mlnth;
      if (baro.newSamp && baro.Vel < 300 && baro.Vel < baro.maxVel && accelNow < 0.2F && fusionVel < 300.0F && baro.Alt < 13000)
      {
        fusionVel *= 0.99F;
        fusionVel += 0.01F * baro.Vel;
      }
      radio.vel = (int16_t)fusionVel;

      // update maximum velocity if it exceeds the previous value
      if (fusionVel > maxVelocity)
      {
        maxVelocity = fusionVel;
      }

      // calculate the new sensor fusion based altitude
      fusionAlt += fusionVel * (float)fltTime.dt * mlnth;
      if (baro.newSamp && baro.Alt < 9000)
      {
        fusionAlt *= 0.95;
        fusionAlt += 0.05 * baro.Alt;
      }
      if (!altOK && (fusionAlt > settings.altThreshold || settings.testMode))
      {
        altOK = true;
      }
      radio.alt = (int16_t)fusionAlt;

    } // end if !apogee

    // caluclate the partial rotation in millionths of a degree, maintaining integer math for speed and retaining all possible precision
    dx += gyro.x * fltTime.dt;
    dy += gyro.y * fltTime.dt;
    dz += gyro.z * fltTime.dt;

    // if active stablization is on, then use the more stable rotation algorithm (Rotation.cpp)
    if (settings.stableRotn || settings.stableVert)
    {
      getDCM2DRotn(dx, dy, dz, gyro.gainZ);
      dx = dy = dz = 0L;
    }

    // else update the quaternion rotation if we are not using active stabilization(Rotation.cpp)
    else if (fltTime.timeCurrent - lastRotn > rotnRate)
    {
      getQuatRotn(dx, dy, dz, gyro.gainZ);
      dx = dy = dz = 0L;
      lastRotn = fltTime.timeCurrent;
    }

    // run event logic (Event_Logic.h)
    checkEvents();
