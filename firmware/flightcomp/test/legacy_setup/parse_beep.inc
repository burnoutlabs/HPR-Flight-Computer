void parseBeep(uint32_t value, uint8_t array[], uint8_t arrayLen)
{
  bool flag = false;
  for (uint8_t i = arrayLen; i > 0; i--)
  {
    array[i - 1] = uint8_t((float)value / powf(10, i - 1));
    value -= array[i - 1] * powf(10, i - 1);
    if (!flag && array[i - 1] > 0)
    {
      flag = true;
    }
    if (flag && array[i - 1] == 0)
    {
      array[i - 1] = 10;
    }
  }
} // end void
