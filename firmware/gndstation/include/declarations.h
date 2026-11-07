/**
 * @file declarations.h
 * @brief Types and extern declarations of the globals shared by the ground station sources.
 *
 * @details
 * Holds the radio register modes, the settings and radio structs, and the packet, SD, GPS
 * and LCD variables. The variables are defined, with their initial values, in globals.cpp;
 * the hardware objects (SD card, LCD, GPS, timers) are defined in the file that drives them.
 */
#pragma once

#include <Arduino.h>

// RegOpMode
enum
{
  SleepMode = 0x00,
  StandbyMode = 0x01,
  TXmode = 0x03,
  RXmode = 0x05,
  CADmode = 0x07,
  LoRaMode = 0x80,
  LowFrqMode = 0x08, // LowFrq mode enables registers below 860MHz
  writeMask = 0x80
};

//---------------------------
// User Settings
//---------------------------
struct UserSettings
{
  boolean radio1enable = true;
  boolean radio2enable = true;
  float radio1freq;
  float radio2freq;
  boolean radio1FHSS = false;
  boolean radio2FHSS = false;
  byte radio1Pwr = 13;
  byte radio2Pwr = 13;
  boolean displayMetric = false;
  byte debugSerial = 1;
  char callSign[7] = "KK4ELF";
  boolean LCD = false;
  byte LCDbrightness = 75;
  byte LCDcontrast = 50;
  boolean LCDcolorEvents = false;
  boolean blueTooth = true;
  boolean enableSD = true;
  boolean enableGPS = true;
};
alignas(4) extern UserSettings settings;
//---------------------------
// Radio Control variables
//---------------------------
typedef struct
{
  float frq;
  byte num;
  boolean enable = true;
  boolean FHSS = false;
  boolean status = false;
  byte TXpwr = 13;
  byte dataPacket[256];
  boolean signalEst = false;
  uint32_t lastRX = 0UL;
  uint8_t cs;
  uint8_t rst;
  uint8_t hailChnl = 0;
  uint8_t chnl1;
  uint8_t nextChnl;
  uint8_t nextChnl2;
  uint8_t chnlUsed = 0;
  boolean syncFreq = true;
  uint32_t lastHopTime = 0UL;
} radio;
extern radio radio1;
extern radio radio2;
extern radio *activeRadio;
extern char serialBuffer[256];
extern int serialPosn;

extern byte dataPkt[80];
extern volatile boolean processIRQ;
extern uint32_t timeLastPkt;
extern uint32_t debugTime;
extern int16_t pktRssi;
extern float radio1Freq;
extern float radio2Freq;
extern byte radio1Fnctn;
extern byte radio2Fnctn;
//---------------------------
// Data Packet variables
//---------------------------
extern uint8_t len;
alignas(4) extern char dataString[512];
extern boolean SDstatus;
extern char sustainerFileName[13];
extern char boosterFileName[14];
extern boolean sustainerFileOpen;
extern boolean boosterFileOpen;
extern boolean sustainerFileCreated;
extern boolean boosterFileCreated;
extern byte n;
extern byte fileNum;
extern byte pktPosn;
extern int16_t packetnum;
extern unsigned long timer;
extern boolean radio1status;
extern boolean radio2status;
extern boolean sendPacket;
extern boolean testMode;
extern unsigned long testStart;
extern unsigned long lastRX;
extern unsigned long debugStart;
extern unsigned long debugStop;
extern unsigned long colorStart;
extern int battVolt;
extern boolean SDinit;
extern boolean LCDinit;
extern boolean parseSustainer;
extern boolean parseBooster;
extern uint32_t timeLastNMEA;
extern bool ledLight;
//---------------------------
// preflight output variables
//---------------------------
extern uint8_t event;
extern int strPosn;
extern boolean signalEst;
extern boolean dataProcessed;
extern boolean sustainerPreFlightWrite;
extern boolean boosterPreFlightWrite;
extern boolean sustainerPostFlightWrite;
extern boolean boosterPostFlightWrite;
extern boolean boosterFileReady;
extern unsigned long lostSignalTime;
extern byte j;
//---------------------------
// preflight output variables
//---------------------------
alignas(4) extern char rocketName[20];
extern int baseAlt;
extern int baseGPSalt;
extern byte contCode;
extern int satNum;
extern boolean fileOpen;
//---------------------------
// inflight output variables
//---------------------------
extern uint16_t sampleTime;
extern int16_t signalStrength;
extern int16_t velocity;
extern int16_t Alt;
extern int16_t spin;
extern int16_t offVert;
extern int16_t accel;
extern int16_t packetNum;
extern boolean apogee;
extern unsigned long lastSustainerRX;
extern unsigned long lastBoosterRX;
//---------------------------
// Postflight output variables
//---------------------------
extern int maxAltitude;
extern int maxVelocity;
extern int maxG;
extern int maxGPSalt;
// GPS output variables
extern byte GPSlock;
extern int GPSalt;
extern int prevGPSalt;
extern char charGPSlat;
extern float GPSlatitude;
extern char charGPSlon;
extern float GPSlongitude;
extern float lastGPSlat;
extern float lastGPSlon;
extern float prevGPSlat;
extern float prevGPSlon;
extern unsigned long lastGPSfix;
//---------------------------
// Radio Variables
//---------------------------
extern boolean GPSdebug;
extern boolean debugSerial;
extern float unitConvert;
extern volatile boolean radio1Pkt;
extern volatile boolean radio2Pkt;
union RadioUnion
{
  float GPScoord;
  unsigned long radioTime;
  byte unionByte[4];
};
extern RadioUnion radioUnion;
union RadioInt
{
  int16_t unionInt;
  byte unionByte[2];
};
extern RadioInt radioInt;
extern byte radioFnctn;
//---------------------------
// LCD Variables
//---------------------------
extern byte whiteIntensity;
extern byte redIntensity;
extern byte greenIntensity;
extern byte blueIntensity;
extern byte flightPhase;
//---------------------------
// Event Table
//---------------------------

extern const String eventTable[33];

extern byte greenEvents[3];
extern byte redEvents[5];

//---------------------------
// Pyro Code Table
//---------------------------

extern const String pyroTable[10];

extern byte greenCont[5];
extern byte redCont[5];

extern uint8_t eventPosn;
