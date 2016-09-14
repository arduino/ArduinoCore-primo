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

#include "BLEPeripheral.h"
#include <string.h>
#include "ble_gap.h"

BLEPeripheral::BLEPeripheral(void){
    BLEManager::registerPeripheral(this);
    memset((void *)_peripheralEventHandlers, 0, BLEPeripheralEventNUM * sizeof(_peripheralEventHandlers[0]));
}

bool BLEPeripheral::begin(void){
    
	//scan all the services
	BLEService * service=serviceList.getFirstElement();
	while(service!=0){ 
		//register service and related characteristics
		service->pushServiceToSD();
		service=service->getNextElement();
	}
	
	// Configure the advertise packets in the SoftDevice
    pushAdvPacketsToSD();
	
	// Start advertising
    ble_gap_adv_params_t advParams;
    advParams.type = BLE_GAP_ADV_TYPE_ADV_IND;
    advParams.p_peer_addr = 0;
    advParams.fp = BLE_GAP_ADV_FP_ANY;
    advParams.interval = 250;
    advParams.timeout = 0;
    advParams.channel_mask.ch_37_off = 0;
    advParams.channel_mask.ch_38_off = 0;
    advParams.channel_mask.ch_39_off = 0;
    sd_ble_gap_adv_start(&advParams);

}

void BLEPeripheral::setAdvertisement(BLEAdvertisement &advertisement){
//
}

BLEAdvertisement &BLEPeripheral::getAdvertisement(){
//
}

void BLEPeripheral::setDeviceName(const char *deviceName){
    ble_gap_conn_sec_mode_t writePermission;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&writePermission);
    sd_ble_gap_device_name_set(&writePermission, (const uint8_t *)deviceName, strlen(deviceName));
}
    
void BLEPeripheral::setAppearance(const unsigned short appearance){
    BLEAdvertisement::setAppearance(appearance);
}
    
void BLEPeripheral::setPreferredConnectionParameters(uint16_t minConnInterval, uint16_t maxConnInterval, uint16_t slaveLatency, uint16_t supervisingTimeout){
//
}
	
void BLEPeripheral::setEventHandler(BLEPeripheralEventType event, BLEPeripheralEventHandler callback){
    if(event < BLEPeripheralEventNUM) {
        _peripheralEventHandlers[event] = callback;
    }
}

void BLEPeripheral::poll(void){
//
}

void BLEPeripheral::end(void){
//
}

bool BLEPeripheral::disconnect(void){
//
}

void BLEPeripheral::onBleEvent(ble_evt_t *bleEvent){
    switch(bleEvent->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            // Update internal connection state variables (inherited from BLELinkStatus)
            ble_gap_evt_connected_t *eventConnected;
            eventConnected = (ble_gap_evt_connected_t *)&bleEvent->evt.gap_evt.params.connected;
            _lsConHandle = bleEvent->evt.gap_evt.conn_handle;
            _lsConnected = true;
            _lsPeerAddress = eventConnected->peer_addr;
            _lsConRole = eventConnected->role;
            _lsConParameters = eventConnected->conn_params;
            
            // Call the event handler if it is registered
            if(_peripheralEventHandlers[BLEPeripheralEventConnected] != 0) {
                _peripheralEventHandlers[BLEPeripheralEventConnected](*this);
            }
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            // Update internal connection state variables (inherited from BLELinkStatus)
            _lsConnected = false;
            memset((void *)&_lsConParameters, 0, sizeof(ble_gap_conn_params_t));
            
            // Call the event handler if it is registered
            if(_peripheralEventHandlers[BLEPeripheralEventDisconnected] != 0) {
                _peripheralEventHandlers[BLEPeripheralEventDisconnected](*this);
            }        
            break;
        
        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
            _lsConParameters = bleEvent->evt.gap_evt.params.conn_param_update.conn_params;
            break;
            
        case BLE_GAP_EVT_TIMEOUT:
            if(_peripheralEventHandlers[BLEPeripheralEventTimeout] != 0) {
                _peripheralEventHandlers[BLEPeripheralEventTimeout](*this);
            }  
            break;
            
        default:
            break;
    }
}
