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
#include "BLEUtil.h"

// #define BLE_CENTRAL_DEBUG

BLECentralRole::BLECentralRole() : 
  _remoteAttributes(NULL),
  _txBufferCount(0),
  _numRemoteAttributes(0),
  _remoteGenericAttributeService("1801"),
  _remoteServicesChangedCharacteristic("2a05", BLEIndicate),
  _numRemoteServices(0),
  _remoteServiceInfo(NULL),
  _remoteServiceDiscoveryIndex(0),
  _numRemoteCharacteristics(0),
  _remoteCharacteristicInfo(NULL),
  _remoteRequestInProgress(false),
  _connectionHandle(BLE_CONN_HANDLE_INVALID)
{
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

void BLECentralRole::addRemoteAttribute(BLERemoteAttribute& remoteAttribute) {
  if (this->_remoteAttributes == NULL) {
    this->_remoteAttributes = (BLERemoteAttribute**)malloc(BLERemoteAttribute::numAttributes() * sizeof(BLERemoteAttribute*));
  }

  this->_remoteAttributes[this->_numRemoteAttributes] = &remoteAttribute;
  this->_numRemoteAttributes++;
}

void BLECentralRole::startScan(){
    memset(&this->_scanParams, 0, sizeof(this->_scanParams));
    this->_scanParams.active   = _activeScan;
    this->_scanParams.interval = _scanInterval;
    this->_scanParams.window   = _scanWindow;
    this->_scanParams.timeout  = _scanTimeout;
    sd_ble_gap_scan_start(&this->_scanParams);
}

void BLECentralRole::stopScan(){
    sd_ble_gap_scan_stop();
}

void BLECentralRole::connect(BLENode& node){
  ble_gap_conn_params_t gap_conn_params;

  gap_conn_params.min_conn_interval = 40;  // in 1.25ms units
  gap_conn_params.max_conn_interval = 80;  // in 1.25ms unit
  gap_conn_params.slave_latency     = 0;
  gap_conn_params.conn_sup_timeout  = 4000 / 10; // in 10ms unit

    uint32_t err_code=sd_ble_gap_connect(&node._advReport.peer_addr, &this->_scanParams, &gap_conn_params);
}

bool BLECentralRole::connected(){
  bool connected = false;
  if(this->_connectionHandle != BLE_CONN_HANDLE_INVALID)
    connected = true;
  return connected;
}

void BLECentralRole::begin(){

  for (int i = 0; i < this->_numRemoteAttributes; i++) {
    BLERemoteAttribute* remoteAttribute = this->_remoteAttributes[i];
    if (remoteAttribute->type() == BLETypeCharacteristic) {
      BLERemoteCharacteristic* remoteCharacteristic = (BLERemoteCharacteristic*)remoteAttribute;

      remoteCharacteristic->setValueChangeListener(*this);
    }
  }

  if (this->_numRemoteAttributes) {
    this->addRemoteAttribute(this->_remoteGenericAttributeService);
    this->addRemoteAttribute(this->_remoteServicesChangedCharacteristic);
  }
  
    if ( this->_numRemoteAttributes > 0) {
    this->_numRemoteAttributes -= 2; // 0x1801, 0x2a05
  }

  for (int i = 0; i < this->_numRemoteAttributes; i++) {
    BLERemoteAttribute *remoteAttribute = this->_remoteAttributes[i];

    if (remoteAttribute->type() == BLETypeService) {
      this->_numRemoteServices++;
    } else if (remoteAttribute->type() == BLETypeCharacteristic) {
      this->_numRemoteCharacteristics++;
    }
  }

  this->_remoteServiceInfo = (struct remoteServiceInfo*)malloc(sizeof(struct remoteServiceInfo) * this->_numRemoteServices);
  this->_remoteCharacteristicInfo = (struct remoteCharacteristicInfo*)malloc(sizeof(struct remoteCharacteristicInfo) * this->_numRemoteCharacteristics);

  BLERemoteService *lastRemoteService = NULL;
  unsigned char remoteServiceIndex = 0;
  unsigned char remoteCharacteristicIndex = 0;

  for (int i = 0; i < this->_numRemoteAttributes; i++) {
    BLERemoteAttribute *remoteAttribute = this->_remoteAttributes[i];
    BLEUuid uuid = BLEUuid(remoteAttribute->uuid());
    const unsigned char* uuidData = uuid.data();

    ble_uuid_t nordicUUID;

  if (uuid.length() == 2) {
    nordicUUID.uuid = (uuidData[1] << 8) | uuidData[0];
    nordicUUID.type = BLE_UUID_TYPE_BLE;
  } else {
    unsigned char uuidDataTemp[16];

    memcpy(&uuidDataTemp, uuidData, sizeof(uuidDataTemp));

    nordicUUID.uuid = (uuidData[13] << 8) | uuidData[12];

    uuidDataTemp[13] = 0;
    uuidDataTemp[12] = 0;

    sd_ble_uuid_vs_add((ble_uuid128_t*)&uuidDataTemp, &nordicUUID.type);
  }

  if (remoteAttribute->type() == BLETypeService) {
    this->_remoteServiceInfo[remoteServiceIndex].service = lastRemoteService = (BLERemoteService *)remoteAttribute;
    this->_remoteServiceInfo[remoteServiceIndex].uuid = nordicUUID;

    memset(&this->_remoteServiceInfo[remoteServiceIndex].handlesRange, 0, sizeof(this->_remoteServiceInfo[remoteServiceIndex].handlesRange));

    remoteServiceIndex++;
  } else if (remoteAttribute->type() == BLETypeCharacteristic) {
      this->_remoteCharacteristicInfo[remoteCharacteristicIndex].characteristic = (BLERemoteCharacteristic *)remoteAttribute;
      this->_remoteCharacteristicInfo[remoteCharacteristicIndex].service = lastRemoteService;
      this->_remoteCharacteristicInfo[remoteCharacteristicIndex].uuid = nordicUUID;

      memset(&this->_remoteCharacteristicInfo[remoteCharacteristicIndex].properties, 0, sizeof(this->_remoteCharacteristicInfo[remoteCharacteristicIndex].properties));
      this->_remoteCharacteristicInfo[remoteCharacteristicIndex].valueHandle = 0;

      remoteCharacteristicIndex++;
    }
  }
  
  this->startScan();
	
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
        case BLE_GAP_EVT_CONNECTED:
            this->_connectionHandle = bleEvt->evt.gap_evt.conn_handle;

            uint8_t count;
            sd_ble_tx_packet_count_get(this->_connectionHandle, &count);
            this->_txBufferCount = count;

            eventHandler = _eventHandlers[BLEConnected];
            if (eventHandler) {
              eventHandler(_node);
            }
            // if (this->_minimumConnectionInterval >= BLE_GAP_CP_MIN_CONN_INTVL_MIN &&
                // this->_maximumConnectionInterval <= BLE_GAP_CP_MAX_CONN_INTVL_MAX) {
              // ble_gap_conn_params_t gap_conn_params;

              // gap_conn_params.min_conn_interval = this->_minimumConnectionInterval;  // in 1.25ms units
              // gap_conn_params.max_conn_interval = this->_maximumConnectionInterval;  // in 1.25ms unit
              // gap_conn_params.slave_latency     = 0;
              // gap_conn_params.conn_sup_timeout  = 4000 / 10; // in 10ms unit

              // sd_ble_gap_conn_param_update(this->_connectionHandle, &gap_conn_params);
            // }
            if (this->_numRemoteServices > 0) {
			  sd_ble_gattc_primary_services_discover(this->_connectionHandle, 1, NULL);
            }
        break;
        case BLE_GAP_EVT_DISCONNECTED:
            this->_connectionHandle = BLE_CONN_HANDLE_INVALID;
            this->_txBufferCount = 0;

            eventHandler = _eventHandlers[BLEDisconnected];
            if (eventHandler) {
              eventHandler(_node);
            }
        // clear remote handle info
        for (int i = 0; i < this->_numRemoteServices; i++) {
          memset(&this->_remoteServiceInfo[i].handlesRange, 0, sizeof(this->_remoteServiceInfo[i].handlesRange));
        }

        for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
          memset(&this->_remoteCharacteristicInfo[i].properties, 0, sizeof(this->_remoteCharacteristicInfo[i].properties));
          this->_remoteCharacteristicInfo[i].valueHandle = 0;
        }

        this->_remoteRequestInProgress = false;
        this->startScan();
        break;

      case BLE_EVT_TX_COMPLETE:
#ifdef BLE_CENTRAL_DEBUG
        Serial.print(F("Evt TX complete "));
        Serial.println(bleEvt->evt.common_evt.params.tx_complete.count);
#endif
        this->_txBufferCount += bleEvt->evt.common_evt.params.tx_complete.count;
      break;

      case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
#ifdef BLE_CENTRAL_DEBUG
        Serial.print(F("Evt Prim Srvc Disc Rsp 0x"));
        Serial.println(bleEvt->evt.gattc_evt.gatt_status, HEX);
#endif
        if (bleEvt->evt.gattc_evt.gatt_status == BLE_GATT_STATUS_SUCCESS) {
          uint16_t count = bleEvt->evt.gattc_evt.params.prim_srvc_disc_rsp.count;
          for (int i = 0; i < count; i++) {
            for (int j = 0; j < this->_numRemoteServices; j++) {
	            if ((bleEvt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[i].uuid.type == this->_remoteServiceInfo[j].uuid.type) &&
                  (bleEvt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[i].uuid.uuid == this->_remoteServiceInfo[j].uuid.uuid)) {
		            this->_remoteServiceInfo[j].handlesRange = bleEvt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[i].handle_range;
                break;
              }
            }
          }

          uint16_t startHandle = bleEvt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[count - 1].handle_range.end_handle + 1;

          sd_ble_gattc_primary_services_discover(this->_connectionHandle, startHandle, NULL);
        } else {
          // done discovering services
          for (int i = 0; i < this->_numRemoteServices; i++) {
            if (this->_remoteServiceInfo[i].handlesRange.start_handle != 0 && this->_remoteServiceInfo[i].handlesRange.end_handle != 0) {
              this->_remoteServiceDiscoveryIndex = i;
              sd_ble_gattc_characteristics_discover(this->_connectionHandle, &this->_remoteServiceInfo[i].handlesRange);
              break;
            }
          }
        }
        break;
		
      case BLE_GATTC_EVT_CHAR_DISC_RSP:
#ifdef BLE_CENTRAL_DEBUG
        Serial.print(F("Evt Char Disc Rsp 0x"));
        Serial.println(bleEvt->evt.gattc_evt.gatt_status, HEX);
#endif
        if (bleEvt->evt.gattc_evt.gatt_status == BLE_GATT_STATUS_SUCCESS) {
          ble_gattc_handle_range_t serviceHandlesRange = this->_remoteServiceInfo[this->_remoteServiceDiscoveryIndex].handlesRange;

          uint16_t count = bleEvt->evt.gattc_evt.params.char_disc_rsp.count;

          for (int i = 0; i < count; i++) {
            for (int j = 0; j < this->_numRemoteCharacteristics; j++) {
              if ((this->_remoteServiceInfo[this->_remoteServiceDiscoveryIndex].service == this->_remoteCharacteristicInfo[j].service) &&
                  (bleEvt->evt.gattc_evt.params.char_disc_rsp.chars[i].uuid.type == this->_remoteCharacteristicInfo[j].uuid.type) &&
                  (bleEvt->evt.gattc_evt.params.char_disc_rsp.chars[i].uuid.uuid == this->_remoteCharacteristicInfo[j].uuid.uuid)) {
                this->_remoteCharacteristicInfo[j].properties = bleEvt->evt.gattc_evt.params.char_disc_rsp.chars[i].char_props;
                this->_remoteCharacteristicInfo[j].valueHandle = bleEvt->evt.gattc_evt.params.char_disc_rsp.chars[i].handle_value;
              }
            }

            serviceHandlesRange.start_handle = bleEvt->evt.gattc_evt.params.char_disc_rsp.chars[i].handle_value;
          }

          sd_ble_gattc_characteristics_discover(this->_connectionHandle, &serviceHandlesRange);
        } else {
          bool discoverCharacteristics = false;

          for (int i = this->_remoteServiceDiscoveryIndex + 1; i < this->_numRemoteServices; i++) {
            if (this->_remoteServiceInfo[i].handlesRange.start_handle != 0 && this->_remoteServiceInfo[i].handlesRange.end_handle != 0) {
              this->_remoteServiceDiscoveryIndex = i;

              sd_ble_gattc_characteristics_discover(this->_connectionHandle, &this->_remoteServiceInfo[i].handlesRange);
              discoverCharacteristics = true;
              break;
            }
          }

          if (!discoverCharacteristics) {
            eventHandler = _eventHandlers[BLERemoteServicesDiscovered];
            if (eventHandler) {
              eventHandler(_node);
            }
          }
        }
        break;
		
      case BLE_GATTC_EVT_READ_RSP: {
#ifdef BLE_CENTRAL_DEBUG
        Serial.print(F("Evt Read Rsp 0x"));
        Serial.println(bleEvt->evt.gattc_evt.gatt_status, HEX);
        Serial.println(bleEvt->evt.gattc_evt.params.read_rsp.handle, DEC);
        BLEUtil::printBuffer(bleEvt->evt.gattc_evt.params.read_rsp.data, bleEvt->evt.gattc_evt.params.read_rsp.len);
#endif
        this->_remoteRequestInProgress = false;

        if (bleEvt->evt.gattc_evt.gatt_status == BLE_GATT_STATUS_ATTERR_INSUF_AUTHENTICATION /*&&
            this->_bondStore*/) {
				//TODO: manage bonding
        } else {
          uint16_t handle = bleEvt->evt.gattc_evt.params.read_rsp.handle;

          for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
            if (this->_remoteCharacteristicInfo[i].valueHandle == handle) {
            // BLERemoteCharacteristicEventHandler * evtHandler = *this->_remoteCharacteristicInfo[i].characteristic->_eventHandlers[BLEValueUpdated]);
            // if(evtHandler)
                this->_remoteCharacteristicInfo[i].characteristic->setValue(_node, bleEvt->evt.gattc_evt.params.read_rsp.data, bleEvt->evt.gattc_evt.params.read_rsp.len); 
              // if (this->_eventListener) {
                // this->_eventListener->BLEDeviceRemoteCharacteristicValueChanged(*this, *this->_remoteCharacteristicInfo[i].characteristic, bleEvt->evt.gattc_evt.params.read_rsp.data, bleEvt->evt.gattc_evt.params.read_rsp. len);
              // }
              break;
            }
          }
        }
        break;
      }
	  
      case BLE_GATTC_EVT_WRITE_RSP:
#ifdef BLE_CENTRAL_DEBUG
        Serial.print(F("Evt Write Rsp 0x"));
        Serial.println(bleEvt->evt.gattc_evt.gatt_status, HEX);
        Serial.println(bleEvt->evt.gattc_evt.params.write_rsp.handle, DEC);
#endif
        this->_remoteRequestInProgress = false;

        if (bleEvt->evt.gattc_evt.gatt_status == BLE_GATT_STATUS_ATTERR_INSUF_AUTHENTICATION /*&&
            this->_bondStore*/) {
				//TODO: manage bonding
        }
        break;

      case BLE_GATTC_EVT_HVX: {
#ifdef BLE_CENTRAL_DEBUG
        Serial.print(F("Evt Hvx 0x"));
        Serial.println(bleEvt->evt.gattc_evt.gatt_status, HEX);
        Serial.println(bleEvt->evt.gattc_evt.params.hvx.handle, DEC);
#endif
        uint16_t handle = bleEvt->evt.gattc_evt.params.hvx.handle;

        if (bleEvt->evt.gattc_evt.params.hvx.type == BLE_GATT_HVX_INDICATION) {
          sd_ble_gattc_hv_confirm(this->_connectionHandle, handle);
        }

        for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
          if (this->_remoteCharacteristicInfo[i].valueHandle == handle) {
            // BLERemoteCharacteristicEventHandler * evtHandler = *this->_remoteCharacteristicInfo[i].characteristic->_eventHandlers[BLEValueUpdated]);
            // if(evtHandler)
                this->_remoteCharacteristicInfo[i].characteristic->setValue(_node, bleEvt->evt.gattc_evt.params.read_rsp.data, bleEvt->evt.gattc_evt.params.read_rsp.len); 
				// BLEPeripheral::BLEDeviceRemoteCharacteristicValueChanged(*this, *this->_remoteCharacteristicInfo[i].characteristic, bleEvt->evt.gattc_evt.params.read_rsp.data, bleEvt->evt.gattc_evt.params.read_rsp. len);
              // this->_eventListener->BLEDeviceRemoteCharacteristicValueChanged(*this, *this->_remoteCharacteristicInfo[i].characteristic, bleEvt->evt.gattc_evt.params.read_rsp.data, bleEvt->evt.gattc_evt.params.read_rsp. len);
            // }
            break;
          }
        }
        break;
      }
	  
      default:
#ifdef BLE_CENTRAL_DEBUG
        Serial.print(F("bleEvt->header.evt_id = 0x"));
        Serial.print(bleEvt->header.evt_id, HEX);
        Serial.print(F(" "));
        Serial.println(bleEvt->header.evt_len);
#endif
      break;
    }
  }
}

void BLECentralRole::setEventHandler(BLEPeripheralEvent event, BLECentralEventHandler eventHandler){
	if (event < sizeof(this->_eventHandlers)) {
    this->_eventHandlers[event] = eventHandler;
  }
}

bool BLECentralRole::canNotifyCharacteristic(BLECharacteristic& /*characteristic*/) {
  return (this->_txBufferCount > 0);
}

bool BLECentralRole::canIndicateCharacteristic(BLECharacteristic& /*characteristic*/) {
  return (this->_txBufferCount > 0);
}

bool BLECentralRole::canReadRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      success = (this->_remoteCharacteristicInfo[i].valueHandle &&
                  this->_remoteCharacteristicInfo[i].properties.read &&
                  !this->_remoteRequestInProgress);
      break;
    }
  }

  return success;
}

bool BLECentralRole::readRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      if (this->_remoteCharacteristicInfo[i].valueHandle && this->_remoteCharacteristicInfo[i].properties.read) {
        this->_remoteRequestInProgress = true;
        success = (sd_ble_gattc_read(this->_connectionHandle, this->_remoteCharacteristicInfo[i].valueHandle, 0) == NRF_SUCCESS);
      }
      break;
    }
  }

  return success;
}

bool BLECentralRole::canWriteRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      if (this->_remoteCharacteristicInfo[i].valueHandle) {
        if (this->_remoteCharacteristicInfo[i].properties.write) {
          success = !this->_remoteRequestInProgress;
        } else if (this->_remoteCharacteristicInfo[i].properties.write_wo_resp) {
          success = (this->_txBufferCount > 0);
        }
      }
      break;
    }
  }

  return success;
}

bool BLECentralRole::writeRemoteCharacteristic(BLERemoteCharacteristic& characteristic, const unsigned char value[], unsigned char length) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      if (this->_remoteCharacteristicInfo[i].valueHandle &&
                  (this->_remoteCharacteristicInfo[i].properties.write_wo_resp || this->_remoteCharacteristicInfo[i].properties.write)) {

        ble_gattc_write_params_t writeParams;

        writeParams.write_op = (this->_remoteCharacteristicInfo[i].properties.write) ? BLE_GATT_OP_WRITE_REQ : BLE_GATT_OP_WRITE_CMD;
        writeParams.handle = this->_remoteCharacteristicInfo[i].valueHandle;
        writeParams.offset = 0;
        writeParams.len = length;
        writeParams.p_value = (uint8_t*)value;

        this->_remoteRequestInProgress = true;

        success = (sd_ble_gattc_write(this->_connectionHandle, &writeParams) == NRF_SUCCESS);
      }
      break;
    }
  }

  return success;
}


bool BLECentralRole::canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      success = (this->_remoteCharacteristicInfo[i].valueHandle &&
                (this->_remoteCharacteristicInfo[i].properties.notify || this->_remoteCharacteristicInfo[i].properties.indicate));
      break;
    }
  }

  return success;
}

bool BLECentralRole::subscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      if (this->_remoteCharacteristicInfo[i].valueHandle &&
                  (this->_remoteCharacteristicInfo[i].properties.notify || this->_remoteCharacteristicInfo[i].properties.indicate)) {

        ble_gattc_write_params_t writeParams;

        uint16_t value = (this->_remoteCharacteristicInfo[i].properties.notify ? 0x0001 : 0x002);

        writeParams.write_op = BLE_GATT_OP_WRITE_REQ;
        writeParams.handle = (this->_remoteCharacteristicInfo[i].valueHandle + 1); // don't discover descriptors for now
        writeParams.offset = 0;
        writeParams.len = sizeof(value);
        writeParams.p_value = (uint8_t*)&value;

        this->_remoteRequestInProgress = true;

        success = (sd_ble_gattc_write(this->_connectionHandle, &writeParams) == NRF_SUCCESS);
      }
      break;
    }
  }

  return success;
}

bool BLECentralRole::canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->canSubscribeRemoteCharacteristic(characteristic);
}

bool BLECentralRole::unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      if (this->_remoteCharacteristicInfo[i].valueHandle &&
                  (this->_remoteCharacteristicInfo[i].properties.notify || this->_remoteCharacteristicInfo[i].properties.indicate)) {

        ble_gattc_write_params_t writeParams;

        uint16_t value = 0x0000;

        writeParams.write_op = BLE_GATT_OP_WRITE_REQ;
        writeParams.handle = (this->_remoteCharacteristicInfo[i].valueHandle + 1); // don't discover descriptors for now
        writeParams.offset = 0;
        writeParams.len = sizeof(value);
        writeParams.p_value = (uint8_t*)&value;

        this->_remoteRequestInProgress = true;

        success = (sd_ble_gattc_write(this->_connectionHandle, &writeParams) == NRF_SUCCESS);
      }
      break;
    }
  }

  return success;
}