/*
****************************************************************************
* Copyright (c) 2017 Arduino srl. All right reserved.
*
* File : CoreSensors.cpp
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

#include "CoreSensors.h"

// Components.
HTS221Sensor  *HumTemp;
LIS3MDLSensor *Magneto;
LSM6DS3Sensor *AccGyr;
DevI2C *dev_i2c;

CoreSensors::CoreSensors(){

}

void CoreSensors::begin(){
    dev_i2c = new DevI2C(&Wire1, 400000, 0);

    HumTemp  = new HTS221Sensor (*dev_i2c);
    HumTemp->Enable();

    Magneto  = new LIS3MDLSensor(*dev_i2c);
    Magneto->Enable();

    AccGyr = new LSM6DS3Sensor(*dev_i2c);
    AccGyr->Enable_X();
    AccGyr->Enable_G();
}

float CoreSensors::getHumidity(){
    HumTemp->GetHumidity(&humidity);
    return humidity;
}

float CoreSensors::getTemperature(){
    HumTemp->GetTemperature(&temperature);
    return temperature;
}

void CoreSensors::getMagnetometer(int32_t *pData){
    Magneto->GetAxes(pData);
}

void CoreSensors::getAccelerometer(int32_t *pData){
    AccGyr->Get_X_Axes(pData);
}
void CoreSensors::getGyroscope(int32_t *pData){
    AccGyr->Get_G_Axes(pData);
}

void CoreSensors::enableSingleTapDetection(){
    AccGyr->Enable_Single_Tap_Detection();
}

void CoreSensors::getStatusSingleTapDetection(uint8_t &Data){
    AccGyr->Get_Status_Single_Tap_Detection((uint8_t *)&Data);
}

void CoreSensors::enableDoubleTapDetection(){
    AccGyr->Enable_Double_Tap_Detection();
}

void CoreSensors::getStatusDoubleTapDetection(uint8_t &Data){
    AccGyr->Get_Status_Double_Tap_Detection((uint8_t *)&Data);
}

void CoreSensors::enableFreeFallDetection(){
    AccGyr->Enable_Free_Fall_Detection();
}

void CoreSensors::getStatusFreeFallDetection(uint8_t &Data){
    AccGyr->Get_Status_Free_Fall_Detection((uint8_t *)&Data);
}

void CoreSensors::enableTiltDetection(){
    AccGyr->Enable_Tilt_Detection();
}

void CoreSensors::getStatusTiltDetection(uint8_t &Data){
    AccGyr->Get_Status_Tilt_Detection((uint8_t *)&Data);
}

void CoreSensors::enableWakeUpDetection(){
    AccGyr->Enable_Wake_Up_Detection();
}

void CoreSensors::getStatusWakeUpDetection(uint8_t &Data){
    AccGyr->Get_Status_Wake_Up_Detection((uint8_t *)&Data);
}

void CoreSensors::enablePedometer(){
    AccGyr->Enable_Pedometer();
}

void CoreSensors::getStatusPedometer(uint8_t &Data){
    AccGyr->Get_Status_Pedometer((uint8_t *)&Data);
}

void CoreSensors::getStepCounter(uint16_t &Data){
    AccGyr->Get_Step_Counter((uint16_t *)&Data);
}

void CoreSensors::enable6DOrientation(){
    AccGyr->Enable_6D_Orientation();
}

void CoreSensors::getStatus6DOrientation(uint8_t &Data){
    AccGyr->Get_Status_6D_Orientation((uint8_t *)&Data);
}

void CoreSensors::get6DOrientationXL(uint8_t &Data){
    AccGyr->Get_6D_Orientation_XL((uint8_t *)&Data);
}

void CoreSensors::get6DOrientationXH(uint8_t &Data){
    AccGyr->Get_6D_Orientation_XH((uint8_t *)&Data);
}

void CoreSensors::get6DOrientationYL(uint8_t &Data){
    AccGyr->Get_6D_Orientation_YL((uint8_t *)&Data);
}

void CoreSensors::get6DOrientationYH(uint8_t &Data){
    AccGyr->Get_6D_Orientation_YH((uint8_t *)&Data);
}

void CoreSensors::get6DOrientationZL(uint8_t &Data){
    AccGyr->Get_6D_Orientation_ZL((uint8_t *)&Data);
}

void CoreSensors::get6DOrientationZH(uint8_t &Data){
    AccGyr->Get_6D_Orientation_ZH((uint8_t *)&Data);
}

CoreSensors coresensors;