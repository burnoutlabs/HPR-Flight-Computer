void checkPyroContinuity()
{

  // check continuity
  pyro1.contStatus = ((digitalRead(pyro1.contPin) == HIGH) ? true : false);
  pyro2.contStatus = ((digitalRead(pyro2.contPin) == HIGH) ? true : false);
  pyro3.contStatus = ((digitalRead(pyro3.contPin) == HIGH) ? true : false);
  pyro4.contStatus = ((digitalRead(pyro4.contPin) == HIGH) ? true : false);

  if (!events.liftoff)
  {
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

    // if the flight profile is complex, but there is no continuity on BOTH of the extra pyros, then reset to a single stage flight
    if ((settings.fltProfile == '2' || settings.fltProfile == 'A') && !cont.upperStage && !cont.boosterSep && !cont.airStart1 && !cont.airStart2)
    {
      if (settings.testMode)
      {
        Serial.println(F("Complex Pyros Not Detected! Flight Profile set to single stage"));
      }
      settings.fltProfile = 'S';
      if (pyro1.func == 'I' || pyro1.func == 'B' || pyro1.func == '1' || pyro1.func == '2')
      {
        pyro1.func = 'N';
      }
      if (pyro2.func == 'I' || pyro2.func == 'B' || pyro2.func == '1' || pyro2.func == '2')
      {
        pyro2.func = 'N';
      }
      if (pyro3.func == 'I' || pyro3.func == 'B' || pyro3.func == '1' || pyro3.func == '2')
      {
        pyro3.func = 'N';
      }
      if (pyro4.func == 'I' || pyro4.func == 'B' || pyro4.func == '1' || pyro4.func == '2')
      {
        pyro4.func = 'N';
      }
    }

    // Look for continuity problems
    if (!pyro1.contStatus && pyro1.func != 'N')
    {
      cont.error = true;
      cont.reportCode = cont.beepCode = 1;
    }
    if (!pyro2.contStatus && pyro2.func != 'N')
    {
      cont.error = true;
      cont.reportCode = cont.beepCode = 2;
    }
    if (!pyro3.contStatus && pyro3.func != 'N')
    {
      cont.error = true;
      cont.reportCode = cont.beepCode = 3;
    }
    if (!pyro4.contStatus && pyro4.func != 'N')
    {
      cont.error = true;
      cont.reportCode = cont.beepCode = 4;
    }
    if (!pyro1.contStatus && !pyro2.contStatus && !pyro3.contStatus && !pyro4.contStatus)
    {
      cont.reportCode = 0;
      cont.beepCode = 1;
    }

    // Report single-stage pre-flight status
    if (!cont.error && (settings.fltProfile == 'S' || settings.fltProfile == 'B'))
    {
      if (cont.main && cont.apogee)
      {
        cont.beepCode = 3;
        cont.reportCode = 9;
      }
      else if (cont.main)
      {
        cont.beepCode = 2;
        cont.reportCode = 8;
      }
      else if (cont.apogee)
      {
        cont.beepCode = 1;
        cont.reportCode = 7;
      }
      postFlightCode = 1;
    }

    // Report two-stage pre-flight status
    if (!cont.error && settings.fltProfile == '2')
    {
      if (cont.boosterSep && cont.upperStage && cont.apogee && cont.main)
      {
        cont.beepCode = 4;
        cont.reportCode = 6;
      }
      else if (cont.upperStage && cont.apogee && cont.main)
      {
        cont.beepCode = 3;
        cont.reportCode = 5;
      }
    }

    // Report airstart pre-flight status
    if (!cont.error && settings.fltProfile == 'A')
    {
      if (cont.airStart1 && cont.airStart2 && cont.apogee && cont.main)
      {
        cont.beepCode = 4;
        cont.reportCode = 6;
      }
      else if (cont.airStart1 && cont.apogee && cont.main)
      {
        cont.beepCode = 3;
        cont.reportCode = 5;
      }
    }

    // Change if Marsa reporting style is desired
    if (settings.reportStyle == 'M')
    {
      cont.beepCode = 2;
      if (cont.error)
      {
        cont.beepCode = 1;
      }
    }
  } // end if liftoff
} // end check continuity
