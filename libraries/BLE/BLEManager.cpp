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

#include "BLEManager.h"

#ifdef __cplusplus
extern "C"{
#endif

#include "ble_conn_state.h"

#ifdef __cplusplus
}
#endif

BLEPeripheral *BLEManagerClass::_peripheralList[1] = {0};
BLECentralRole *BLEManagerClass::_centralList[1] = {0};

BLEManagerClass::BLEManagerClass(){}

bool BLEManagerClass::registerPeripheral(BLEPeripheral *peripheral) {
    _peripheralList[0] = peripheral;
}

bool BLEManagerClass::registerCentral(BLECentralRole *central){
	_centralList[0] = central;
}

void processBleEvents(ble_evt_t *bleEvent){
    ble_conn_state_on_ble_evt(bleEvent);

	uint16_t handler = bleEvent->evt.gap_evt.conn_handle;
	uint16_t role = ble_conn_state_role(handler);

	if(role == BLE_GAP_ROLE_PERIPH){
		if(BLEManagerClass::_peripheralList[0] != 0){
			BLEManagerClass::_peripheralList[0]->poll(bleEvent);
		}
	}
	if((role == BLE_GAP_ROLE_CENTRAL) || (bleEvent->header.evt_id == BLE_GAP_EVT_ADV_REPORT)){
		if(BLEManagerClass::_centralList[0] != 0){
			BLEManagerClass::_centralList[0]->poll(bleEvent);
		}
	}
}

bool isPeripheralRunning(){
	return (BLEManagerClass::_peripheralList[0] != 0) ? true : false;
}

bool isCentralRunning(){
	return (BLEManagerClass::_centralList[0] != 0) ? true : false;
}

void callEvtListener(uint32_t type, uint32_t code){
	BLEManagerClass::_peripheralList[0]->callEvtListener(type, code);
}

BLEManagerClass BLEManager;	