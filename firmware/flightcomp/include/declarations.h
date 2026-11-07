/**
 * @file declarations.h
 * @brief Types and extern declarations of the globals shared by the flight computer sources.
 *
 * @details
 * Holds the EEPROM address map, the settings, sensor, pin, and flight-state structs, the
 * radio register and packet variables, and the device function pointers that setPolyMorphs()
 * (Device_Mgmt.cpp) assigns at startup. The variables are defined, with their initial
 * values, in globals.cpp.
 */
#pragma once

#include <Arduino.h>

// alignas(4) on a byte-aligned array, struct or union: while every global was defined in the
// one translation unit, GCC gave these word alignment (its -O2 data alignment on ARM) and
// generated code that relies on it. Most files now see only the extern declaration, so the
// alignment is stated here, and on the definition, to keep that code the same.

// Header file for all major declared variables
//-----------------------------------------------
// Written by SparkyVT, TRA #12111, NAR #85720, L3
//-----------Change Log--------------------------
// 29 Nov 23: Broke out a separate header file to make reading the main file easier
//-----------------------------------------
// EEPROM Locations
//-----------------------------------------
struct EepromMap
{
  int maxFltAlt = 0;  // 00-05: maximum altitude of last flight
  int accelBiasX = 6; // 06-11: accel.bias(X,Y,Z)
  int accelBiasY = 8;
  int accelBiasZ = 10;
  int highGbiasX = 12; // 12-17: highG.bias(X,Y,Z)
  int highGbiasY = 14;
  int highGbiasZ = 16;
  int magBiasX = 18; // 18-23: mag.bias(X,Y,Z)
  int magBiasY = 20;
  int magBiasZ = 22;
  int accelXsign = 24; // 24-29: accelerometer to world orientation (x-sign,x-pointer,y-sign,y-pointer,z-sign,z-pointer)
  int accelXptr = 25;
  int accelYsign = 26;
  int accelYptr = 27;
  int accelZsign = 28;
  int accelZptr = 29;
  int gyroXsign = 30; // 30-35: gyro to world orientation (x-sign,x-pointer,y-sign,y-pointer,z-sign,z-pointer)
  int gyroXptr = 31;
  int gyroYsign = 32;
  int gyroYptr = 33;
  int gyroZsign = 34;
  int gyroZptr = 35;
  int magXsign = 36; // 36-41: magnetometer to world orientation (x-sign,x-pointer,y-sign,y-pointer,z-sign,z-pointer)
  int magXptr = 37;
  int magYsign = 38;
  int magYptr = 39;
  int magZsign = 40;
  int magZptr = 41;
  int highGxSign = 42; // 42-47: highG to world orientation (x-sign,x-pointer,y-sign,y-pointer,z-sign,z-pointer)
  int highGxPtr = 43;
  int highGySign = 44;
  int highGyPtr = 45;
  int highGzSign = 46;
  int highGzPtr = 47;
  int accel2boardXaxis = 48; // 48-53: accelerometer to main board orientation (x-sign,x-axis,y-sign,y-axis,z-sign,z-axis)
  int accel2boardXsign = 49;
  int accel2boardYaxis = 50;
  int accel2boardYsign = 51;
  int accel2boardZaxis = 52;
  int accel2boardZsign = 53;
  int gyro2boardXaxis = 54; // 54-59: gyro to main board orientation (x-sign,x-axis,y-sign,y-axis,z-sign,z-axis)
  int gyro2boardXsign = 55;
  int gyro2boardYaxis = 56;
  int gyro2boardYsign = 57;
  int gyro2boardZaxis = 58;
  int gyro2boardZsign = 59;
  int mag2boardXaxis = 60; // 60-65: magnetometer to main board orientation (x-sign,x-axis,y-sign,y-axis,z-sign,z-axis)
  int mag2boardXsign = 61;
  int mag2boardYaxis = 62;
  int mag2boardYsign = 63;
  int mag2boardZaxis = 64;
  int mag2boardZsign = 65;
  int highG2boardXaxis = 66; // 66-71: high-G accelerometer to main board orientation (x-sign,x-axis,y-sign,y-axis,z-sign,z-axis)
  int highG2boardXsign = 67;
  int highG2boardYaxis = 68;
  int highG2boardYsign = 69;
  int highG2boardZaxis = 70;
  int highG2boardZsign = 71;
  int accelBusType = 72; // 72-93: device bus info
  int accelBusNum = 73;
  int gyroBusType = 74;
  int gyroBusNum = 75;
  int magBusType = 76;
  int magBusNum = 77;
  int highGBusType = 78;
  int highGBusNum = 79;
  int baroPressOffset = 80; // 80-83 : barometer pressure offset, stays in this position so I don't have to recalibrate all my flight computers
  int baroTempOffset = 84;  // 84-87  : barometer temperature offset, stays in this position so I don't have to recalibrate all my flight computers
  int baroBusType = 88;
  int baroBusNum = 89;
  int radioBusType = 90;
  int radioBusNum = 91;
  int sdBusType = 92;
  int sdBusNum = 93;
  int gpsBusType = 94;
  int gpsBusNum = 95;
  int pyro1ContPin = 96;    // 96     : Pyro 1 Continuity Pin
  int pyro1FirePin = 97;    // 97     : Pyro 1 Fire Pin
  int pyro2ContPin = 98;    // 98     : Pyro 2 Continuity Pin
  int pyro2FirePin = 99;    // 98     : Pyro 2 Fire Pin
  int pyro3ContPin = 100;   // 100   : Pyro 3 Continuity Pin
  int pyro3FirePin = 101;   // 101   : Pyro 3 Fire Pin
  int pyro4ContPin = 102;   // 102   : Pyro 4 Continuity Pin
  int pyro4FirePin = 103;   // 103   : Pyro 4 Fire Pin
  int nullPin = 104;        // 104        : Null Continuity / Fire Pin
  int beepPin = 105;        // 105        : Beeper Pin
  int battReadPin = 106;    // 106    : Battery Read Pin
  int testModeGndPin = 107; // 107 : Test Mode Button Gnd Pin
  int testModeRdPin = 108;  // 108   : Test Mode Read Pin
  int radioCSpin = 109;     // 109     : radio CS pin
  int radioIRQpin = 110;    // 110    : radio interrupt pin
  int radioRstPin = 111;    // 111    : radio reset pin
  int radioEnPin = 112;     // 112     : radio enable pin (optional w/ Adafruit breakout only)
  int accelCSpin = 113;
  int gyroCSpin = 114;
  int magCSpin = 115;
  int highGcsPin = 116;
  int sdCSpin = 117;
  int baroCSpin = 118;
  int servo1pin = 119; // 119   : servo 1 control pin
  int servo2pin = 120; // 120   : servo 2 control pin
  int servo3pin = 121; // 121   : servo 3 control pin
  int servo4pin = 122; // 122   : servo 4 control pin
  int servo5pin = 123; // 123   : servo 5 control pin
  int servo6pin = 124; // 124   : servo 6 control pin
  int servo7pin = 125; // 125   : servo 7 control pin
  int servo8pin = 126; // 126   : servo 8 control pin
  int accelID = 127;   // 127   : Sensor ID accel
  int gyroID = 128;    // 128   : Sensor ID gyro
  int highGID = 129;   // 129   : Sensor ID highG
  int baroID = 130;    // 130   : Sensor ID baro
  int radioID = 131;   // 131   : Sensor ID Radio
  int GPSID = 132;     // 132   : Sensor ID GPS
  int magID = 133;
  int sdID = 134;
  int HWversion = 135;
  int HWsubVersion = 136;
  int HWunitNum = 137;
  int callSign = 138; // 138-143: Ham Radio Callsign
  //-------------------------------------------------------------
  // flight settings stored in EEPROM to facilitate rapid recovery
  //-------------------------------------------------------------
  int rocketName = 144; // Maximum of 20 characters
  int fltProfile = 164;
  int units = 165;
  int reportStyle = 166;
  int setupTime = 167;         // 4
  int gTrigger = 171;          // 2
  int detectLiftoffTime = 173; // 4
  int mainDeployAlt = 177;     // 2
  int rcdTime = 179;           // 4
  int apogeeDelay = 183;
  int silentMode = 184;
  int magSwitchEnable = 185;
  int inflightRecover = 186;
  int gpsLogFile = 187;
  int fireTime = 188; // 4
  int pyro4Func = 192;
  int pyro3Func = 193;
  int pyro2Func = 194;
  int pyro1Func = 195;
  int TXenable = 196;
  int TXpwr = 197;
  int TXfreq = 198; // 4
  int FHSS = 202;
  int boosterSeparationDelay = 203; // 4
  int sustainerFireDelay = 207;     // 4
  int airStart1Event = 211;
  int airStart1Delay = 212; // 4
  int airStart2Event = 216;
  int airStart2Delay = 217; // 2
  int altThreshold = 219;   // 2
  int maxAngle = 221;
  int stableRotn = 222;
  int stableVert = 223;
  int flyback = 224;
  int serialDebug = 225;
  int lastFile = 226; // 2
  int baseAlt = 228;  // 4
  int lastEvent = 232;
  //-------------------------------------------------------------
  // active stabilization parameters
  //-------------------------------------------------------------
  int servo1trim = 233;
  int servo2trim = 234;
  int servo3trim = 235;
  int servo4trim = 236;
  int servo5trim = 237;
  int servo6trim = 238;
  int servo7trim = 239;
  int servo8trim = 240;
  int Kp = 241;           // 4
  int Ki = 245;           // 4
  int Kd = 249;           // 4
  int totalFlights = 253; // 2
  int gyroBiasX = 255;    // 2
  int gyroBiasY = 257;    // 2
  int gyroBiasZ = 259;    // 2
};
// Never written: const gives it internal linkage, so each address folds to a constant
const EepromMap eeprom{};
//-----------------------------------------
// Sensor device data
//-----------------------------------------
struct SensorConfig
{
  byte accel = 0;
  byte mag = 0;
  byte gyro = 0;
  byte highG = 0;
  byte baro = 0;
  byte radio = 0;
  byte GNSS = 0;
  char accelBusType;
  byte accelBusNum;
  char gyroBusType;
  byte gyroBusNum;
  char magBusType;
  byte magBusNum;
  char baroBusType;
  byte baroBusNum;
  char highGBusType;
  byte highGBusNum;
  char radioBusType;
  byte radioBusNum;
  char gnssBusType;
  byte gnssBusNum;
  char sdBusType;
  byte sdBusNum;
  bool statusAccel = false;
  bool statusGyro = false;
  bool statusHighG = false;
  bool statusBaro = false;
  bool statusMag = false;
  bool statusRadio = false;
  bool pyroPWM = false;
};
alignas(4) extern SensorConfig sensors;
//-----------------------------------------
// User Settings
//-----------------------------------------
struct UserSettings
{
  bool testMode = false;
  bool calibrationMode = false;
  //--------flight settings----------------
  char rocketName[20] = ""; // Maximum of 20 characters
  char HWid[7] = "";
  char fltProfile = 'S';
  char units = 'S';
  char reportStyle = 'P';
  bool GPSfile = false;
  unsigned long setupTime = 5000000UL;
  float mainDeployAlt = 153;             // Up to 458m for main deploy
  unsigned long rcdTime = 900000000UL;   // 15min
  unsigned long apogeeDelay = 1000000UL; // 1.0s apogee delay
  bool silentMode = false;               // true turns off beeper
  bool magSwitchEnable = false;
  byte inflightRecover = 0;
  bool GPSlog = false;
  byte serialDebug = 1;
  //--------pyro settings----------------
  unsigned long fireTime = 500000UL; // 0.5s
  char pyro4Func = 'M';
  char pyro3Func = 'A';
  char pyro2Func = 'N';
  char pyro1Func = 'N';
  //--------telemetry settings----------------
  char callSign[7] = "";
  bool TXenable = true; // false turns off radio transmissions
  byte TXpwr = 13;
  float TXfreq = 433.250;
  bool FHSS = false;
  //--------2 stage settings----------------
  unsigned long boosterSeparationDelay = 500000UL; // 0.5s
  unsigned long sustainerFireDelay = 1000000UL;    // 1.0s
  //--------airstart settings----------------
  char airStart1Event = 'B';
  unsigned long airStart1Delay = 500000UL;
  char airStart2Event = 'B';
  unsigned long airStart2Delay = 500000UL;
  //-------safety thresholds----------------
  int altThreshold = 120; // 120m = 400ft
  int maxAngle = 45;      // degrees
  //-------active stabilization----------------
  bool stableRotn = false;
  bool stableVert = false;
  //----------flyback option--------------------
  bool flyBack = false;
};
extern UserSettings settings;
//-----------------------------------------
// Sensor variables
//-----------------------------------------
// rawData, the bus read buffer, is declared in Bus_Mgmt.h
typedef struct
{
  byte addr;
  int16_t ADCmax;
  float gainX;
  float gainY;
  float gainZ;
  int16_t rawX;
  int16_t rawY;
  int16_t rawZ;
  int8_t dirX;
  int8_t dirY;
  int8_t dirZ;
  char orientX;
  char orientY;
  char orientZ;
  int16_t *ptrX;
  int16_t *ptrY;
  int16_t *ptrZ;
  int8_t *ptrXsign;
  int8_t *ptrYsign;
  int8_t *ptrZsign;
  int32_t sumX0 = 0L;
  int32_t sumY0 = 0L;
  int32_t sumZ0 = 0L;
  int16_t x0;
  int16_t y0;
  int16_t z0;
  int16_t x;
  int16_t y;
  int16_t z;
  int16_t biasX;
  int16_t biasY;
  int16_t biasZ;
  uint32_t timeLastSamp = 0UL;
  uint32_t timeBtwnSamp = 10000UL;
  boolean newSamp = false;
} sensorData;
extern sensorData accel;
extern sensorData mag;
extern sensorData gyro;
extern sensorData highG;
//-----------------------------------------
// pyro variables
//-----------------------------------------
typedef struct
{
  char func;
  bool fireStatus = false;
  bool contStatus = false;
  unsigned long fireStart;
  uint8_t contPin;
  uint8_t firePin;
} pyroMap;
extern pyroMap pyro1;
extern pyroMap pyro2;
extern pyroMap pyro3;
extern pyroMap pyro4;
//-----------------------------------------
// flight events
//-----------------------------------------
struct eventList
{
  bool preLiftoff = true;
  bool inFlight = false;
  bool postFlight = false;
  bool liftoff = false;
  bool falseLiftoffCheck = true;
  bool boosterBurnout = false;
  bool boosterBurnoutCheck = false;
  bool boosterSeparation = false;
  bool sustainerFireCheck = false;
  bool sustainerFire = false;
  bool sustainerIgnition = false;
  bool sustainerBurnout = false;
  bool airStart1Check = false;
  bool airStart1Fire = false;
  bool airStart1Ignition = false;
  bool airStart1BurnoutCheck = false;
  bool airStart1Burnout = false;
  bool airStart2Check = false;
  bool airStart2Fire = false;
  bool airStart2Ignition = false;
  bool airStart2BurnoutCheck = false;
  bool airStart2Burnout = false;
  bool apogee = false;
  bool apogeeFire = false;
  bool apogeeSeparation = false;
  bool mainDeploy = false;
  bool touchdown = false;
  bool timeOut = false;
};
alignas(4) extern struct eventList events;
alignas(4) extern struct eventList resetEvents;
//-----------------------------------------
// Master timing variables
//-----------------------------------------
struct timerList
{
  unsigned long liftoff = 0UL;
  unsigned long detectLiftoffTime = 500000UL; // 0.5s
  unsigned long boosterBurnout = 0UL;
  unsigned long boosterSeparation = 0UL;
  unsigned long sustainerFireCheck = 0UL;
  unsigned long sustainerFire = 0UL;
  unsigned long sustainerIgnition = 0UL;
  unsigned long sustainerBurnout = 0UL;
  unsigned long airStart1Check = 0UL;
  unsigned long airStart1Fire = 0UL;
  unsigned long airStart1Ignition = 0UL;
  unsigned long airStart1BurnoutCheck = 0UL;
  unsigned long airStart1Burnout = 0UL;
  unsigned long airStart2Check = 0UL;
  unsigned long airStart2Fire = 0UL;
  unsigned long airStart2Ignition = 0UL;
  unsigned long airStart2BurnoutCheck = 0UL;
  unsigned long airStart2Burnout = 0UL;
  unsigned long apogee = 0UL;
  unsigned long apogeeFire = 0UL;
  unsigned long apogeeSeparation = 0UL;
  unsigned long mainDeploy = 0UL;
  unsigned long touchdown = 0UL;
  unsigned long padTime = 0UL;
  unsigned long tmClock = 0UL;
  unsigned long tmClockPrev = 0UL;
  unsigned long timeCurrent = 0UL;
  unsigned long dt = 0UL;
};
extern struct timerList fltTime;
extern struct timerList resetFltTime;
//-----------------------------------------
// GPIO pin mapping
//-----------------------------------------
struct PinMap
{
  uint8_t nullCont;
  uint8_t nullFire;
  uint8_t beep;
  uint8_t testGnd;
  uint8_t testRead;
  uint8_t mag;
  uint8_t batt;
  uint8_t pyro1Cont;
  uint8_t pyro1Fire;
  uint8_t pyro2Cont;
  uint8_t pyro2Fire;
  uint8_t pyro3Cont;
  uint8_t pyro3Fire;
  uint8_t pyro4Cont;
  uint8_t pyro4Fire;
  uint8_t radioCS;
  uint8_t radioIRQ;
  uint8_t radioRST;
  uint8_t radioEN;
  uint8_t servo1;
  uint8_t servo2;
  uint8_t servo3;
  uint8_t servo4;
  uint8_t servo5;
  uint8_t servo6;
  uint8_t servo7;
  uint8_t servo8;
  uint8_t accelCS;
  uint8_t gyroCS;
  uint8_t magCS;
  uint8_t highG_CS;
  uint8_t baroCS;
  uint8_t SD_CS;
  uint8_t firePin = 0;
};
alignas(4) extern PinMap pins;
//-----------------------------------------
// radio variables
//-----------------------------------------
alignas(4) extern uint8_t dataPacket[256];
extern bool liftoffSync;
extern unsigned long TXstartTime;
extern bool implicitHdr;
extern bool syncFreq;
extern uint8_t pktPosn;
extern uint8_t pktSize;
extern uint16_t sampNum;
extern uint8_t packetSamples;
extern bool gpsTransmit;
extern bool TX;
extern bool SDradioTX;
extern uint8_t radioMode;
extern volatile boolean clearTX;
extern volatile boolean buildPkt;
extern volatile boolean syncFlag;
extern bool sendPkt;
struct PacketIntervals
{
  uint32_t preLiftoff = 1000000UL;
  uint32_t inflight = 200000UL;
  uint32_t postFlight = 10000000UL;
  uint32_t samplesPerPkt = 4UL;
};
extern PacketIntervals pktInterval;
struct RadioTelemetry
{
  int16_t packetnum = 0;
  uint8_t event = 0;
  uint16_t fltTime = 0;
  int16_t baseAlt = 0;
  int16_t alt = 0;
  int16_t accel = 0;
  int16_t vel = 0;
  int16_t roll = 0;
  int16_t offVert = 0;
  int16_t maxAlt = 0;
  int16_t maxVel = 0;
  int16_t maxGPSalt = 0;
  int16_t maxG = 0;
  int16_t GPSalt = 0;
  uint8_t satNum = 0;
  bool pktCallsign = false;
  uint32_t lastSampTime = 0UL;
};
extern RadioTelemetry radio;
//-----------------------------------------
// continuity Booleans
//-----------------------------------------
struct ContinuityStatus
{
  bool apogee;
  bool main;
  bool boosterSep;
  bool upperStage;
  bool airStart1;
  bool airStart2;
  bool noFunc;
  bool error;
  uint8_t reportCode;
  uint8_t beepCode;
};
alignas(4) extern ContinuityStatus cont;
//-----------------------------------------
// Non-Event Booleans
//-----------------------------------------
extern bool rotnOK;
extern bool altOK;
extern bool beep;
extern bool pyroFire;
extern bool fileClose;
extern unsigned long boosterBurpTime;
extern float unitConvert;
//-----------------------------------------
// digital accelerometer variables
//-----------------------------------------
extern int g;
extern float accelNow;
extern float maxG;
extern int gTrigger; // 2.5G trigger
//-----------------------------------------
// High-G accelerometer variables
//-----------------------------------------
extern int high1G;
extern bool filterFull;
extern long highGsum;
extern int highGfilter[30];
extern uint8_t sizeHighGfilter;
extern uint8_t filterPosn;
extern float highGsmooth;
extern float adcConvert;
extern uint16_t ADCmidValue;
union CalUnion
{
  int16_t calValue;
  uint8_t calByte[2];
};
alignas(4) extern CalUnion calUnion;
//-----------------------------------------
// Altitude & Baro Sensor variables
//-----------------------------------------
struct BaroData
{
  float rawAlt = 0.0F;
  float Alt = 0.0F;
  float baseAlt = 10.0F;
  float maxAlt = 0.0F;
  float smoothAlt = 0.0F;
  float Vel = 0.0F;
  float maxVel = 0.0F;
  float pressure;
  float temperature;
  unsigned long timeLastSamp = 0UL;
  unsigned long timeBtwnSamp = 50000UL;
  bool newSamp = false;
  bool newTemp = false;
  float seaLevelPressure = 1013.25;
  float tempOffset;
  float pressOffset;
};
extern BaroData baro;
//-----------------------------------------
// Baro Reporting Variables
//-----------------------------------------
extern uint8_t baroTouchdown;
extern uint8_t touchdownTrigger;
extern float pressureAvg;
extern float pressureAvg5[5];
extern float pressureSum;
extern uint8_t pressurePosn;
//-----------------------------------------
// Magnetometer Variables
//-----------------------------------------
extern int16_t magTrigger;
extern uint8_t magCalibrateMode;
extern long magRoll;
extern int magOffVert;
extern int magPitch;
extern int magYaw;
//-----------------------------------------
// gyro & rotation variables
//-----------------------------------------
extern long dx;
extern long dy;
extern long dz;
extern float yawY0;
extern float pitchX0;
extern int pitchX;
extern int yawY;
extern long rollZ;
const float mlnth = 0.000001;
extern int offVert;
extern bool rotationFault;
//-----------------------------------------
// Rotation timing variables
//-----------------------------------------
extern unsigned long lastRotn; // time of the last call to update the rotation
extern unsigned long rotnRate; // 100 updates per second
//-----------------------------------------
// velocity calculation variables
//-----------------------------------------
extern float accelVel;
extern float accelAlt;
extern float maxVelocity;
extern float fusionVel;
extern float fusionAlt;
extern float thresholdVel;
extern float thresholdAlt;
extern uint32_t clearRailTime;
//-----------------------------------------
// beeper variables
//-----------------------------------------
extern uint8_t beep_counter;
extern uint8_t beepPosn;
extern uint32_t beep_delay;
extern uint8_t beepCode;
extern uint32_t beep_len;
extern uint32_t timeBeepStart;
extern uint32_t timeLastBeepEnd;
extern bool beepAlt;
extern bool beepVel;
extern bool beepCont;
extern bool beepAlarm;
extern bool livePyroAlarm;
const uint32_t short_beep_delay = 100000UL;
const uint32_t long_beep_delay = 800000UL;
const uint32_t alarmBeepDelay = 10000UL;
const uint32_t alarmBeepLen = 10000UL;
const uint32_t medBeepLen = 100000UL;
const uint32_t shortBeepLen = 10000UL;
//-----------------------------------------
// SD card writing variables
//-----------------------------------------
extern uint32_t writeStart;
extern uint32_t writeTime;
extern uint32_t maxWriteTime;
extern uint32_t writeThreshold;
extern uint32_t writeThreshCount;
extern bool syncApogee;
extern bool syncMains;
const byte decPts = 2;
const byte base = 10;
// strPosn and dataString, the SD card line buffer, are declared in SD_Card.h
alignas(4) extern uint8_t maxAltDigits[6];
alignas(4) extern uint8_t maxVelDigits[4];
alignas(4) extern uint8_t voltageDigits[2];
extern uint8_t altDigits;
extern uint8_t velDigits;
extern uint8_t n;
extern float voltage;
extern bool writeVolt;
extern unsigned long voltRate;
extern uint16_t voltReading;
extern unsigned long lastVolt;
extern bool reportCode; // true = report max altitude, false = report max velocity
extern uint8_t postFlightCode;
const char cs = ',';
//-----------------------------------------
// GNSS Variables
//-----------------------------------------
struct eventData
{
  float latitude;
  float longitude;
  float alt = 0.0;
  int year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t mili;
};
struct GnssData
{
  float alt = 0.0;
  float maxAlt = 0.0;
  float baseAlt = 0.0;
  float vel = 0.0F;
  float latitude;
  float longitude;
  float avgAlt[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
  float altBuff[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
  uint32_t timeBuff[5] = {0UL, 0UL, 0UL, 0UL, 0UL};
  bool bufferFull = false;
  float altSum = 0.0;
  uint8_t bufferPosn = 0;
  uint8_t altPosn = 0;
  eventData liftoff;
  eventData apogee;
  eventData touchdown;
  uint8_t fix = 0;
  uint16_t fixCount = 0;
  unsigned long timeLastFix = 0UL;
  bool SDwrite = false;
  bool configDefaults = false;
  bool configFlight = false;
  bool configPwrSave = false;
  // GPS Global variables
  uint8_t serialPosn = 0;
  char serialBuffer[60] = "";
  bool msgRX = false;
};
extern GnssData gnss;
//-----------------------------------------
// EEPROM useful unions
//-----------------------------------------
union UlongUnion
{
  unsigned long val;
  byte Byte[4];
};
extern UlongUnion ulongUnion;
union IntUnion
{
  int val;
  byte Byte[2];
};
extern IntUnion intUnion;
union FloatUnion
{
  float val;
  byte Byte[4];
};
extern FloatUnion floatUnion;
//-----------------------------------------
// Active Stabilization
//-----------------------------------------
extern char userInput;
extern int8_t servo1trim;
extern int8_t servo2trim;
extern int8_t servo3trim;
extern int8_t servo4trim;
extern int8_t servo5trim;
extern int8_t servo6trim;
extern int8_t servo7trim;
extern int8_t servo8trim;
const uint32_t controlInterval = 50000UL;
extern uint32_t timeLastControl;
extern float Kp;
extern float Ki;
extern float Kd;
//-----------------------------------------
// debug
//-----------------------------------------
extern boolean GPSecho;
extern boolean radioDebug;
extern boolean TXdataFile;
//-----------------------------------------
// loop debug and analysis variables
//-----------------------------------------
extern int cyclesBtwn;
extern uint32_t sampleTime;
extern uint32_t sampleStart;
extern uint32_t sampleTimeCheck;
extern uint32_t debugTimer;
//--------------------------------------------
// Radio event codes to eliminate magic numbers
//--------------------------------------------
enum flightCodes
{
  Preflight = 0,
  Liftoff = 1,
  Booster_Burnout = 2,
  Apogee = 3,
  Fire_Apogee = 4,
  Separation = 5,
  Fire_Mains = 6,
  Under_Chute = 7,
  Eject_Booster = 8,
  Fire_Sustainer = 9,
  Sustainer_Ignition = 10,
  Sustainer_Burnout = 11,
  Fire_Airstart1 = 12,
  Airstart1_Ignition = 13,
  Airstart1_Burnout = 14,
  Fire_Airstart2 = 15,
  Airstart2_Ignition = 16,
  Airstart2_Burnout = 17,
  NoFire_Rotn_Limit = 18,
  NoFire_Alt_Limit = 19,
  NoFire_RotnAlt_Limit = 20,
  Booster_Apogee = 21,
  Fire_Booster_Apogee = 22,
  Booster_Separation = 23,
  Fire_Booster_Mains = 24,
  Booster_Under_Chute = 25,
  Time_Limit = 26,
  Touchdown = 27,
  Power_Loss_Restart = 28,
  Booster_Touchdown = 29,
  Booster_Preflight = 30,
  Booster_Time_Limit = 31,
  Booster_Power_Loss = 32
};
//--------------------------------------------
// Radio operating modes (RegOpMode)
//--------------------------------------------
enum radioCodes
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
extern uint8_t radioFnctn;
//--------------------------------------------
// Function Pointers
//--------------------------------------------
/*!
 * @brief Initializes the configured accelerometer; returns false if it does not respond.
 * Set by setPolyMorphs() to the driver for the configured sensor type.
 */
extern bool (*beginAccel)();
/*!
 * @brief Initializes the configured gyroscope; returns false if it does not respond.
 * Set by setPolyMorphs() to the driver for the configured sensor type.
 */
extern bool (*beginGyro)();
/*!
 * @brief Initializes the configured magnetometer; returns false if it does not respond.
 * Set by setPolyMorphs() to the driver for the configured sensor type.
 */
extern bool (*beginMag)();
/*!
 * @brief Initializes the configured high-G accelerometer; returns false if it does not respond.
 * Set by setPolyMorphs() to the driver for the configured sensor type.
 */
extern bool (*beginHighG)();
/*!
 * @brief Initializes the configured barometer; returns false if it does not respond.
 * Set by setPolyMorphs() to the driver for the configured sensor type.
 */
extern bool (*beginBaro)();
extern bool (*beginRadio)();
/*!
 * @brief Reads raw data from the configured accelerometer.
 * Set by setPolyMorphs() to the driver for the configured sensor type.
 */
extern void (*getAccel)();
/*!
 * @brief Reads raw data from the configured gyroscope.
 * Set by setPolyMorphs() to the driver for the configured sensor type.
 */
extern void (*getGyro)();
/*!
 * @brief Reads raw data from the configured magnetometer.
 * Set by setPolyMorphs() to the driver for the configured sensor type.
 */
extern void (*getMag)();
/*!
 * @brief Reads raw data from the configured high-G accelerometer.
 * Set by setPolyMorphs() to the driver for the configured sensor type.
 */
extern void (*getHighG)();
/*!
 * @brief Reads raw data from the configured barometer.
 * Set by setPolyMorphs() to the driver for the configured sensor type.
 */
extern void (*getBaro)();
extern bool (*setRadioFreq)(float freq);
extern bool (*setRadioPWR)(int8_t pwr);
extern void (*radioSleep)();
extern bool (*radioSendPkt)(uint8_t *data, uint8_t len);
extern void (*GNSSrestorDefaults)();
extern void (*GNSSconfig)();
extern void (*GNSSpowerSave)();
