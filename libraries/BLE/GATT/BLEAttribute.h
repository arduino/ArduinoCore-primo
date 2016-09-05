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

typedef uint16_t BLEPermissions;

class BLEAttribute {
	public:
		BLEUuid getUuid(void);
		uint16_t getHandle(void);
		uint8_t getValue(void);
		uint16_t getValueLength(void);
		BLEPermissions getPermissions(void);
		
	protected:
		void setHandle(uint16_t handle);
		
	private:
		BLEUuid uuid;
		uint16_t handle;
		BLEPermissions permissions;
		uint8_t *value;
		uint16_t data_length;
};

#endif
