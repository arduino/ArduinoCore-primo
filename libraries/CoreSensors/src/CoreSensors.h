/*
****************************************************************************
* Copyright (c) 2017 Arduino srl. All right reserved.
*
* File : CoreSensors.h
* Date : 2017/06/10
* Revision : 0.0.1 $
* Author: jeff[at]arduino[dot]org
*
****************************************************************************

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef __CORE_SENSORS_H_
#define __CORE_SENSORS_H_

#include <Wire.h>
#include <Arduino.h>
#include <stdlib.h>

#include "DevI2C_Arduino/DevI2C.h"
#include "HTS221Sensor/HTS221Sensor.h"
#include "LIS3MDLSensor/LIS3MDLSensor.h"
#include "LSM6DS3Sensor/LSM6DS3Sensor.h"

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

#ifdef __cplusplus
}
#endif //

class CoreSensors {
 public:
    CoreSensors();
    void begin();
    float getHumidity();
    float getTemperature();
    void getMagnetometer(int32_t *pData);
    void getAccelerometer(int32_t *pData);
    void getGyroscope(int32_t *pData);
    void enableSingleTapDetection();
    void getStatusSingleTapDetection(uint8_t &Data);
    void enableDoubleTapDetection();
    void getStatusDoubleTapDetection(uint8_t &Data);
    void enableFreeFallDetection();
    void getStatusFreeFallDetection(uint8_t &Data);
    void enableTiltDetection();
    void getStatusTiltDetection(uint8_t &Data);
    void enableWakeUpDetection();
    void getStatusWakeUpDetection(uint8_t &Data);
    void enablePedometer();
    void getStatusPedometer(uint8_t &Data);
    void getStepCounter(uint16_t &Data);
    void enable6DOrientation();
    void getStatus6DOrientation(uint8_t &Data);
    void get6DOrientationXL(uint8_t &Data);
    void get6DOrientationXH(uint8_t &Data);
    void get6DOrientationYL(uint8_t &Data);
    void get6DOrientationYH(uint8_t &Data);
    void get6DOrientationZL(uint8_t &Data);
    void get6DOrientationZH(uint8_t &Data);

 private:
    float humidity;
    float temperature;
};

extern CoreSensors coresensors;

#endif __CORE_SENSORS_H_