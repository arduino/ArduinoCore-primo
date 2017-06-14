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

CoreSensors coresensors;