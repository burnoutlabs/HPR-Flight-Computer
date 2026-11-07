void setPolyMorphs()
{

  // To enable faster execution and simplicity of code, the "begin" and "get" sensor functions are pointers

  // accelerometer pointers
  switch (sensors.accel)
  {

  case 1:
    beginAccel = beginLSM303_A;
    getAccel = getLSM303_A;
    break;

  case 2:
    beginAccel = beginGyro = beginLSM9DS1_AG;
    getAccel = getGyro = getLSM9DS1_AG;
    break;

  case 3:
    beginAccel = beginGyro = beginLSM6DS33;
    getAccel = getGyro = getLSM6DS33;
    break;

  case 4:
    beginAccel = beginGyro = beginMPU6050;
    getAccel = getGyro = getMPU6050;
    break;

  case 5:
    beginAccel = beginGyro = beginMPU9250_AG;
    getAccel = getGyro = getMPU9250_AG;
    break;

  case 6:
    beginAccel = beginGyro = beginLSM6DSOX;
    getAccel = getGyro = getLSM6DSOX;
    break;

  case 7: // External Library
    beginAccel = beginExternalAccel;
    getAccel = getExternalAccel;
    break;

  case 8:
    beginAccel = beginGyro = beginLSM6DS3TR;
    getAccel = getGyro = getLSM6DS3TR;
    break;

  default:
    beginAccel = beginExternalAccel;
    getAccel = getExternalAccel;
    break;
  }

  // gyroscope pointers
  switch (sensors.gyro)
  {

  case 1:
    beginGyro = beginL3GD20H;
    getGyro = getL3GD20H;
    break;

  case 7: // External Library
    beginGyro = beginExternalGyro;
    getGyro = getExternalGyro;
    break;

  default: // all other casesare
    beginGyro = dummyBool;
    getGyro = dummyVoid;
    break;
  }

  // High-G Accelerometer Pointers
  switch (sensors.highG)
  {

  case 1:
    beginHighG = beginADS1115;
    getHighG = getADS1115;
    break;

  case 2:
    beginHighG = beginH3LIS331DL;
    getHighG = getH3LIS331DL;
    break;

  case 3:
    beginHighG = beginADXL377;
    getHighG = getADXL377;
    break;

  case 4:
    beginHighG = beginExternalHighG;
    getHighG = getExternalHighG;
    break;

  default:
    beginHighG = dummyBool;
    getHighG = dummyVoid;
    break;
  }

  // magnetometer pointers
  switch (sensors.mag)
  {

  case 1:
    beginMag = beginLSM303_M;
    getMag = getLSM303_M;
    break;

  case 2:
    beginMag = beginLSM9DS1_M;
    getMag = getLSM9DS1_M;
    break;

  case 3:
    beginMag = beginLIS3MDL;
    getMag = getLIS3MDL;
    break;

  case 4:
    beginMag = beginMPU9250_M;
    getMag = getMPU9250_M;
    break;

  case 5: // External Library
    beginMag = beginExternalMag;
    getMag = getExternalMag;
    break;

  default:
    beginMag = dummyBool;
    getMag = dummyVoid;
    break;
  }

  // set barometer pointers
  switch (sensors.baro)
  {

  case 1:
    beginBaro = beginBMP180;
    getBaro = getBMP180;
    break;

  case 2:
    beginBaro = beginMPL3115A2;
    getBaro = getMPL3115A2;
    break;

  case 3:
    beginBaro = beginBMP280;
    getBaro = getBMP280;
    break;

  case 4:
    beginBaro = beginBMP388;
    getBaro = getBMP388;
    break;

  case 5:
    beginBaro = beginMS56XX;
    getBaro = getMS56XX;
    break;

  case 6:
    beginBaro = beginMS56XX;
    getBaro = getMS56XX;
    break;

  case 7:
    beginBaro = beginLPS25H;
    getBaro = getLPS25H;
    break;

  case 8:
    beginBaro = beginExternalBaro;
    getBaro = getExternalBaro;
    break;

  default:
    beginBaro = beginExternalBaro;
    getBaro = getExternalBaro;
    break;
  }

  // set radio pointers
  switch (sensors.radio)
  {

  case 1:
    beginRadio = beginSX127X;
    setRadioFreq = setFreqSX127X;
    setRadioPWR = setPwrSX127X;
    radioSleep = sleepSX127X;
    radioSendPkt = sendPktSX127X;
    break;

  case 2:
    beginRadio = beginRFD900;
    setRadioFreq = dummyBool1;
    setRadioPWR = dummyBool2;
    radioSleep = dummyVoid;
    radioSendPkt = sendPktRFD900;
    break;

  case 3:
    beginRadio = beginExternalRadio;
    setRadioFreq = setExternalRadioFreq;
    setRadioPWR = setExternalRadioPwr;
    radioSleep = externalRadioSleep;
    radioSendPkt = sendExternalRadioDataPkt;
    break;

  default: // no radio
    beginRadio = dummyBool;
    setRadioFreq = dummyBool1;
    setRadioPWR = dummyBool2;
    radioSleep = dummyVoid;
    radioSendPkt = dummyBool3;
    break;
  }

  // set GNSS device pointers
  switch (sensors.GNSS)
  {

  case 1:
    GNSSrestorDefaults = UBLOXrestorDefaults;
    GNSSconfig = UBLOXconfig;
    GNSSpowerSave = UBLOXpowerSave;
    break;

  case 2:
    GNSSrestorDefaults = UBLOXrestorDefaults;
    GNSSconfig = UBLOXconfig;
    GNSSpowerSave = UBLOXpowerSave;
    break;

  case 3:
    GNSSrestorDefaults = UBLOXrestorDefaults;
    GNSSconfig = UBLOXconfig;
    GNSSpowerSave = UBLOXpowerSave;
    break;

  case 4:
    GNSSrestorDefaults = dummyVoid;
    GNSSconfig = ExternalGNSSconfig;
    GNSSpowerSave = externalGNSS_PSM;
    break;

  default: // no GNSS
    GNSSrestorDefaults = dummyVoid;
    GNSSconfig = dummyVoid;
    GNSSpowerSave = dummyVoid;
    break;
  }

} // end setPolyMorphs()
