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

#include "BLECentralRole.h"

BLECentralRole::BLECentralRole(){
	BLEManager.registerCentral(this);
}

void BLECentralRole::setScanParameters(short scanInterval, short scanWindow, short scanTimeout, bool activeScan){
    if(scanInterval < MIN_SCAN_INTERVAL)
        scanInterval = MIN_SCAN_INTERVAL;
    if(scanInterval > MAX_SCAN_INTERVAL)
        scanInterval = MAX_SCAN_INTERVAL;
    if(scanWindow < MIN_SCAN_INTERVAL)
        scanWindow = MIN_SCAN_INTERVAL;
    if(scanWindow > MAX_SCAN_INTERVAL)
        scanWindow = MAX_SCAN_INTERVAL;
    // scan window must be lower or equal than scan interval
    if(scanWindow > scanInterval)
        scanInterval = scanWindow;
	
    _scanInterval = scanInterval;
    _scanWindow   = scanWindow;
    _scanTimeout  = scanTimeout;
    _activeScan   = activeScan;
}

void BLECentralRole::setScanInterval(short scanInterval){
    if(scanInterval < MIN_SCAN_INTERVAL)
        scanInterval = MIN_SCAN_INTERVAL;
    if(scanInterval > MAX_SCAN_INTERVAL)
        scanInterval = MAX_SCAN_INTERVAL;
    // scan window must be lower or equal than scan interval
    if(_scanWindow > scanInterval)
        scanInterval = _scanWindow;
	
    _scanInterval = scanInterval;
}

void BLECentralRole::setScanWindow(short scanWindow){
    if(scanWindow < MIN_SCAN_INTERVAL)
        scanWindow = MIN_SCAN_INTERVAL;
    if(scanWindow > MAX_SCAN_INTERVAL)
        scanWindow = MAX_SCAN_INTERVAL;
    // scan window must be lower or equal than scan interval
	if(scanWindow > _scanInterval)
        _scanInterval = scanWindow;
	
    _scanWindow = scanWindow;
}

void BLECentralRole::setScanTimeout(short scanTimeout){
    _scanTimeout = scanTimeout;
}

void BLECentralRole::setActiveScan(bool activeScan){
    _activeScan = activeScan;
}

void BLECentralRole::startScan(){
    ble_gap_scan_params_t params;
    memset(&params, 0, sizeof(params));
    params.active   = _activeScan;
    params.interval = _scanInterval;
    params.window   = _scanWindow;
    params.timeout  = _scanTimeout;
    sd_ble_gap_scan_start(&params);
}

void BLECentralRole::stopScan(){
    sd_ble_gap_scan_stop();
}

void BLECentralRole::poll(ble_evt_t *bleEvt){
  bool event = true;
  BLECentralEventHandler eventHandler;
  if(!bleEvt){
    uint32_t   evtBuf[BLE_STACK_EVT_MSG_BUF_SIZE] __attribute__ ((__aligned__(BLE_EVTS_PTR_ALIGNMENT)));
    uint16_t   evtLen = sizeof(evtBuf);
    bleEvt = (ble_evt_t*)evtBuf;
    if (sd_ble_evt_get((uint8_t*)evtBuf, &evtLen) != NRF_SUCCESS)
      event = false;
  }
  if(event){
    switch (bleEvt->header.evt_id) {
	  case BLE_GAP_EVT_ADV_REPORT:
		_node.setAdvPck(bleEvt->evt.gap_evt.params.adv_report);
        eventHandler = _eventHandlers[BLEScanReceived];
        if (eventHandler) {
          eventHandler(_node);
		}
		break;
	  default: break;
    }
  }
}

void BLECentralRole::setEventHandler(BLEPeripheralEvent event, BLECentralEventHandler eventHandler){
	if (event < sizeof(this->_eventHandlers)) {
    this->_eventHandlers[event] = eventHandler;
  }
}