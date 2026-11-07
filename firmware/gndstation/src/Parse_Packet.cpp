/**
 * @file Parse_Packet.cpp
 * @brief Handling of each telemetry packet the radios receive.
 *
 * The packets are decoded with the shared TelemetryProtocol library, the same code that
 * defines the format the flight computer sends (V4.7.2): a 6-character callsign, then the
 * event byte. loop() hands each received packet to processPacket(), which checks the
 * callsign and dispatches on the event to the parser for the packet type.
 */
#include <Arduino.h>
#include <TelemetryProtocol.h>

#include "declarations.h"
#include "LCD.h"
#include "Parse_Packet.h"
#include "Radio.h"
#include "SD_Card.h"

// Time between inflight samples, in hundredths of a second: the flight computer sends a
// packet every 200ms with 4 samples (pktInterval.inflight / pktInterval.samplesPerPkt).
static const uint16_t INFLIGHT_SAMPLE_INTERVAL_CS = 5;

bool processPacket()
{

  // every packet from our flight computer starts with its callsign
  if (!telemetry::callsignMatches(activeRadio->dataPacket, len, settings.callSign))
  {
    return false;
  }

  // set event flags
  event = (byte)activeRadio->dataPacket[telemetry::EVENT_OFFSET];
  if (settings.debugSerial)
  {
    if (event != telemetry::SYNC_MARKER)
    {
      Serial.println("-------Data Packet Received--------");
    }
    else
    {
      Serial.println("-------Sync Packet Received--------");
    }
    Serial.print("Packet on radio");
    Serial.print(activeRadio->num);
    Serial.print(", event: ");
    Serial.println(event);
    Serial.print("Length: ");
    Serial.println(len);
    Serial.print("TimeStamp: ");
    Serial.println(activeRadio->lastRX);
  }
  // determine if it is a sustainer or booster packet
  if (event < 21)
  {
    parseSustainer = true;
  }
  else if (event == 26 || event == 27 || event == 28)
  {
    parseSustainer = true;
  }
  else if (event != telemetry::SYNC_MARKER && event < 33)
  {
    parseBooster = true;
  }

  // Create the SD file
  if (parseSustainer && !sustainerFileCreated && SDinit)
  {
    createSustainerFile();
  }
  if (parseBooster && !boosterFileCreated && SDinit)
  {
    createBoosterFile();
  }

  // sync packet
  if (event == telemetry::SYNC_MARKER)
  {
    syncPkt(activeRadio->dataPacket);
  }

  // preflight packet
  else if (event == 0 || event == 30)
  {
    // parse the preFlight packet
    preflightPacket(activeRadio->dataPacket);
    // signal established
    signalEst = true;
    activeRadio->signalEst = true;
    // set lost signal time
    lostSignalTime = 2000000UL;
    // LCD
    preflightLCD();
  }

  // inflight packet
  else if (event < 26 && event != 28)
  {
    // set lost signal time
    lostSignalTime = 2000000UL;

    // write the last pre-flight packet to the SD card
    if (SDinit && sustainerPreFlightWrite && parseSustainer)
    {
      writePreflightData();
    }
    if (SDinit && boosterPreFlightWrite && parseBooster)
    {
      writePreflightData();
    }

    // parse the packet
    inflightPacket(activeRadio->dataPacket);

    // display to LCD
    inflightLCD();
  }

  // postflight packet
  else if (event == 26 || event == 27 || event == 29 || event == 31)
  {
    // set lost signal time
    lostSignalTime = 35000000UL;
    // parse the packet
    postflightPacket(activeRadio->dataPacket);
    // display to LCD
    postflightLCD();
  }

  // error packet
  else
  {
    errorLCD();
  }

  parseSustainer = false;
  parseBooster = false;
  return true;
}

void preflightPacket(byte rxPacket[])
{

  /*---------------------------------------------------
                PREFLIGHT PACKET
   -----------------------------------------------------*/

  // Read data from pre-flight packet
  telemetry::PreflightPacket pkt;
  if (!telemetry::decodePreflight(rxPacket, len, pkt))
  {
    return;
  }
  event = pkt.event;
  GPSlock = pkt.gnssFix;
  contCode = pkt.contCode;
  memcpy(rocketName, pkt.rocketName, sizeof(rocketName));
  baseAlt = pkt.baseAlt;
  baseGPSalt = pkt.gpsAlt;
  GPSlatitude = pkt.latitude;
  GPSlongitude = pkt.longitude;
  satNum = pkt.satNum;

  // the flight computer hops before every preflight packet: follow it to the next channel
  if (activeRadio->FHSS)
  {
    activeRadio->syncFreq = false;
    if (settings.debugSerial)
    {
      dispPktInfo();
      Serial.print(F("Hopping Freq: "));
    }
    activeRadio->chnlUsed = 0;
    hopFreq();
  }
  // capture the last good GPS coordinates to potentially store later in EEPROM
  if (GPSlock == 1)
  {
    lastGPSlat = GPSlatitude;
    lastGPSlon = GPSlongitude;
  }
}

void inflightPacket(byte rxPacket[])
{

  /*---------------------------------------------------
                INFLIGHT PACKET
   -----------------------------------------------------*/

  telemetry::InflightPacket pkt;
  if (!telemetry::decodeInflight(rxPacket, len, pkt))
  {
    return;
  }

  // the GPS data and packet number first, then the samples
  packetnum = pkt.packetNum;

  if (debugSerial)
  {
    Serial.print(F("Inflight Packet Received, PktNum: "));
    Serial.println(packetnum);
  }

  GPSalt = pkt.gpsAlt;
  GPSlatitude = pkt.latitude;
  GPSlongitude = pkt.longitude;

  // determine GPS lock from coordinates
  if (fabs(GPSlongitude) > 0)
  {
    GPSlock = 1;
    lastGPSfix = micros();
  }
  else
  {
    GPSlock = 0;
  }

  // parse inflight packet of 4 samples
  strPosn = 0;
  for (byte i = 0; i < telemetry::INFLIGHT_SAMPLES; i++)
  {

    // parse the samples
    const telemetry::InflightSample &sample = pkt.samples[i];
    event = sample.event;
    if (!apogee && event >= 4 && event <= 6)
    {
      apogee = true;
    }
    // the packet carries no sample time: count samples from the first packet after liftoff
    sampleTime = (uint16_t)(((packetnum - 1) * telemetry::INFLIGHT_SAMPLES + i) * INFLIGHT_SAMPLE_INTERVAL_CS);
    velocity = sample.velocity;
    if (!apogee && velocity > maxVelocity)
    {
      maxVelocity = velocity;
    }
    Alt = sample.altitude;
    if (!apogee && Alt > maxAltitude)
    {
      maxAltitude = Alt;
    }
    spin = sample.roll;
    offVert = sample.offVert;
    accel = sample.accel;
    if (SDinit)
    {
      writeInflightData();
    }
    if (!apogee && accel > maxG)
    {
      maxG = accel;
    }
  }

  // write to the SD card
  if (parseSustainer)
  {
    sustainerFile.write(dataString, strPosn);
  }
  if (parseBooster)
  {
    boosterFile.write(dataString, strPosn);
  }

  // sync the SD card
  if (SDinit && parseSustainer)
  {
    sustainerFile.sync();
  }
  if (SDinit && parseBooster)
  {
    boosterFile.sync();
  }

  if (settings.debugSerial)
  {
    Serial.println("Inflight Data Written");
  }
  // set the next channel: the flight computer hops after every 3rd packet
  if (activeRadio->FHSS)
  {
    activeRadio->syncFreq = false;
    if (debugSerial)
    {
      dispPktInfo();
    }
    activeRadio->lastHopTime = activeRadio->lastRX - (packetnum % 3 * 200000UL);
    activeRadio->chnlUsed = 0;
    if (packetnum % 3 == 0)
    {
      if (settings.debugSerial)
      {
        Serial.print(F("Hopping Freq: "));
      }
      hopFreq();
    }
  }

  // capture the last good GPS coordinates to potentially store later in EEPROM
  if (GPSlock == 1)
  {
    lastGPSlat = GPSlatitude;
    lastGPSlon = GPSlongitude;
  }
}

void postflightPacket(byte rxPacket[])
{

  /*---------------------------------------------------
                POSTFLIGHT PACKET
   -----------------------------------------------------*/
  telemetry::PostflightPacket pkt;
  if (!telemetry::decodePostflight(rxPacket, len, pkt))
  {
    return;
  }
  maxAltitude = pkt.maxAlt;
  maxVelocity = pkt.maxVel;
  maxG = pkt.maxG;
  maxGPSalt = pkt.maxGpsAlt;
  GPSlock = pkt.gnssFix;
  GPSalt = pkt.gpsAlt;
  GPSlatitude = pkt.latitude;
  GPSlongitude = pkt.longitude;

  // capture the last good GPS coordinates to potentially store later in EEPROM
  if (GPSlock == 1)
  {
    lastGPSlat = GPSlatitude;
    lastGPSlon = GPSlongitude;
  }

  // write to the SD card
  if (SDinit && parseSustainer && sustainerPostFlightWrite)
  {
    writePostflightData();
  }
  if (SDinit && parseBooster && boosterPostFlightWrite)
  {
    writePostflightData();
  }

  // sync the SD card
  if (SDinit && parseSustainer)
  {
    sustainerFile.flush();
  }
  if (SDinit && parseBooster)
  {
    boosterFile.flush();
  }

  // the flight computer hops before every postflight packet: follow it to the next channel
  if (activeRadio->FHSS)
  {
    activeRadio->chnlUsed = 0;
    hopFreq();
  }
}
