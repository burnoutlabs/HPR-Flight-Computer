/**
 * @file globals.cpp
 * @brief Definitions of the ground station's global variables.
 *
 * @details
 * declarations.h declares these extern for every source file; this is the one place they
 * are defined, with their initial values.
 */
#include <Arduino.h>

#include "declarations.h"

char serialBuffer[256];
int serialPosn = 0;

byte dataPkt[80];
volatile boolean processIRQ = false;
uint32_t timeLastPkt = 0UL;
uint32_t debugTime;
int16_t pktRssi;
float radio1Freq = 433.500;
float radio2Freq = 902.300;

byte radio1Fnctn = RXmode;
byte radio2Fnctn = RXmode;

//---------------------------
// User Settings
//---------------------------
alignas(4) UserSettings settings;
//---------------------------
// Radio Control variables
//---------------------------
radio radio1;
radio radio2;
radio *activeRadio;
//---------------------------
// Data Packet variables
//---------------------------
uint8_t len;
// dataString is defined in SD_Card.cpp
boolean SDstatus = false;
char sustainerFileName[13] = "FLIGHT01.txt";
char boosterFileName[14] = "BOOSTER01.txt";
boolean sustainerFileOpen = false;
boolean boosterFileOpen = false;
boolean sustainerFileCreated = false;
boolean boosterFileCreated = false;
byte n = 0;
byte fileNum = 1;
byte pktPosn = 0;
int16_t packetnum = 0;
unsigned long timer = 0UL;
boolean radio1status = false;
boolean radio2status = false;
boolean sendPacket = true;
boolean testMode = false;
unsigned long testStart = 0UL;
unsigned long lastRX = 0UL;
unsigned long debugStart = 0UL;
unsigned long debugStop = 0UL;
unsigned long colorStart;
int battVolt;
boolean SDinit = false;
boolean LCDinit = true;
boolean parseSustainer = false;
boolean parseBooster = false;
uint32_t timeLastNMEA = 0UL;
bool ledLight = false;
//---------------------------
// preflight output variables
//---------------------------
uint8_t event;
// strPosn is defined in SD_Card.cpp
boolean signalEst = false;
boolean dataProcessed = false;
boolean sustainerPreFlightWrite = true;
boolean boosterPreFlightWrite = true;
boolean sustainerPostFlightWrite = true;
boolean boosterPostFlightWrite = true;
boolean boosterFileReady = false;
unsigned long lostSignalTime = 2000000UL;
byte j = 0;
//---------------------------
// preflight output variables
//---------------------------
alignas(4) char rocketName[20] = "";
int baseAlt = 0;
int baseGPSalt = 0;
byte contCode;
int satNum = 0;
boolean fileOpen = true;
//---------------------------
// inflight output variables
//---------------------------
uint16_t sampleTime;
int16_t signalStrength = 0;
int16_t velocity = 0;
int16_t Alt = 0;
int16_t spin = 0;
int16_t offVert = 0;
int16_t accel = 0;
int16_t packetNum = 0;
boolean apogee = false;
unsigned long lastSustainerRX = 0UL;
unsigned long lastBoosterRX = 0UL;
//---------------------------
// Postflight output variables
//---------------------------
int maxAltitude = 0;
int maxVelocity = 0;
int maxG = 0;
int maxGPSalt = 0;
// GPS output variables
byte GPSlock;
int GPSalt = 0;
int prevGPSalt;
char charGPSlat;
float GPSlatitude;
char charGPSlon;
float GPSlongitude;
float lastGPSlat;
float lastGPSlon;
float prevGPSlat = 0.0;
float prevGPSlon = 0.0;
unsigned long lastGPSfix = 0UL;
//---------------------------
// Radio Variables
//---------------------------
boolean GPSdebug = false;
boolean debugSerial = true;
float unitConvert = 3.28084F;
volatile boolean radio1Pkt = false;
volatile boolean radio2Pkt = false;
RadioUnion radioUnion;
RadioInt radioInt;
byte radioFnctn;
//---------------------------
// LCD Variables
//---------------------------
byte whiteIntensity = 125;
byte redIntensity = 255;
byte greenIntensity = 255;
byte blueIntensity = 255;
byte flightPhase = 0;
//---------------------------
// Event Table
//---------------------------
const String event_00 = "Preflight";
const String event_01 = "Liftoff";
const String event_02 = "Booster Burnout";
const String event_03 = "Apogee Detected";
const String event_04 = "Firing Apogee Pyro";
const String event_05 = "Separation Detected!";
const String event_06 = "Firing Mains";
const String event_07 = "Under Chute";
const String event_08 = "Ejecting Booster";
const String event_09 = "Firing 2nd Stage";
const String event_10 = "2nd Stage Ignition";
const String event_11 = "2nd Stage Burnout";
const String event_12 = "Firing Airstart1";
const String event_13 = "Airstart1 Ignition";
const String event_14 = "Airstart1 Burnout";
const String event_15 = "Firing Airstart2";
const String event_16 = "Airstart2 Ignition";
const String event_17 = "Airstart2 Burnout";
const String event_18 = "NoFire: Rotn Limit";
const String event_19 = "NoFire: Alt Limit";
const String event_20 = "NoFire: Rotn/Alt Lmt";
const String event_21 = "Booster Apogee";
const String event_22 = "Booster Apogee Fire";
const String event_23 = "Booster Separation!";
const String event_24 = "Booster Main Deploy";
const String event_25 = "Booster Under Chute";
const String event_26 = "Time Limit Exceeded";
const String event_27 = "Touchdown!";
const String event_28 = "Power Loss! Restart!";
const String event_29 = "Booster Touchdown!";
const String event_30 = "Booster Preflight";
const String event_31 = "Booster Time Limit";
const String event_32 = "Booster Pwr Restart";

const String eventTable[] = {
    event_00, event_01, event_02, event_03, event_04,
    event_05, event_06, event_07, event_08, event_09,
    event_10, event_11, event_12, event_13, event_14,
    event_15, event_16, event_17, event_18, event_19,
    event_20, event_21, event_22, event_23, event_24,
    event_25, event_26, event_27, event_28, event_29,
    event_30, event_31, event_32};

byte greenEvents[] = {5, 6, 7};
byte redEvents[] = {18, 19, 20, 26, 28};

//---------------------------
// Pyro Code Table
//---------------------------
const String cont_0 = "No Pyros Detected!";
const String cont_1 = "No Continuity Pyro 1";
const String cont_2 = "No Continuity Pyro 2";
const String cont_3 = "No Continuity Pyro 3";
const String cont_4 = "No Continuity Pyro 4";
const String cont_5 = "All 3 Pyros Detected";
const String cont_6 = "All 4 Pyros Detected";
const String cont_7 = "Pyro Apogee Only";
const String cont_8 = "Pyro Mains Only";
const String cont_9 = "Pyro Mains & Apogee";

const String pyroTable[] = {
    cont_0, cont_1, cont_2, cont_3, cont_4,
    cont_5, cont_6, cont_7, cont_8, cont_9};

byte greenCont[5] = {5, 6, 7, 8, 9};
byte redCont[5] = {0, 1, 2, 3, 4};

uint8_t eventPosn = 0;
