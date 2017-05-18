/*
****************************************************************************
* Copyright (c) 2017 Arduino srl. All right reserved.
*
* File : stm32pwr.cpp
* Date : 2017/04/30
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


#include "stm32pwr.h"



void stm32pwrClass::powerOnWiFi()
{
  digitalWrite(GPIO_ESP_PW, HIGH); 
} 

void stm32pwrClass::powerOffWiFi()
{
  digitalWrite(GPIO_ESP_PW, LOW);
}

void stm32pwrClass::enableWiFi()
{
  digitalWrite(GPIO_ESP_EN, HIGH);
}

void stm32pwrClass::disableWiFi()
{
  digitalWrite(GPIO_ESP_EN, LOW); 
}

void stm32pwrClass::enableSleepMode()
{
  pinMode(USER1_BUTTON, STM32_IT);
}

void stm32pwrClass::disableStandbyMode()
{
  pinMode(BAT_VOL, INPUT); 
}
 

stm32pwrClass stm32pwr;
