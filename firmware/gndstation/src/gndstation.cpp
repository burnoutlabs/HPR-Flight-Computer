/**
 * @brief File: gndstation.ino
 *
 * This file contains the setup and loop functions for the rocket's control system.
 *
 * Versions:
 * - V7 uses only a single microprocessor
 * - V8 uses an optimized data stream
 * - V9 overhauls the whole system
 * - V10 eliminates the RadioHead library
 * - V10_1 finalizes functionality to use two radios in tandem
 * - V10_2 fixes some bugs and adds a distance calculation to the LCD screen
 *
 * TO DO:
 * 0) Read user data from SD card
 * 1) Create BLE Service
 * 2) Pin correction for LCD - OK
 * 3) Radio 1 controls - OK
 * 4) Radio 2 controls - OK
 * 5) GPS Services - OK
 * 6) LSM9DS1 orientation
 * 7) LSM9DS1 magnetic
 * 8) Include Software SPI - OK
 * 9) Create user menu interface
 * 10) Delineate FHSS with a booster
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <Wire.h>

#include "declarations.h"
#include "gndstation.h"
#include "GPS.h"
#include "LCD.h"
#include "Parse_Packet.h"
#include "Radio.h"
#include "SD_Card.h"

// Hardware Serial for GPS
HardwareSerial *HWSERIAL;

// Radio control pins
#define radio1RST 14 // ok
#define radio1CS 15  // ok
#define radio1IRQ 17 // ok
#define radio2RST 16 // ok
#define radio2CS 20  // ok
#define radio2IRQ 22 // ok

// GPS Setup
TinyGPSPlus GPS;

// Timer for the radio
IntervalTimer radioTimer;

void setup()
{

  // start SPI
  SPI.begin();

  // Primary Serial Port
  Serial.begin(9600);
  Serial2.begin(9600);

  // Read the battery voltage
  pinMode(A0, INPUT);
  battVolt = 0;
  for (byte i = 0; i < 5; i++)
  {
    battVolt += analogRead(A10);
    delay(10);
  }
  battVolt /= 5;

  // Set interrupts
  pinMode(radio1IRQ, INPUT);
  attachInterrupt(digitalPinToInterrupt(radio1IRQ), clearIRQ1, RISING);
  pinMode(radio2IRQ, INPUT);
  attachInterrupt(digitalPinToInterrupt(radio2IRQ), clearIRQ2, RISING);

  // Start SD card
  SDinit = SD.begin(SD_CONFIG);
  if (!SDinit)
  {
    Serial.println(F("SD card failed"));
  }
  else
  {
    Serial.println(F("SD Card OK!"));
  }
  // Read user settings
  readRadioSettingsSD();

  // set the radio pointers
  radio1.num = 1;
  radio2.num = 2;
  radio1.cs = radio1CS;
  radio2.cs = radio2CS;
  radio1.rst = radio1RST;
  radio2.rst = radio2RST;

  // set radio pins
  pinMode(radio1.cs, OUTPUT);
  pinMode(radio1.rst, OUTPUT);
  pinMode(radio2.cs, OUTPUT);
  pinMode(radio2.rst, OUTPUT);
  digitalWrite(radio1.cs, HIGH);
  digitalWrite(radio1.rst, HIGH);
  digitalWrite(radio2.cs, HIGH);
  digitalWrite(radio2.rst, HIGH);

  // Radio1 setup
  delay(500);
  activeRadio = &radio1;
  activeRadio->status = radioBegin();
  if (!activeRadio->status)
  {
    Serial.print("Radio ");
    Serial.print(activeRadio->num);
    Serial.println(" Failed");
  }
  else
  {
    Serial.print("Radio");
    Serial.print(activeRadio->num);
    Serial.println(" OK!");
  }
  // if the radio is active
  if (activeRadio->status)
  {
    // set frequency
    if (setRadioFreq(activeRadio->frq))
    {
      Serial.print("Radio");
      Serial.print(activeRadio->num);
      Serial.print(" on ");
      Serial.print(activeRadio->frq, 3);
      Serial.println(" MHz");
    }
    else
    {
      Serial.print("Radio");
      Serial.print(activeRadio->num);
      Serial.println(" Set Frequency Failed");
    }
    // set to recieve mode
    radioSetMode(RXmode);
  }

  // Radio2 setup
  delay(500);
  activeRadio = &radio2;
  activeRadio->status = radioBegin();
  if (!activeRadio->status)
  {
    Serial.print("Radio ");
    Serial.print(activeRadio->num);
    Serial.println(" Failed");
  }
  else
  {
    Serial.print("Radio");
    Serial.print(activeRadio->num);
    Serial.println(" OK!");
  }
  // if the radio is active
  if (activeRadio->status)
  {
    // set frequency
    if (setRadioFreq(activeRadio->frq))
    {
      Serial.print("Radio");
      Serial.print(activeRadio->num);
      Serial.print(" on ");
      Serial.print(activeRadio->frq, 3);
      Serial.println(" MHz");
    }
    else
    {
      Serial.print("Radio");
      Serial.print(activeRadio->num);
      Serial.println(" Set Frequency Failed");
    }
    // set to recieve mode
    radioSetMode(RXmode);
  }

  // set active radio based on user settings
  if (radio1.enable & !radio2.enable)
  {
    activeRadio = &radio1;
  }
  else if (!radio1.enable & radio2.enable)
  {
    activeRadio = &radio2;
  }
  else
  {
    activeRadio = &radio1;
  }

  /* EEPROM Allocations:
   Posn 00-19: Sustainer Name
   Posn 20-23: Latitude Float
   Posn 24-27: Longitude Float
   Posn    28: Radio1 Freq
   Posn 29-48: Booster Name
   Posn 49-52: Latitude Float
   Posn 53-56: Longitude Float
   Posn    57: Radio2 Freq
   */

  // read EEPROM for the rocket name
  for (byte i = 0; i < sizeof(rocketName); i++)
  {
    rocketName[i] = EEPROM.read(i);
  }
  // Read EEPROM for the sustainer last GPS coordinates
  for (byte i = 0; i < 4; i++)
  {
    radioUnion.unionByte[i] = EEPROM.read(20 + i);
  }
  lastGPSlat = radioUnion.GPScoord;
  if (fabs(lastGPSlat) > 90)
  {
    lastGPSlat = 0.0;
  }
  for (byte i = 0; i < 4; i++)
  {
    radioUnion.unionByte[i] = EEPROM.read(24 + i);
  }
  lastGPSlon = radioUnion.GPScoord;
  if (fabs(lastGPSlon) > 180)
  {
    lastGPSlon = 0.0;
  }

  // configure GPS
  configGPS();

  // Display the startup screen
  Serial.println("starting LCD");
  startupLCD();

} // end setup

void loop()
{

  boolean msgRX = false;

  // Serial Print Debug
  if (micros() - timer > 1000000UL && micros() - timeLastNMEA > 500000)
  {
    timer = micros();
    Serial.print(F("Waiting for Packet, frequency "));
    Serial.print(readRadioFreq(), 3);
    Serial.println(" MHz");
  }

  // check radios for recieved packet with interrupt
  if (radio1Pkt || radio2Pkt)
  {

    // select the radio that recieved a packet, if both do radio1 first
    if (radio1Pkt)
    {
      activeRadio = &radio1;
    }
    else
    {
      activeRadio = &radio2;
    }

    // set the timers
    lastRX = micros();
    activeRadio->lastRX = lastRX;
    timer = lastRX;

    // get the packet from the radio
    bool pktRX = radioRecvPkt(activeRadio->dataPacket);

    // get signal strength
    signalStrength = pktRssi;

    // if it is from our flight computer, parse it, log it, display it and follow the frequency hops
    if (pktRX)
    {
      processPacket();
    }

    if (activeRadio->num == 1)
    {
      radio1Pkt = false;
    }
    else
    {
      radio2Pkt = false;
    }
  } // end check radios

  // On FHSS, if signal was established on the ground but a packet was missed, then move to the next channel
  if (event == 0 && radio1.FHSS && micros() - radio1.lastHopTime > 2000000UL && radio1.signalEst && radio1.chnlUsed < 3)
  {
    Serial.println("-------Radio 1 Missed Packet--------");
    activeRadio = &radio1;
    // signal to move to the synch freq because the necessary lead time on the ground is too long to try and catch the next packet
    activeRadio->chnlUsed = 3;
    hopFreq();
  }

  // On FHSS, if signal was established in-flight but a packet was missed, then move to the next channel
  else if (radio1.FHSS && micros() - radio1.lastHopTime > 630000UL && radio1.signalEst && radio1.chnlUsed < 3)
  {
    Serial.println("-------Radio 1 Missed Packet--------");
    activeRadio = &radio1;
    hopFreq();
  }

  if (event == 0 && radio2.FHSS && micros() - radio2.lastHopTime > 2000000UL && radio2.signalEst && radio2.chnlUsed < 3)
  {
    Serial.println("-------Radio 1 Missed Packet--------");
    activeRadio = &radio2;
    // signal to move to the synch freq because the necessary lead time on the ground is too long to try and catch the next packet
    activeRadio->chnlUsed = 3;
    hopFreq();
  }

  else if (radio2.FHSS && micros() - radio2.lastHopTime > 630000UL && radio2.signalEst && radio2.chnlUsed < 3)
  {
    Serial.println("-------Radio 2 Missed Packet--------");
    activeRadio = &radio2;
    hopFreq();
  }

  // display lost signal status
  if (micros() - lastRX > lostSignalTime && signalEst)
  {

    if (debugSerial)
    {
      Serial.println(F("Signal Lost!"));
    }

    /* EEPROM Allocations:
    Posn 00-19: Sustainer Name
    Posn 20-23: Latitude Float
    Posn 24-27: Longitude Float
    Posn    28: Radio1 Freq
    Posn 29-48: Booster Name
    Posn 49-52: Latitude Float
    Posn 53-56: Longitude Float
    Posn    57: Radio2 Freq
    */

    // Write the last GPS coordinates to EEPROM for later display
    for (byte i = 0; i < sizeof(rocketName); i++)
    {
      EEPROM.update(i, rocketName[i]);
    }
    radioUnion.GPScoord = lastGPSlat;
    for (byte i = 0; i < 4; i++)
    {
      EEPROM.update(i + 20, radioUnion.unionByte[i]);
    }
    radioUnion.GPScoord = lastGPSlon;
    for (byte i = 0; i < 4; i++)
    {
      EEPROM.update(i + 24, radioUnion.unionByte[i]);
    }

    // sync the SD card
    if (SDinit && sustainerFileOpen)
    {
      sustainerFile.sync();
    }

    // display last coordinates on LCD
    signalLostLCD();

    signalEst = false;
  }

  // GPS
  while (Serial2.available() > 0)
  {
    if (!msgRX)
    {
      msgRX = true;
    }
    char c = Serial2.read();
    GPS.encode(c);
    if (GPSdebug)
    {
      serialBuffer[serialPosn] = c;
      serialPosn++;
    }
  }
  if (GPSdebug && msgRX)
  {
    serialBuffer[serialPosn] = '\0';
    Serial.print(serialBuffer);
    msgRX = false;
    serialPosn = 0;
    timeLastNMEA = micros();
  }

  // check LED
  if (ledLight && micros() - colorStart > 2000000)
  {
    byte bright = whiteIntensity;
    lcd.setBacklight(bright, bright, bright); // Set backlight to white
    ledLight = false;
  }

} // end loop

void clearIRQ1()
{
  noInterrupts();
  radio1Pkt = true;
  interrupts();
}

void clearIRQ2()
{
  noInterrupts();
  radio2Pkt = true;
  interrupts();
}
