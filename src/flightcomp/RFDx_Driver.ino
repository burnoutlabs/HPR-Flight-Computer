/**
 * @file RFDx_Driver.ino
 * @brief Driver for the RFD900 serial telemetry radio.
 *
 * @details
 * The radio is driven over the Serial2 UART at 57600 baud: beginRFD900() opens the port and
 * sendPktRFD900() writes each packet followed by a line ending. Frequency and power are set on
 * the radio itself, so setPolyMorphs() points those function pointers at dummy functions.
 */

#define radioPort Serial2
#define RADIO_UART_SPEED 57600

bool beginRFD900()
{

  boolean successFlag = true;

  radioPort.begin(RADIO_UART_SPEED);

  if (successFlag)
  {
    Serial.println("RFD900 Radio OK!");
  }

  return successFlag;
}

bool sendPktRFD900(uint8_t *data, uint8_t len)
{
  for (int i = 0; i < len; i++)
  {
    radioPort.write(data[i]);
    radioPort.flush();
  }
  radioPort.println();
  radioPort.flush();
  // set flag
  radioFnctn = TXmode;
  TX = true;
  return true;
}