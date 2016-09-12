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

#ifndef __BLEATTRIBUTE_H
#define __BLEATTRIBUTE_H

#include <stdint.h>
#include "BLEUuid.h"
#include "SoftDeviceManager.h"


class BLEAttribute {
	public:
		BLEUuid getUuid(void);
		void setUuid(const char * uuidString);
		void setUuid(uint16_t shortUuid);
		uint16_t getHandle(void);
		uint8_t * getValue(void);
		void setValue(uint8_t * value);
		uint16_t getValueLength(void);
		void setValueLength(uint16_t dataLength);
		uint8_t getPermissions(void);
		void setPermissions(uint8_t permissions);
		
	protected:
		void setHandle(uint16_t handle);

		BLEUuid _uuid;
		uint16_t _handle;
		uint8_t _permissions;
		uint8_t *_value;
		uint16_t _dataLength;
};

#endif
