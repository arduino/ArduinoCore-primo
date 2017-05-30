/*
****************************************************************************
* Copyright (c) 2017 Arduino srl. All right reserved.
*
* File : cir.h
* Date : 2017/03/10
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

#ifndef __CIR_H__
#define __CIR_H__

#ifdef ARDUINO_NRF52_PRIMO_CORE
#error "CIR library is not compatible with Arduino Primo Core"
#endif //ARDUINO_NRF52_PRIMO_CORE

#include <Wire.h>
#include <stdint.h>
#include "Arduino.h"

extern "C" {
  #include "nrf_delay.h"
}

#define SEND_BUF_LENGTH 256
#define REC_BUF_LENGTH 74

class cir {
public:
	cir(void);
	void enableReceiver(void);
	void disableReceiver(void);
	bool getReceiverStatus(void);
	uint32_t digitalRead(void);

	void enableTransmitter(void);
	void disableTransmitter(void);
	void digitalWrite(uint32_t data);

	uint16_t recData[34];
	uint16_t leader_mark, leader_space, mark_max, mark_min, space_max, space_min;

private:
	uint8_t sendBuffer[SEND_BUF_LENGTH];
	uint8_t recBuffer[REC_BUF_LENGTH];


};

#endif __CIR_H___
