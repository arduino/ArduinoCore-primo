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

#include "BLEService.h"
#include "Arduino.h"
BLEService::BLEService(BLEUuid uuid){
	_uuid=uuid;
}

BLEService::BLEService(const char *uuidString){
    bool err_code = _uuid.set(uuidString);
	if(!err_code) SDManager.registerError("BLEService::BLEService(const char *uuidString)", err_code, "format of UUID string incorrect");
}

BLEService::BLEService(uint16_t shortUuid){
	_uuid.set(shortUuid);
}

void BLEService::addCharacteristic(BLECharacteristic& characteristic){
    characteristic.setParentService(this);
	_characteristicList.add(&characteristic);
}

BLEUuid BLEService::getUuid(void){
	return _uuid;
}

BLEService * BLEService::getNextElement(void){
	return _nextElement;
}

void BLEService::setNextElement(BLEService * element){
	_nextElement = element;
}

LinkedList<BLECharacteristic *> BLEService::getCharacteristicList(void){
	return _characteristicList;
}

void BLEService::pushServiceToSD(){
	ble_uuid_t sUuid={_uuid.getAlias(), _uuid.getType()};
	uint16_t service_handle;
	//add the service to the SoftDevice
	uint32_t err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &sUuid, &service_handle);
	if(err_code!=0) SDManager.registerError("BLEPeripheral::begin()", err_code, "add service failed");
	//add all its characteristics
	BLECharacteristic *characteristic = _characteristicList.getFirstElement();
		while(characteristic != 0){
			characteristic->pushCharacteristicToSD(service_handle);
			characteristic = characteristic->getNextElement();
		}
}

void BLEService::onGattsEventWrite(ble_gatts_evt_write_t *ble_gatts_evt_write){
	//Forward the event to all the characteristics in the service
	BLECharacteristic *characteristic = _characteristicList.getFirstElement();
    while(characteristic != 0){
        characteristic->onGattsEventWrite(ble_gatts_evt_write);
        characteristic = characteristic->getNextElement();
    }    
}