void firePyros(char event)
{
  pyroFire = true;
  if (pyro1.func == event)
  {
    pins.firePin = pyro1.firePin;
    digitalWrite(pyro1.firePin, HIGH);
    pyro1.fireStatus = true;
    pyro1.fireStart = fltTime.timeCurrent;
  }
  if (pyro2.func == event)
  {
    pins.firePin = pyro2.firePin;
    digitalWrite(pyro2.firePin, HIGH);
    pyro2.fireStatus = true;
    pyro2.fireStart = fltTime.timeCurrent;
  }
  if (pyro3.func == event)
  {
    pins.firePin = pyro3.firePin;
    digitalWrite(pyro3.firePin, HIGH);
    pyro3.fireStatus = true;
    pyro3.fireStart = fltTime.timeCurrent;
  }
  if (pyro4.func == event)
  {
    pins.firePin = pyro4.firePin;
    digitalWrite(pyro4.firePin, HIGH);
    pyro4.fireStatus = true;
    pyro4.fireStart = fltTime.timeCurrent;
  }
}

void pulsePyro()
{
  static boolean pyro1fire = false;
  static boolean pyro2fire = false;
  static boolean pyro3fire = false;
  static boolean pyro4fire = false;

  if (pyro1.fireStatus)
  {
    if (pyro1fire)
    {
      digitalWrite(pyro1.firePin, LOW);
      pyro1fire = false;
    }
    else
    {
      digitalWrite(pyro1.firePin, HIGH);
      pyro1fire = true;
    }
  }
  if (pyro2.fireStatus)
  {
    if (pyro2fire)
    {
      digitalWrite(pyro2.firePin, LOW);
      pyro2fire = false;
    }
    else
    {
      digitalWrite(pyro2.firePin, HIGH);
      pyro2fire = true;
    }
  }
  if (pyro3.fireStatus)
  {
    if (pyro3fire)
    {
      digitalWrite(pyro3.firePin, LOW);
      pyro3fire = false;
    }
    else
    {
      digitalWrite(pyro3.firePin, HIGH);
      pyro3fire = true;
    }
  }
  if (pyro4.fireStatus)
  {
    if (pyro4fire)
    {
      digitalWrite(pyro4.firePin, LOW);
      pyro4fire = false;
    }
    else
    {
      digitalWrite(pyro4.firePin, HIGH);
      pyro4fire = true;
    }
  }
}
