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

#ifndef __BLEDESCRIPTOR_H
#define __BLEDESCRIPTOR_H

#include <stdint.h>
#include "BLEAttribute.h"
#include "..\common\BLEUuid.h"

class BLEDescriptor : public BLEAttribute {
	
	public:
		BLEDescriptor(const char * uuid, uint8_t *data, uint16_t dataLength);
		BLEDescriptor(uint16_t shortUuid, uint8_t *data, uint16_t dataLength);
		BLEDescriptor(const char * uuid, char data[]);
		BLEDescriptor(uint16_t shortUuid, char data[]);
		BLEDescriptor * getNextElement(void);
		void setNextElement(BLEDescriptor * element);
		void pushDescriptorToSD(uint16_t char_handle);
		
	private:
		BLEDescriptor        *nextElement = 0;
		ble_gatts_attr_t     attr_desc_value;
		ble_uuid_t           dUuid;
		ble_gatts_attr_md_t  attr_md;
		uint16_t             *desc_handle;
};

#endif
