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

#ifndef _BLE_CENTRAL_ROLE_H
#define _BLE_CENTRAL_ROLE_H

#define MAX_SCAN_INTERVAL 0X4000
#define MIN_SCAN_INTERVAL 0X0004

#include "BLENode.h"
#include "BLEManager.h"
#include "BLERemoteAttribute.h"
#include "BLERemoteCharacteristic.h"
#include "BLERemoteService.h"
#include "BLEUuid.h"


typedef void (*BLECentralEventHandler)(BLENode& node);

class BLECentralRole : public BLERemoteCharacteristicValueChangeListener 
{
public:
    BLECentralRole();

    void setScanParameters(short scanInterval, short scanWindow, short scanTimeout, bool activeScan);	
	
    // scan intervals in 0.625 ms increments,
    // must be between  0x0004 (2.5 ms) and 0x4000 (10.24 s)
    void setScanInterval(short scanInterval);
	
    // scan windows in 0.625 ms increments,
    // must be between  0x0004 (2.5 ms) and 0x4000 (10.24 s)
    void setScanWindow(short scanWindow);
	
    // scan timeout in 1 s increments,
    // must be between  0x0000 and 0xFFFF. 0X0000 disables timeout
    void setScanTimeout(short scanTimeout);
	
    void setActiveScan(bool activeScan);
	
    void addRemoteAttribute(BLERemoteAttribute& remoteAttribute);
	
    void startScan();
	
    void stopScan();
	
    void connect(BLENode& node);
	
    bool connected();
	
    void disconnect();
	
    void begin();
	
    void poll(ble_evt_t *bleEvt = 0);
	
    void setEventHandler(BLEPeripheralEvent event, BLECentralEventHandler);
	
    virtual bool canNotifyCharacteristic(BLECharacteristic& characteristic);
    virtual bool canIndicateCharacteristic(BLECharacteristic& characteristic);

    virtual bool canReadRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    virtual bool readRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    virtual bool canWriteRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    virtual bool writeRemoteCharacteristic(BLERemoteCharacteristic& characteristic, const unsigned char value[], unsigned char length);
    virtual bool canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    virtual bool subscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    virtual bool canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    virtual bool unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
	
private:

    struct remoteServiceInfo {
      BLERemoteService* service;

      ble_uuid_t uuid;
      ble_gattc_handle_range_t handlesRange;
    };

    struct remoteCharacteristicInfo {
      BLERemoteCharacteristic* characteristic;
      BLERemoteService* service;

      ble_uuid_t uuid;
      ble_gatt_char_props_t properties;
      uint16_t valueHandle;
    };

    short                             _scanInterval = 0x0004;
    short                             _scanWindow   = 0x0004;
    short                             _scanTimeout  = 0;
    bool                              _activeScan   = true;
    //uint8_t                         _selective;

    BLENode                           _node;
    BLECentralEventHandler            _eventHandlers[7];
    BLERemoteAttribute**              _remoteAttributes;
    unsigned char                     _numRemoteAttributes;
    uint16_t                          _connectionHandle;
    ble_gap_scan_params_t             _scanParams;

    unsigned char                     _txBufferCount;
	
    BLERemoteService                  _remoteGenericAttributeService;
    BLERemoteCharacteristic           _remoteServicesChangedCharacteristic;
    unsigned char                     _numRemoteServices;
    struct remoteServiceInfo*         _remoteServiceInfo;
    unsigned char                     _remoteServiceDiscoveryIndex;
    unsigned char                     _numRemoteCharacteristics;
    struct remoteCharacteristicInfo*  _remoteCharacteristicInfo;
    bool                              _remoteRequestInProgress;

};

#endif //_BLE_CENTRAL_ROLE_H