/*
****************************************************************************
* Copyright (c) 2017 Arduino srl. All right reserved.
*
* File : stm32pwr.h
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


#ifndef STM32PWR_h
#define STM32PWR_h

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif


class stm32pwrClass{

	public:
		
		/**
         * \turn the WiFi on
         */
         void powerOnWiFi(void) ;

        /**
         * \turn the WiFi off
         */
         void powerOffWiFi(void) ;

        /**
         * \enable WiFi
         */
         void enableWiFi(void) ;

        /**
         * \disable WiFi
         */
         void disableWiFi(void) ;

        /**
         * \setup USER1_BUTTON as an interrupt to trigger STM32 enter to sleep mode or wake up from sleep mode
         */
         void enableSleepMode(void) ;

        /**
         * \disable STM32 enter to standby mode
         */
         void disableStandbyMode(void) ;
		
	private:
		
};

extern stm32pwrClass stm32pwr;

#endif //STM32PWR_h