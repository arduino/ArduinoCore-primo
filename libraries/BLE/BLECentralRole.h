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
#include "ble_hci.h"

#define  MAX_PERIPHERAL   7

typedef void (*BLECentralEventHandler)(BLENode& node);

class BLECentralRole : public BLECharacteristicValueChangeListener,
                       public BLERemoteCharacteristicValueChangeListener 
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

    // connection intervals in 1.25 ms increments,
    // must be between  0x0006 (7.5 ms) and 0x0c80 (4 s), values outside of this range will be ignored
    void setConnectionInterval(unsigned short minimumConnectionInterval, unsigned short maximumConnectionInterval);
	
    void addAttribute(BLELocalAttribute& attribute);

    void addLocalAttribute(BLELocalAttribute& localAttribute);

    void addRemoteAttribute(BLERemoteAttribute& remoteAttribute);
	
    void startScan();
	
    void stopScan();
	
    void connect(BLENode& node);
	
    bool connected();
	
    void disconnect();
	
    void allowMultilink(uint8_t linksNo);
	
    BLEStatus status();

    void setBondStore(BLEBondStore& bondStore);
    void enableBond(BLEBondingType type = JUST_WORKS);
    void clearBondStoreData();
    char *getPasskey();
    void sendPasskey(char passkey[]);
    void confirmPasskey(bool confirm);

    void begin();
	
    void poll(ble_evt_t *bleEvt = 0);

    void printBleMessage(int eventCode, int messageCode);	

    void setEventHandler(BLEPeripheralEvent event, BLECentralEventHandler);
    void setEventHandler(BLEPeripheralEvent event, BLEMessageEventHandler eventHandler);

    bool updateCharacteristicValue(BLECharacteristic& characteristic);
    bool characteristicValueChanged(BLECharacteristic& characteristic);
    bool canNotifyCharacteristic(BLECharacteristic& characteristic);
    bool canIndicateCharacteristic(BLECharacteristic& characteristic);

    bool canReadRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool readRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool canWriteRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool writeRemoteCharacteristic(BLERemoteCharacteristic& characteristic, const unsigned char value[], unsigned char length);
    bool canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool subscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
	
private:
    void initLocalAttributes();

    struct localCharacteristicInfo {
      BLECharacteristic* characteristic;
      BLEService* service;

      ble_gatts_char_handles_t handles;
      bool notifySubscribed;
      bool indicateSubscribed;
    };

    struct remoteServiceInfo {
      BLERemoteService* service;

      ble_uuid_t uuid;
      ble_gattc_handle_range_t handlesRange;
    };

    struct remoteCharacteristicInfo {
      BLERemoteCharacteristic* characteristic;
      BLERemoteService* service;

      bool connectionIndex[7];
      ble_uuid_t uuid;
      ble_gatt_char_props_t properties;
      uint16_t valueHandle;
    };

    BLEStatus                         _status;

    short                             _scanInterval = 0x0004;
    short                             _scanWindow   = 0x0004;
    short                             _scanTimeout  = 0;
    bool                              _activeScan   = true;
    //uint8_t                         _selective;

    BLENode                           _node[7];
    BLENode                           _tempNode;
    BLECentralEventHandler            _eventHandlers[7];
    BLEMessageEventHandler            _messageEventHandler;

    BLELocalAttribute**               _localAttributes;
    unsigned char                     _numLocalAttributes;
    BLERemoteAttribute**              _remoteAttributes;
    unsigned char                     _numRemoteAttributes;

    unsigned char                     _numLocalCharacteristics;
    struct localCharacteristicInfo*   _localCharacteristicInfo;

    uint16_t                          _connectionHandle[7];
    uint16_t                          _actualConnHandle;
    ble_gap_scan_params_t             _scanParams;
    uint8_t                           _peripheralConnected;
    uint8_t                           _allowedPeripherals;
    unsigned short                    _minimumConnectionInterval;
    unsigned short                    _maximumConnectionInterval;

    bool                              _bond;
    BLEBondStore                      _bondStore;
    uint8_t                           _bondData[((sizeof(ble_gap_enc_key_t) + 3) / 4) * 4]  __attribute__ ((__aligned__(4)));
    ble_gap_enc_key_t*                _encKey;
    bool                              _mitm;
    uint8_t                           _io_caps;
    uint8_t                           _passkey[6];
    bool                              _userConfirm;
	
    unsigned char                     _txBufferCount;
	
    BLEService                        _genericAccessService;
    BLECharacteristic                 _deviceNameCharacteristic;
    BLECharacteristic                 _appearanceCharacteristic;
    BLEService                        _genericAttributeService;
    BLECharacteristic                 _servicesChangedCharacteristic;
	
    BLERemoteService                  _remoteGenericAttributeService;
    BLERemoteCharacteristic           _remoteServicesChangedCharacteristic;
    unsigned char                     _numRemoteServices;
    struct remoteServiceInfo*         _remoteServiceInfo;
    unsigned char                     _remoteServiceDiscoveryIndex;
    unsigned char                     _numRemoteCharacteristics;
    struct remoteCharacteristicInfo*  _remoteCharacteristicInfo;
    bool                              _remoteRequestInProgress;
	
    char*                          _hci_messages[63] = {"Success", "Unknow btle command", "Unknow connection identifier", "", "", "Authentication failure", "Pin or key missing", "Memory capacity exceeded", "Connection timeout", "", "", "", "Command disallowed", "", "", "", "", "", "Invalid btle command parameters", "Remote user terminated connection", "Remote dev termination due to low resources", "Remote dev termination due to power off", "Local host terminated connection", "", "", "", "Unsupported remote feature", "", "", "", "Invalid lmp parameters", "Unspecified error", "", "", "Lmp response timeout", "", "Lmp pdu not allowed", "", "", "", "Instant passed", "Pairing with unit key unsupported", "Different transaction collision", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "Controller busy", "Connection interval unacceptable", "Directed advertiser timeout", "Connection terminated due mic failure", "Connection failed to be established" };
    char*                          _gap_sec_status[256] = {"Success", "Timeout", "Pdu invalid", "Rfu range1 begin", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "Rfu range1 end", "Passkey entry failed", "Oob not available", "Auth requested", "Confirm value", "Pairing not supported", "Enc key size", "Smp cmd unsupported", "Unspecified", "Repeated attempts", "Invalid params", "Dhkey failure", "Numeric comparison failure", "Bd Edr in prog", "X trans key disallowed", "Rfu range2 begin", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "Rfu range2 end"};
    char*                          _gatt_status[26] = {"Success", "Status unkown", "Att error: invalid error code", "Att error: invalid attribute handle", "Att error: read not permitted", "Att error: write not permitted", "Att error: used att as invalid pdu", "Att error: authenticated link required", "Att error: Used att as request not supported", "Att error: invalid offset", "Att error: used in att as Insufficient authorization", "Att error: used att as prepare queue full", "Att error: Used att as attribute not found", "Att error: attribute cannot be read or written using read/write requests", "Att error: Encryption key size used is insufficient", "Att error: invalid value size", "Att error: very unlikely error", "Att error: encrypted link required", "Att error: attribute type is not a supported grouping attrybute", "Att error: encrypted link required", "Att error: application range begin", "Att error: application range end", "Att common profile and service error: client characteristic configuration descriptor improperly configured", "Att common profile and service error: procedure already in progress", "Att common profile and service error: out of range"};
    char*                          _evt_code_to_string[6] = {"Disconnect reason: ",  "Authentication status error: ", "In services discovery procedure: ", 	"In characteristic discovery procedure: ", "Reading not succeeded: ", "Writing not succeeded: "};
};

#endif //_BLE_CENTRAL_ROLE_H