/*
****************************************************************************
* Copyright (c) 2017 Arduino srl. All right reserved.
*
* File : bat.cpp
* Date : 2017/04/10
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

#include "bat.h"

bat::bat(void) {

}

double bat::analogRead(void) {
	double voltage;
	uint32_t code=0;

	delay(150);

	int i;
	for (i=0;i<4;i++) {
		recBuffer[i] = 0;
	}

	Wire.begin();
	Wire.beginTransmission(0x48);
	Wire.write(0xED);
	Wire.endTransmission();
	delay(15);
	Wire.requestFrom(0x48,4, true);
	recBuffer[0] = Wire.read();

	for (i=1;i<4;i++) {
		recBuffer[i] = Wire.read();
	}

	code  =  (recBuffer[0] << 24);
	code |=  (recBuffer[1] << 16);
	code |=  (recBuffer[2] <<  8);
	code |=  recBuffer[3];

	voltage = (6.665 * code) / 4096;

	return voltage;
}
