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

#include "BLECharacteristic.h"


BLECharacteristic::BLECharacteristic(const char * uuid, uint8_t properties, uint8_t *data, uint16_t dataLength, bool variableLength){
	_uuid.set(uuid);
	_properties=properties;
	fillCharStructures(_uuid, properties, data, dataLength, variableLength);
}

BLECharacteristic::BLECharacteristic(const char * uuid, uint8_t properties){
	_uuid.set(uuid);
	_properties=properties;
	fillCharStructures(_uuid, properties, NULL, 0, 0);
}

BLECharacteristic::BLECharacteristic(uint16_t shortUuid, uint8_t properties, uint8_t *data, uint16_t dataLength, bool variableLength){
	_uuid.set(shortUuid);
	_properties=properties;
	fillCharStructures(_uuid, properties, data, dataLength, variableLength);
}

BLECharacteristic::BLECharacteristic(uint16_t shortUuid, uint8_t properties){
	_uuid.set(shortUuid);
	_properties=properties;
	fillCharStructures(_uuid, properties, NULL, 0, 0);
}

void BLECharacteristic::addDescriptor(BLEDescriptor& descriptor){
	descriptorList.add(&descriptor);
}
		
void BLECharacteristic::setEventHandler(BLECharacteristicEventHandlerType callback){
	//
}		

void BLECharacteristic::setValue(uint8_t *data_ptr, uint16_t length, BLESetType setType){
	_setType=setType;
	attr_char_value.init_len=length;
	attr_char_value.max_len=length;
	attr_char_value.p_value=data_ptr;
}

void BLECharacteristic::setValue(uint8_t *data_ptr){
	setValue(data_ptr, strlen((char *)data_ptr));
}
	
void BLECharacteristic::setValue(uint8_t data_ptr){
	setValue(&data_ptr, 1);
}

void BLECharacteristic::setValue(const char *data_ptr){
	setValue((uint8_t *)data_ptr, strlen(data_ptr));
}

uint8_t BLECharacteristic::getProperties(void){
	return _properties;
}

BLECharacteristic * BLECharacteristic::getNextElement(){
	return nextElement;
}

void BLECharacteristic::setNextElement(BLECharacteristic * element){
	nextElement=element;
}


void BLECharacteristic::pushCharacteristicToSD(uint16_t service_handle){
	uint32_t err_code=sd_ble_gatts_characteristic_add(service_handle, &char_md, &attr_char_value, &char_handl);
	if(err_code!=0) SDManager.registerError("BLECharacteristic::pushCharacteristicToSD()", err_code, "add characteristic failed");
}

void BLECharacteristic::fillCharStructures(BLEUuid uuid, uint8_t properties, uint8_t *data, uint16_t dataLength, bool variableLength){
	memset(&char_md, 0, sizeof(char_md));
	//set characteristic properties
	char_md.char_props.broadcast=properties & 0b00000001;
	char_md.char_props.read=(properties & 0b00000010)>>1;
	char_md.char_props.write_wo_resp=(properties & 0b00000100)>>2;
	char_md.char_props.write=(properties & 0b00001000)>>3;
	char_md.char_props.notify=(properties & 0b00010000)>>4;
	char_md.char_props.indicate=(properties & 0b00100000)>>5;
	char_md.char_props.auth_signed_wr=(properties & 0b01000000)>>6;
	//TODO: add user descriptor
	char_md.p_char_user_desc=NULL;
	//TODO: add format descriptor
	char_md.p_char_pf=NULL;
	//if notify or indicate properties are set, set the cccd's structure
	if((properties & 0b00110000)!=0){
		memset(&cccd_md, 0, sizeof(cccd_md));
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
		cccd_md.vloc=BLE_GATTS_VLOC_STACK;
		char_md.p_cccd_md=&cccd_md;
	}
	else
		char_md.p_cccd_md=NULL;
	
	char_md.p_sccd_md=NULL;
	
	//set attribute metadata structure
	memset(&attr_md, 0, sizeof(attr_md));
	if((properties & 0b00000010)>>1)
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	if((properties & 0b00001000)>>3)
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
	attr_md.vloc       =  BLE_GATTS_VLOC_STACK;
	attr_md.rd_auth    = 0;
	attr_md.vlen       = variableLength;
	if((properties & 0b00000100)>>2)
		attr_md.wr_auth    = 1;
	
	//set characteristic value structure
	memset(&attr_char_value, 0, sizeof(attr_char_value));
	cUuid.uuid=_uuid.getAlias();
	cUuid.type=_uuid.getType();
	attr_char_value.p_uuid=&cUuid;
	attr_char_value.p_attr_md=&attr_md;
	attr_char_value.init_offs=0;
	//if data = NULL user will insert data with setValue funciton
	if(data!=NULL){
		attr_char_value.init_len=dataLength;
		attr_char_value.max_len=dataLength;
		attr_char_value.p_value=data;
	}

}