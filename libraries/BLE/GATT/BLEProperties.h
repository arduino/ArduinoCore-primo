/*
  Copyright (c) 2016 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
*/

#ifndef __BLEPROPERTIES_H
#define __BLEPROPERTIES_H

enum {
	BLEBroadcast = 0b00000001,
	BLERead = 0b00000010,
	BLEWriteCommand = 0b00000100,
	BLEWrite = 0b00001000,
	BLENotify = 0b00010000,
	BLEIndicate = 0b00100000,
	BLEAuthSighedWrite = 0b01000000
};


#endif