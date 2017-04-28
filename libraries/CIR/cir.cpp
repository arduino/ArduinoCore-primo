/*
****************************************************************************
* Copyright (c) 2017 Arduino srl. All right reserved.
*
* File : cir.cpp
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

#include "cir.h"

cir::cir(void) {

}

void cir::enableReceiver(void) {
	  nrf_delay_ms(100);
	  Wire.begin();
	  Wire.beginTransmission(0x48);
	  Wire.write(0xA0);
	  Wire.endTransmission();
}

void cir::disableReceiver(void) {
	  nrf_delay_ms(15);
	  Wire.begin();
	  Wire.beginTransmission(0x48);
	  Wire.write(0xA1);
	  Wire.endTransmission();
}

bool cir::getReceiverStatus(void) {
	 nrf_delay_ms(150);
	 Wire.begin();
	 Wire.beginTransmission(0x48);
	 Wire.write(0xA2);
	 Wire.endTransmission();
	 nrf_delay_ms(15);
	 Wire.requestFrom(0x48,4, true);
	 recBuffer[0] = Wire.read();

	 int i;

	 for (i=1;i<4;i++) {
		 recBuffer[i] = Wire.read();
	 }

	 if (recBuffer[0] == 0)
		 return false;
	 else
		 return true;
}

uint32_t cir::digitalRead(void) {
	uint32_t code=0;
	code  =  (recBuffer[0] << 24);
	code |=  (recBuffer[1] << 16);
	code |=  (recBuffer[2] <<  8);
	code |=  recBuffer[3];

	return code;
}

void cir::enableTransmitter(void) {
	  nrf_delay_ms(100);
	  Wire.begin();
	  Wire.beginTransmission(0x48);
	  Wire.write(0xA3);
	  Wire.endTransmission();
	  nrf_delay_ms(1500);
}

void cir::disableTransmitter(void) {
	  nrf_delay_ms(100);
	  Wire.begin();
	  Wire.beginTransmission(0x48);
	  Wire.write(0xA4);
	  Wire.endTransmission();

}

void cir::digitalWrite(uint32_t data) {

	  uint8_t transdata[4];
	  transdata[0] = (data >> 24);
	  transdata[1] = (data >> 16);
	  transdata[2] = (data >> 8);
	  transdata[3] = data;

	  nrf_delay_ms(200);
	  Wire.begin();
	  Wire.beginTransmission(0x48);
	  Wire.write(0xA5);
	  Wire.write(transdata[0]);
	  Wire.write(transdata[1]);
	  Wire.write(transdata[2]);
	  Wire.write(transdata[3]);
	  Wire.endTransmission();
}
