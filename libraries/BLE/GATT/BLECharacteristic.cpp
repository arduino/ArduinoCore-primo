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
	_characteristicValue.setUuid(uuid);
	_properties=properties;
	_characteristicValue.setValue(data);
	_characteristicValue.setValueLength(dataLength);
}

BLECharacteristic::BLECharacteristic(const char * uuid, uint8_t properties){
	_characteristicValue.setUuid(uuid);
	_properties=properties;
}

BLECharacteristic::BLECharacteristic(uint16_t shortUuid, uint8_t properties, uint8_t *data, uint16_t dataLength, bool variableLength){
	_characteristicValue.setUuid(shortUuid);
	_characteristicValue.setValue(data);
	_characteristicValue.setValueLength(dataLength);
	_properties=properties;
}

BLECharacteristic::BLECharacteristic(uint16_t shortUuid, uint8_t properties){
	_characteristicValue.setUuid(shortUuid);
	_properties=properties;
}

void BLECharacteristic::addDescriptor(BLEDescriptor& descriptor){
	descriptorList.add(&descriptor);
}
		
void BLECharacteristic::setEventHandler(BLECharacteristicEventHandlerType callback){
	//
}		

void BLECharacteristic::setValue(uint8_t *data_ptr, uint16_t length, BLESetType setType){
	_characteristicValue.setValue(data_ptr);
	_characteristicValue.setValueLength(length);
	_setType=setType;
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

BLEAttribute BLECharacteristic::getCharacteristicValue(){
	return _characteristicValue;
}