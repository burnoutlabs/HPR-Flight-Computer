/**
 * @file Device_Drivers.h
 * @brief Drivers for the sensors the flight computer supports directly, in Device_Drivers.cpp.
 */
#pragma once

#include <Arduino.h>

bool beginLSM303_A();
bool beginLSM303_M();
void getLSM303_A();
void getLSM303_M();
bool beginL3GD20H();
void getL3GD20H();
bool beginLSM9DS1_AG();
bool beginLSM9DS1_M();
void getLSM9DS1_AG();
void getLSM9DS1_M();
bool beginLSM6DS3TR();
void getLSM6DS3TR();
bool beginLSM6DS33();
void getLSM6DS33();
bool beginLSM6DSOX();
void getLSM6DSOX();
bool beginMPU6050();
void getMPU6050();
bool beginMPU9250_AG();
void getMPU9250_AG();
bool beginMPU9250_M();
void getMPU9250_M();
bool beginLIS3MDL();
void getLIS3MDL();
bool beginLPS25H();
void getLPS25H();
bool beginADXL377();
void getADXL377();
bool beginH3LIS331DL();
void getH3LIS331DL();
bool beginADS1115();
void getADS1115();
boolean beginMPL3115A2();
void getMPL3115A2();
float pressureToAltitude(float seaLevel, float atmospheric);
bool beginBMP180();
void getBMP180();
void initiateTemp();
void initiatePressure();
void getPressure();
boolean beginBMP280();
void getBMP280();
boolean beginBMP388();
void getBMP388();
void cmdMS56XX(byte cmd);
bool beginMS56XX();
void getMS56XX();
float ConvertTempMS56XX();
float ConvertPressMS56XX();
