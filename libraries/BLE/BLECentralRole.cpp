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
  _localAttributes(NULL),
  _numLocalAttributes(0),
  _remoteAttributes(NULL),
  _numRemoteAttributes(0),

  _numLocalCharacteristics(0),
  _localCharacteristicInfo(NULL),
  
  _txBufferCount(0),
  _peripheralConnected(0),
  _allowedPeripherals(1),

  _genericAccessService("1800"),
  _deviceNameCharacteristic("2a00", BLERead, 19),
  _appearanceCharacteristic("2a01", BLERead, 2),
  _genericAttributeService("1801"),
  _servicesChangedCharacteristic("2a05", BLEIndicate, 4),

  _remoteGenericAttributeService("1801"),
  _remoteServicesChangedCharacteristic("2a05", BLEIndicate),
  _numRemoteServices(0),
  _remoteServiceInfo(NULL),
  _remoteServiceDiscoveryIndex(0),
  _numRemoteCharacteristics(0),
  _remoteCharacteristicInfo(NULL),
  _remoteRequestInProgress(false),

  _connectionHandle({BLE_CONN_HANDLE_INVALID, BLE_CONN_HANDLE_INVALID, BLE_CONN_HANDLE_INVALID, BLE_CONN_HANDLE_INVALID, BLE_CONN_HANDLE_INVALID, BLE_CONN_HANDLE_INVALID, BLE_CONN_HANDLE_INVALID})
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

void BLECentralRole::initLocalAttributes() {
  this->_localAttributes = (BLELocalAttribute**)malloc(BLELocalAttribute::numAttributes() * sizeof(BLELocalAttribute*));

  this->_localAttributes[0] = &this->_genericAccessService;
  this->_localAttributes[1] = &this->_deviceNameCharacteristic;
  this->_localAttributes[2] = &this->_appearanceCharacteristic;

  this->_localAttributes[3] = &this->_genericAttributeService;
  this->_localAttributes[4] = &this->_servicesChangedCharacteristic;

  this->_numLocalAttributes = 5;
}

void BLECentralRole::addAttribute(BLELocalAttribute& attribute) {
  this->addLocalAttribute(attribute);
}

void BLECentralRole::addLocalAttribute(BLELocalAttribute& localAttribute) {
  if (this->_localAttributes == NULL) {
    this->initLocalAttributes();
  }

  this->_localAttributes[this->_numLocalAttributes] = &localAttribute;
  this->_numLocalAttributes++;
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
  // check if at least one peripheral is connected
  for(int i = 0; i < _allowedPeripherals; i++)
  if(this->_connectionHandle[i] != BLE_CONN_HANDLE_INVALID){
    connected = true;
    break;
  }
  return connected;
}

void BLECentralRole::allowMultilink(uint8_t linksNo){
  if(linksNo > MAX_PERIPHERAL)
    linksNo = MAX_PERIPHERAL;
  _allowedPeripherals = linksNo;
}

void BLECentralRole::begin(){

  for (int i = 0; i < _numLocalAttributes; i++) {
    BLELocalAttribute *localAttribute = _localAttributes[i];

    if (localAttribute->type() == BLETypeCharacteristic) {
      this->_numLocalCharacteristics++;
    }
  }

  this->_numLocalCharacteristics -= 3; // 0x2a00, 0x2a01, 0x2a05

  this->_localCharacteristicInfo = (struct localCharacteristicInfo*)malloc(sizeof(struct localCharacteristicInfo) * this->_numLocalCharacteristics);

  unsigned char localCharacteristicIndex = 0;

  uint16_t handle = 0;
  BLEService *lastService = NULL;

  for (int i = 0; i < _numLocalAttributes; i++) {
    BLELocalAttribute *localAttribute = _localAttributes[i];
    BLEUuid uuid = BLEUuid(localAttribute->uuid());
    const unsigned char* uuidData = uuid.data();
    unsigned char value[255];

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

    if (localAttribute->type() == BLETypeService) {
      BLEService *service = (BLEService *)localAttribute;

      if (strcmp(service->uuid(), "1800") == 0 || strcmp(service->uuid(), "1801") == 0) {
        continue; // skip
      }

      sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &nordicUUID, &handle);

      lastService = service;
    } else if (localAttribute->type() == BLETypeCharacteristic) {
      BLECharacteristic *characteristic = (BLECharacteristic *)localAttribute;

      characteristic->setValueChangeListener(*this);
	  
      if (strcmp(characteristic->uuid(), "2a00") == 0) {
        ble_gap_conn_sec_mode_t secMode;
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&secMode); // no security is needed

      } else if (strcmp(characteristic->uuid(), "2a01") == 0) {
        //do nothing
      } else if (strcmp(characteristic->uuid(), "2a05") == 0) {
        // do nothing
      } else {
        uint8_t properties = characteristic->properties() & 0xfe;
        uint16_t valueLength = characteristic->valueLength();

        this->_localCharacteristicInfo[localCharacteristicIndex].characteristic = characteristic;
        this->_localCharacteristicInfo[localCharacteristicIndex].notifySubscribed = false;
        this->_localCharacteristicInfo[localCharacteristicIndex].indicateSubscribed = false;
        this->_localCharacteristicInfo[localCharacteristicIndex].service = lastService;

        ble_gatts_char_md_t characteristicMetaData;
        ble_gatts_attr_md_t clientCharacteristicConfigurationMetaData;
        ble_gatts_attr_t    characteristicValueAttribute;
        ble_gatts_attr_md_t characteristicValueAttributeMetaData;

        memset(&characteristicMetaData, 0, sizeof(characteristicMetaData));

        memcpy(&characteristicMetaData.char_props, &properties, 1);

        characteristicMetaData.p_char_user_desc  = NULL;
        characteristicMetaData.p_char_pf         = NULL;
        characteristicMetaData.p_user_desc_md    = NULL;
        characteristicMetaData.p_cccd_md         = NULL;
        characteristicMetaData.p_sccd_md         = NULL;

        if (properties & (BLENotify | BLEIndicate)) {
          memset(&clientCharacteristicConfigurationMetaData, 0, sizeof(clientCharacteristicConfigurationMetaData));

          BLE_GAP_CONN_SEC_MODE_SET_OPEN(&clientCharacteristicConfigurationMetaData.read_perm);
          BLE_GAP_CONN_SEC_MODE_SET_OPEN(&clientCharacteristicConfigurationMetaData.write_perm);

          clientCharacteristicConfigurationMetaData.vloc = BLE_GATTS_VLOC_STACK;

          characteristicMetaData.p_cccd_md = &clientCharacteristicConfigurationMetaData;
        }

        memset(&characteristicValueAttributeMetaData, 0, sizeof(characteristicValueAttributeMetaData));

        if (properties & (BLERead | BLENotify | BLEIndicate)) {
          // if (this->_bondStore && !this->_bondStore->hasData()) {
            // if(this->_lesc > 1)
                // BLE_GAP_CONN_SEC_MODE_SET_LESC_ENC_WITH_MITM(&characteristicValueAttributeMetaData.read_perm);
            // else
                // BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&characteristicValueAttributeMetaData.read_perm);
          // } else {
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&characteristicValueAttributeMetaData.read_perm);
          // }
        }

        if (properties & (BLEWriteWithoutResponse | BLEWrite)) {
          // if (this->_bondStore && !this->_bondStore->hasData()) {
            // if(this->_lesc > 1)
                // BLE_GAP_CONN_SEC_MODE_SET_LESC_ENC_WITH_MITM(&characteristicValueAttributeMetaData.write_perm);
            // else
                // BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&characteristicValueAttributeMetaData.write_perm);
          // } else {
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&characteristicValueAttributeMetaData.write_perm);
          // }
        }

        characteristicValueAttributeMetaData.vloc       = BLE_GATTS_VLOC_STACK;
        characteristicValueAttributeMetaData.rd_auth    = 0;
        characteristicValueAttributeMetaData.wr_auth    = 0;
        characteristicValueAttributeMetaData.vlen       = !characteristic->fixedLength();

        for (int j = (i + 1); j < _numLocalAttributes; j++) {
          localAttribute = _localAttributes[j];

          if (localAttribute->type() != BLETypeDescriptor) {
            break;
          }

          BLEDescriptor *descriptor = (BLEDescriptor *)localAttribute;

          if (strcmp(descriptor->uuid(), "2901") == 0) {
            characteristicMetaData.p_char_user_desc        = (uint8_t*)descriptor->value();
            characteristicMetaData.char_user_desc_max_size = descriptor->valueLength();
            characteristicMetaData.char_user_desc_size     = descriptor->valueLength();
          } else if (strcmp(descriptor->uuid(), "2904") == 0) {
            characteristicMetaData.p_char_pf = (ble_gatts_char_pf_t *)descriptor->value();
          }
        }

        memset(&characteristicValueAttribute, 0, sizeof(characteristicValueAttribute));

        characteristicValueAttribute.p_uuid       = &nordicUUID;
        characteristicValueAttribute.p_attr_md    = &characteristicValueAttributeMetaData;
        characteristicValueAttribute.init_len     = valueLength;
        characteristicValueAttribute.init_offs    = 0;
        characteristicValueAttribute.max_len      = characteristic->valueSize();
        characteristicValueAttribute.p_value      = NULL;

        sd_ble_gatts_characteristic_add(BLE_GATT_HANDLE_INVALID, &characteristicMetaData, &characteristicValueAttribute, &this->_localCharacteristicInfo[localCharacteristicIndex].handles);

        if (valueLength) {
          for (int j = 0; j < valueLength; j++) {
            value[j] = (*characteristic)[j];
          }

          ble_gatts_value_t val;

          val.len = valueLength;
          val.offset = 0;
          val.p_value = (uint8_t*)value;
          sd_ble_gatts_value_set(BLE_CONN_HANDLE_INVALID, this->_localCharacteristicInfo[localCharacteristicIndex].handles.value_handle, &val);
		}

        localCharacteristicIndex++;
      }
    } else if (localAttribute->type() == BLETypeDescriptor) {
      BLEDescriptor *descriptor = (BLEDescriptor *)localAttribute;

      if (strcmp(descriptor->uuid(), "2901") == 0 ||
          strcmp(descriptor->uuid(), "2902") == 0 ||
          strcmp(descriptor->uuid(), "2903") == 0 ||
          strcmp(descriptor->uuid(), "2904") == 0) {
        continue; // skip
      }

      uint16_t valueLength = descriptor->valueLength();

      ble_gatts_attr_t descriptorAttribute;
      ble_gatts_attr_md_t descriptorMetaData;

      memset(&descriptorAttribute, 0, sizeof(descriptorAttribute));
      memset(&descriptorMetaData, 0, sizeof(descriptorMetaData));

      descriptorMetaData.vloc = BLE_GATTS_VLOC_STACK;
      descriptorMetaData.vlen = (valueLength == descriptor->valueLength()) ? 0 : 1;

      // if (this->_bondStore && !this->_bondStore->hasData()) {
        // if(this->_lesc > 1)
            // BLE_GAP_CONN_SEC_MODE_SET_LESC_ENC_WITH_MITM(&descriptorMetaData.read_perm);
        // else
            // BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&descriptorMetaData.read_perm);
      // } else {
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&descriptorMetaData.read_perm);
//      }

      descriptorAttribute.p_uuid    = &nordicUUID;
      descriptorAttribute.p_attr_md = &descriptorMetaData;
      descriptorAttribute.init_len  = valueLength;
      descriptorAttribute.max_len   = descriptor->valueLength();
      descriptorAttribute.p_value   = NULL;

      sd_ble_gatts_descriptor_add(BLE_GATT_HANDLE_INVALID, &descriptorAttribute, &handle);

      if (valueLength) {
        for (int j = 0; j < valueLength; j++) {
          value[j] = (*descriptor)[j];
        }

          ble_gatts_value_t val;

          val.len = valueLength;
          val.offset = 0;
          val.p_value = (uint8_t*)value;
          sd_ble_gatts_value_set(BLE_CONN_HANDLE_INVALID, handle, &val);
      }
    }
  }


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
            _tempNode.setAdvPck(bleEvt->evt.gap_evt.params.adv_report);
            eventHandler = _eventHandlers[BLEScanReceived];
            if (eventHandler) {
              eventHandler(_tempNode);
            }
        break;
        case BLE_GAP_EVT_CONNECTED:
            uint8_t index;
            //save the handler in the first free location
            for(index = 0; index < _allowedPeripherals; index++)
              if(this->_connectionHandle[index] == BLE_CONN_HANDLE_INVALID)
                break;
            this->_connectionHandle[index] = bleEvt->evt.gap_evt.conn_handle;
            _node[index] = _tempNode;			

            uint8_t count;
            sd_ble_tx_packet_count_get(this->_connectionHandle[index], &count);
            this->_txBufferCount = count;

            eventHandler = _eventHandlers[BLEConnected];
            if (eventHandler) {
              eventHandler(_node[index]);
            }

            if (this->_numRemoteServices > 0) {
			  sd_ble_gattc_primary_services_discover(this->_connectionHandle[index], 1, NULL);
            }

            _peripheralConnected++;
            if(_peripheralConnected < _allowedPeripherals)
              this->startScan();
        break;
        case BLE_GAP_EVT_DISCONNECTED:
            uint8_t currentPeripheral;
            for(currentPeripheral = 0; currentPeripheral < _allowedPeripherals; currentPeripheral++)
              if(this->_connectionHandle[currentPeripheral] == bleEvt->evt.gap_evt.conn_handle)
                break;
            this->_connectionHandle[currentPeripheral] = BLE_CONN_HANDLE_INVALID;
            _peripheralConnected--;
            this->_txBufferCount = 0;

            eventHandler = _eventHandlers[BLEDisconnected];
            if (eventHandler) {
              eventHandler(_node[currentPeripheral]);
            }
        // clear remote handle info
        for (int i = 0; i < this->_numRemoteServices; i++) {
          memset(&this->_remoteServiceInfo[i].handlesRange, 0, sizeof(this->_remoteServiceInfo[i].handlesRange));
        }

        if(_peripheralConnected == 0){
          for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
            memset(&this->_remoteCharacteristicInfo[i].properties, 0, sizeof(this->_remoteCharacteristicInfo[i].properties));
            this->_remoteCharacteristicInfo[i].valueHandle = 0;
          }
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
        uint8_t pIndex;
        for(pIndex = 0; pIndex < _allowedPeripherals; pIndex++)
          if(this->_connectionHandle[pIndex] == bleEvt->evt.gap_evt.conn_handle)
            break;

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

          sd_ble_gattc_primary_services_discover(this->_connectionHandle[pIndex], startHandle, NULL);
        } else {
          // done discovering services
          for (int i = 0; i < this->_numRemoteServices; i++) {
            if (this->_remoteServiceInfo[i].handlesRange.start_handle != 0 && this->_remoteServiceInfo[i].handlesRange.end_handle != 0) {
              this->_remoteServiceDiscoveryIndex = i;
              sd_ble_gattc_characteristics_discover(this->_connectionHandle[pIndex], &this->_remoteServiceInfo[i].handlesRange);
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
        uint8_t periphIndex;
        for(periphIndex = 0; periphIndex < _allowedPeripherals; periphIndex++)
          if(this->_connectionHandle[periphIndex] == bleEvt->evt.gap_evt.conn_handle)
            break;

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

          sd_ble_gattc_characteristics_discover(this->_connectionHandle[periphIndex], &serviceHandlesRange);
        } else {
          bool discoverCharacteristics = false;

          for (int i = this->_remoteServiceDiscoveryIndex + 1; i < this->_numRemoteServices; i++) {
            if (this->_remoteServiceInfo[i].handlesRange.start_handle != 0 && this->_remoteServiceInfo[i].handlesRange.end_handle != 0) {
              this->_remoteServiceDiscoveryIndex = i;

              sd_ble_gattc_characteristics_discover(this->_connectionHandle[periphIndex], &this->_remoteServiceInfo[i].handlesRange);
              discoverCharacteristics = true;
              break;
            }
          }

          if (!discoverCharacteristics) {
            eventHandler = _eventHandlers[BLERemoteServicesDiscovered];
            if (eventHandler) {
              eventHandler(_node[periphIndex]);
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

          uint8_t j = 0;
          for(; j < _allowedPeripherals; j++)
          if(this->_connectionHandle[j] == bleEvt->evt.gap_evt.conn_handle)
            break;

          for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
            if (this->_remoteCharacteristicInfo[i].valueHandle == handle) {
                this->_remoteCharacteristicInfo[i].characteristic->setValue(_node[j], bleEvt->evt.gattc_evt.params.read_rsp.data, bleEvt->evt.gattc_evt.params.read_rsp.len); 
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
        uint8_t currentPeripheral;
        for(currentPeripheral = 0; currentPeripheral < _allowedPeripherals; currentPeripheral++)
          if(this->_connectionHandle[currentPeripheral] == bleEvt->evt.gap_evt.conn_handle)
            break;

        uint16_t handle = bleEvt->evt.gattc_evt.params.hvx.handle;

        if (bleEvt->evt.gattc_evt.params.hvx.type == BLE_GATT_HVX_INDICATION) {
          sd_ble_gattc_hv_confirm(this->_connectionHandle[currentPeripheral], handle);
        }

        for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
          if (this->_remoteCharacteristicInfo[i].valueHandle == handle) {
                this->_remoteCharacteristicInfo[i].characteristic->setValue(_node[currentPeripheral], bleEvt->evt.gattc_evt.params.read_rsp.data, bleEvt->evt.gattc_evt.params.read_rsp.len); 
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

bool BLECentralRole::updateCharacteristicValue(BLECharacteristic& characteristic) {
  bool success = true;

  for (int i = 0; i < this->_numLocalCharacteristics; i++) {
    struct localCharacteristicInfo* localCharacteristicInfo = &this->_localCharacteristicInfo[i];

    if (localCharacteristicInfo->characteristic == &characteristic) {

      uint16_t valueLength = characteristic.valueLength();

      ble_gatts_value_t val;

      val.len = valueLength;
      val.offset = 0;
      val.p_value = (uint8_t*)characteristic.value();
      sd_ble_gatts_value_set(BLE_CONN_HANDLE_INVALID, localCharacteristicInfo->handles.value_handle, &val);
	

      ble_gatts_hvx_params_t hvxParams;

      memset(&hvxParams, 0, sizeof(hvxParams));

      hvxParams.handle = localCharacteristicInfo->handles.value_handle;
      hvxParams.offset = 0;
      hvxParams.p_data = NULL;
      hvxParams.p_len  = &valueLength;

      if (localCharacteristicInfo->notifySubscribed) {
        if (this->_txBufferCount > 0) {
          this->_txBufferCount--;

          hvxParams.type = BLE_GATT_HVX_NOTIFICATION;

        for(int currentPeripheral = 0; currentPeripheral < _allowedPeripherals; currentPeripheral++)
          if(this->_connectionHandle[currentPeripheral] != BLE_CONN_HANDLE_INVALID)
            sd_ble_gatts_hvx(this->_connectionHandle[currentPeripheral], &hvxParams);
        } else {
          success = false;
        }
      }

      if (localCharacteristicInfo->indicateSubscribed) {
        if (this->_txBufferCount > 0) {
          this->_txBufferCount--;

          hvxParams.type = BLE_GATT_HVX_INDICATION;

        for(int currentPeripheral = 0; currentPeripheral < _allowedPeripherals; currentPeripheral++)
          if(this->_connectionHandle[currentPeripheral] != BLE_CONN_HANDLE_INVALID)
            sd_ble_gatts_hvx(this->_connectionHandle[currentPeripheral], &hvxParams);
        } else {
          success = false;
        }
      }
    }
  }

  return success;
}


bool BLECentralRole::characteristicValueChanged(BLECharacteristic& characteristic) {
  return this->updateCharacteristicValue(characteristic);
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
        for(int currentPeripheral = 0; currentPeripheral < _allowedPeripherals; currentPeripheral++)
          if(this->_connectionHandle[currentPeripheral] != BLE_CONN_HANDLE_INVALID)
            success = (sd_ble_gattc_read(this->_connectionHandle[currentPeripheral], this->_remoteCharacteristicInfo[i].valueHandle, 0) == NRF_SUCCESS);
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

        for(int currentPeripheral = 0; currentPeripheral < _allowedPeripherals; currentPeripheral++)
          if(this->_connectionHandle[currentPeripheral] != BLE_CONN_HANDLE_INVALID)
            success = (sd_ble_gattc_write(this->_connectionHandle[currentPeripheral], &writeParams) == NRF_SUCCESS);
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

        for(int currentPeripheral = 0; currentPeripheral < _allowedPeripherals; currentPeripheral++)
          if(this->_connectionHandle[currentPeripheral] != BLE_CONN_HANDLE_INVALID)
            success = (sd_ble_gattc_write(this->_connectionHandle[currentPeripheral], &writeParams) == NRF_SUCCESS);
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

        for(int currentPeripheral = 0; currentPeripheral < _allowedPeripherals; currentPeripheral++)
          if(this->_connectionHandle[currentPeripheral] != BLE_CONN_HANDLE_INVALID)
            success = (sd_ble_gattc_write(this->_connectionHandle[currentPeripheral], &writeParams) == NRF_SUCCESS);
      }
      break;
    }
  }

  return success;
}