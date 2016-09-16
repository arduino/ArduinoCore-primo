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
	//enable the Softdevice if is not yet enabled
	if(!SDManager.isEnabled())
		SDManager.begin();
	
    // Register the peripheral with the BLEManager (future proofing in case we will have to support multiple peripherals)
    BLEManager.registerPeripheral(this);
    
    // Set default advertise parameters
    _advParams.type = BLE_GAP_ADV_TYPE_ADV_IND;
    _advParams.p_peer_addr = 0;
    _advParams.fp = BLE_GAP_ADV_FP_ANY;
    _advParams.interval = 250;
    _advParams.timeout = 0;
    _advParams.channel_mask.ch_37_off = 0;
    _advParams.channel_mask.ch_38_off = 0;
    _advParams.channel_mask.ch_39_off = 0;
    
    // Clear the peripheral event handlers
    memset((void *)_peripheralEventHandlers, 0, BLEPeripheralEventNUM * sizeof(_peripheralEventHandlers[0]));
}

bool BLEPeripheral::begin(void){
    
	//scan all the services
	BLEService * service = serviceList.getFirstElement();
	while(service != 0){ 
		//register service and related characteristics
		service->pushServiceToSD();
		service = service->getNextElement();
	}
	
	// Configure the advertise packets in the SoftDevice
    pushAdvPacketsToSD();
	
	// Start advertising
    sd_ble_gap_adv_start(&_advParams);
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

bool BLEPeripheral::setLocalName(const char* localName){
    setDeviceName(localName);
    return BLEAdvertisement::setLocalName(localName);
}
    
void BLEPeripheral::setAppearance(const unsigned short appearance){
    uint32_t err_code = sd_ble_gap_appearance_set(appearance);
    if(err_code != 0) SDManager.registerError("BLEPeripheral::setAppearance", err_code, "Unable to set appearance");
    BLEAdvertisement::setAppearance(appearance);
}
    
void BLEPeripheral::setPreferredConnectionParameters(uint16_t minConIntervalMs, uint16_t maxConIntervalMs, uint16_t slaveLatency, uint16_t supervisingTimeoutMs){
    uint32_t                err_code;
    ble_gap_conn_params_t   gapConParams;

    memset(&gapConParams, 0, sizeof(gapConParams));

    gapConParams.min_conn_interval = minConIntervalMs * 4 / 5;
    gapConParams.max_conn_interval = maxConIntervalMs * 4 / 5;
    gapConParams.slave_latency     = slaveLatency;
    gapConParams.conn_sup_timeout  = supervisingTimeoutMs / 10;

    err_code = sd_ble_gap_ppcp_set(&gapConParams);
    if(err_code != 0) SDManager.registerError("BLEPeripheral::setPreferredConnectionParameters", err_code, "Failed to set PPCP");
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
            
            // Forward the connection handle to all the services
            setConHandleInServices(bleEvent->evt.gap_evt.conn_handle);
            
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

            // Restart advertising automatically (consider removing this later, to allow the application to control this)
            sd_ble_gap_adv_start(&_advParams);            
            break;
        
        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
            _lsConParameters = bleEvent->evt.gap_evt.params.conn_param_update.conn_params;
            break;
            
        case BLE_GAP_EVT_TIMEOUT:
            if(_peripheralEventHandlers[BLEPeripheralEventTimeout] != 0) {
                _peripheralEventHandlers[BLEPeripheralEventTimeout](*this);
            }  
            break;
        
        case BLE_GATTS_EVT_WRITE:
            forwardGattsEventWriteToServices((ble_gatts_evt_write_t *)&bleEvent->evt.gatts_evt.params.write);
            break;
        default:
            break;
    }
}
