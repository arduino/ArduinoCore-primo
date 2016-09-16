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

#include "BLEDescriptor.h"

BLEDescriptor::BLEDescriptor(const char * uuid, uint8_t *data, uint16_t dataLength){
	setUuid(uuid);
	setValue(data);
	setValueLength(dataLength);
}

BLEDescriptor::BLEDescriptor(uint16_t shortUuid, uint8_t *data, uint16_t dataLength){
	setUuid(shortUuid);
	setValue(data);
	setValueLength(dataLength);
}

BLEDescriptor::BLEDescriptor(const char * uuid, char data[]){
	setUuid(uuid);
	setValue((uint8_t *)data);
	setValueLength((uint16_t)strlen(data));
}

BLEDescriptor::BLEDescriptor(uint16_t shortUuid, char data[]){	
	setUuid(shortUuid);
	setValue((uint8_t *)data);
	setValueLength((uint16_t)strlen(data));
}
			
BLEDescriptor * BLEDescriptor::getNextElement(){
	return nextElement;
}
		
void BLEDescriptor::setNextElement(BLEDescriptor * element){
	nextElement = element;
}

void BLEDescriptor::pushDescriptorToSD(uint16_t char_handle){	
	//set attribute metadata structure
	memset(&attr_md, 0, sizeof(attr_md));
	//set read permission
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	attr_md.vloc       =  BLE_GATTS_VLOC_STACK;
	
	
	//set descriptor value structure
	memset(&attr_desc_value, 0, sizeof(attr_desc_value));
	dUuid.uuid = getUuid().getAlias();
	dUuid.type = getUuid().getType();
	attr_desc_value.p_uuid    = &dUuid;
	attr_desc_value.p_attr_md = &attr_md;
	attr_desc_value.init_len  = getValueLength();
	attr_desc_value.max_len   = getValueLength();
	attr_desc_value.p_value   = getValue();
	
	uint32_t err_code = sd_ble_gatts_descriptor_add(char_handle, &attr_desc_value, desc_handle);
	if(err_code != 0) SDManager.registerError("BLEDescriptor::pushDescriptorToSD()", err_code, "add descriptor failed");
}
