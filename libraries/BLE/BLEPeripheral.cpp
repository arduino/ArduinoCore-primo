// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Modified by Arduino.org development team

#include "BLEUuid.h"

#include "BLEDeviceLimits.h"
#include "BLEUtil.h"
#include "BLEManager.h"

#include "BLEPeripheral.h"

//#define BLE_PERIPHERAL_DEBUG

#define DEFAULT_DEVICE_NAME "Arduino"
#define DEFAULT_APPEARANCE  0x0000

BLEPeripheral::BLEPeripheral(unsigned char req, unsigned char rdy, unsigned char rst) :
#if defined(NRF51) || defined(NRF52) || defined(__RFduino__)
  _nRF51822(),
#else
  _nRF8001(req, rdy, rst),
#endif

  _serviceSolicitationUuid(NULL),
  _manufacturerData(NULL),
  _manufacturerDataLength(0),
  _localName(NULL),

  _localAttributes(NULL),
  _numLocalAttributes(0),
  _remoteAttributes(NULL),
  _numRemoteAttributes(0),

  _genericAccessService("1800"),
  _deviceNameCharacteristic("2a00", BLERead, 19),
  _appearanceCharacteristic("2a01", BLERead, 2),
  _genericAttributeService("1801"),
  _servicesChangedCharacteristic("2a05", BLEIndicate, 4),

  _remoteGenericAttributeService("1801"),
  _remoteServicesChangedCharacteristic("2a05", BLEIndicate),

  _central(this)
{
#if defined(NRF51) || defined(NRF52) || defined(__RFduino__)
  this->_device = &this->_nRF51822;
#else
  this->_device = &this->_nRF8001;
#endif

  memset(this->_eventHandlers, 0x00, sizeof(this->_eventHandlers));
  memset(this->_servicesUuid, 0x00, 32);
  
  this->setDeviceName(DEFAULT_DEVICE_NAME);
  this->setAppearance(DEFAULT_APPEARANCE);

  this->_device->setEventListener(this);
  
  BLEManager.registerPeripheral(this);
}

BLEPeripheral::~BLEPeripheral() {
  this->end();

  if (this->_remoteAttributes) {
    free(this->_remoteAttributes);
  }

  if (this->_localAttributes) {
    free(this->_localAttributes);
  }
}


void BLEPeripheral::begin() {
  unsigned char advertisementDataSize = 0;
  unsigned char scanDataSize = 0;

  BLEEirData advertisementData[6];
  BLEEirData scanData[3];

  unsigned char remainingAdvertisementDataLength = BLE_ADVERTISEMENT_DATA_MAX_VALUE_LENGTH + 2;
  unsigned char remainingScanDataLength = BLE_SCAN_DATA_MAX_VALUE_LENGTH + 2;
  
  if (this->_serviceSolicitationUuid){
    BLEUuid serviceSolicitationUuid = BLEUuid(this->_serviceSolicitationUuid);

    unsigned char uuidLength = serviceSolicitationUuid.length();
    advertisementData[advertisementDataSize].length = uuidLength;
    advertisementData[advertisementDataSize].type = (uuidLength > 2) ? 0x15 : 0x14;

    memcpy(advertisementData[advertisementDataSize].data, serviceSolicitationUuid.data(), uuidLength);
    advertisementDataSize += 1;
    remainingAdvertisementDataLength -= uuidLength + 2;
  }
  if (this->_servicesUuid[0]){
	  if (this->_servicesUuidLength  <= remainingAdvertisementDataLength) {
	  //add the first type found in the array (16 or 128 bit)
	  unsigned char len = this->_servicesUuid[0];
	  unsigned char lenSecondType = 0;
	  unsigned char type = this->_servicesUuid[1];
	  
      advertisementData[advertisementDataSize].length = len;
      advertisementData[advertisementDataSize].type = type;

      memcpy(advertisementData[advertisementDataSize].data, &this->_servicesUuid[2], len);
      advertisementDataSize += 1;	
	  remainingAdvertisementDataLength -= len + 2;
	
	  //add the other type if present
	  if(this->_servicesUuid[len + 2] != 0 && len < (BLE_ADVERTISEMENT_DATA_MAX_VALUE_LENGTH + 2)){
		  lenSecondType = this->_servicesUuid[len + 2];
		  type = this->_servicesUuid[len + 3];
		  
		  advertisementData[advertisementDataSize].length = lenSecondType;
          advertisementData[advertisementDataSize].type = type;
      
	      memcpy(advertisementData[advertisementDataSize].data, &this->_servicesUuid[len + 4], lenSecondType);
          advertisementDataSize += 1;
		  remainingAdvertisementDataLength -= lenSecondType + 2;
	  }
    }
  }
  if (this->_manufacturerData && this->_manufacturerDataLength > 0) {
    if (remainingAdvertisementDataLength >= 3) {
      unsigned char dataLength = this->_manufacturerDataLength;

      if (dataLength + 2 > remainingAdvertisementDataLength) {
        dataLength = remainingAdvertisementDataLength - 2;
      }

      advertisementData[advertisementDataSize].length = dataLength;
      advertisementData[advertisementDataSize].type = 0xff;

      memcpy(advertisementData[advertisementDataSize].data, this->_manufacturerData, dataLength);
      advertisementDataSize += 1;
      remainingAdvertisementDataLength -= dataLength + 2;
    }
  }

  if (this->_localName){
    unsigned char localNameLength = strlen(this->_localName);
    scanData[scanDataSize].length = localNameLength;

    if (scanData[scanDataSize].length > BLE_SCAN_DATA_MAX_VALUE_LENGTH) {
      scanData[scanDataSize].length = BLE_SCAN_DATA_MAX_VALUE_LENGTH;
    }

    scanData[scanDataSize].type = (localNameLength > scanData[scanDataSize].length) ? 0x08 : 0x09;

    memcpy(scanData[scanDataSize].data, this->_localName, scanData[scanDataSize].length);
	
    remainingScanDataLength -= scanData[scanDataSize].length + 2;
	scanDataSize += 1;
  }
  
  if(this->_appearance){
	  if(remainingAdvertisementDataLength >= 4){
		  advertisementData[advertisementDataSize].length = 2;
		  advertisementData[advertisementDataSize].type = 0x19;
		  memcpy(advertisementData[advertisementDataSize].data, &this->_appearance, 2);
		  remainingAdvertisementDataLength -= 4;
		  advertisementDataSize += 1;
	  }
	  // if there is no room in advertisement data packet try to add the field in the scan response packet
	  else
		  if(remainingScanDataLength >= 4){
			  scanData[scanDataSize].length = 2;
			  scanData[scanDataSize].type = 0x19;
			  memcpy(scanData[scanDataSize].data, &this->_appearance, 2);
			  remainingScanDataLength -= 4;
			  scanDataSize += 1;
		  }
	  }
	  
  if(this->_txPower){
	  if(remainingAdvertisementDataLength >= 3){
		  advertisementData[advertisementDataSize].length = 1;
		  advertisementData[advertisementDataSize].type = 0x0A;
		  memcpy(advertisementData[advertisementDataSize].data, &this->_txPower, 1);
		  remainingAdvertisementDataLength -= 3;
		  advertisementDataSize += 1;
	  }
   // if there is no room in advertisement data packet try to add the field in the scan response packet
   else
	  if(remainingScanDataLength >= 3){
		  scanData[scanDataSize].length = 1;
		  scanData[scanDataSize].type = 0x0A;
		  memcpy(scanData[scanDataSize].data, &this->_txPower, 1);
		  remainingScanDataLength -= 2;
		  scanDataSize += 1;
	  }	  
  }

  if (this->_localAttributes == NULL) {
    this->initLocalAttributes();
  }

  for (int i = 0; i < this->_numLocalAttributes; i++) {
    BLELocalAttribute* localAttribute = this->_localAttributes[i];
    if (localAttribute->type() == BLETypeCharacteristic) {
      BLECharacteristic* characteristic = (BLECharacteristic*)localAttribute;

      characteristic->setValueChangeListener(*this);
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
  this->_device->begin(advertisementDataSize, advertisementData,
                        scanDataSize, scanData,
                        this->_localAttributes, this->_numLocalAttributes,
                        this->_remoteAttributes, this->_numRemoteAttributes);

  this->_device->requestAddress();
}

void BLEPeripheral::poll(ble_evt_t *bleEvent) {
  this->_device->poll(bleEvent);
}

void BLEPeripheral::end() {
  this->_device->end();
}

void BLEPeripheral::setAdvertisedServiceUuid(const char* advertisedServiceUuid) {
	
    BLEUuid advServiceUuid = BLEUuid(advertisedServiceUuid);
	unsigned char uuidLength = advServiceUuid.length();
	uint8_t type = uuidLength > 2 ? 0x06 : 0x02;
	uint8_t pos;
	bool inPck = false;
		
	//if maximum size is reached or the room is not enough return
	if(this->_servicesUuidLength >= BLE_ADVERTISEMENT_DATA_MAX_VALUE_LENGTH + 2 || ((BLE_ADVERTISEMENT_DATA_MAX_VALUE_LENGTH + 2) - this->_servicesUuidLength) < uuidLength)
		return;
		
	//check if the field is already in the packet
	for(pos = 0; pos < 32 && this->_servicesUuid[pos] != 0; pos += this->_servicesUuid[pos] + 2)
		if(this->_servicesUuid[pos+1] == type){
			inPck = true;
			break;
		}
	if(inPck){
		//move fields if necessary to make room for the extension
		for(int i = this->_servicesUuidLength + uuidLength - 1; i >= (pos + this->_servicesUuid[pos] + 1 + uuidLength); i--){
			this->_servicesUuid[i] = this->_servicesUuid[i - uuidLength];
		}
		//copy the new data into the field and update the length byte
		memcpy(&this->_servicesUuid[pos + this->_servicesUuid[pos] +2], advServiceUuid.data(), uuidLength);
		this->_servicesUuid[pos] += uuidLength;
		this->_servicesUuidLength += uuidLength;
	}
	else{
		//add the packet
		this->_servicesUuid[this->_servicesUuidLength] = uuidLength;
		this->_servicesUuid[this->_servicesUuidLength + 1] = type;
		
		memcpy(&this->_servicesUuid[this->_servicesUuidLength + 2], advServiceUuid.data(), uuidLength);
		
		this->_servicesUuidLength += uuidLength + 2;
	}
}

void BLEPeripheral::setServiceSolicitationUuid(const char* serviceSolicitationUuid) {
  this->_serviceSolicitationUuid = serviceSolicitationUuid;
}

void BLEPeripheral::setManufacturerData(const unsigned char manufacturerData[], unsigned char manufacturerDataLength) {
  this->_manufacturerData = manufacturerData;
  this->_manufacturerDataLength = manufacturerDataLength;
}

void BLEPeripheral::setLocalName(const char* localName) {
  this->_localName = localName;
}

void BLEPeripheral::setConnectable(bool connectable) {
  this->_device->setConnectable(connectable);
}

bool  BLEPeripheral::setTxPower(int txPower) {
  this->_txPower = txPower;
  return this->_device->setTxPower(txPower);
}

void BLEPeripheral::setBondStore(BLEBondStore& bondStore) {
  this->_device->setBondStore(bondStore);
}

void BLEPeripheral::setDeviceName(const char* deviceName) {
  this->_deviceNameCharacteristic.setValue(deviceName);
}

void BLEPeripheral::setAppearance(unsigned short appearance) {
  this->_appearanceCharacteristic.setValue((unsigned char *)&appearance, sizeof(appearance));
  _appearance=appearance;
}

void BLEPeripheral::addAttribute(BLELocalAttribute& attribute) {
  this->addLocalAttribute(attribute);
}

void BLEPeripheral::addLocalAttribute(BLELocalAttribute& localAttribute) {
  if (this->_localAttributes == NULL) {
    this->initLocalAttributes();
  }

  this->_localAttributes[this->_numLocalAttributes] = &localAttribute;
  this->_numLocalAttributes++;
}

void BLEPeripheral::addRemoteAttribute(BLERemoteAttribute& remoteAttribute) {
  if (this->_remoteAttributes == NULL) {
    this->_remoteAttributes = (BLERemoteAttribute**)malloc(BLERemoteAttribute::numAttributes() * sizeof(BLERemoteAttribute*));
  }

  this->_remoteAttributes[this->_numRemoteAttributes] = &remoteAttribute;
  this->_numRemoteAttributes++;
}

void BLEPeripheral::setAdvertisingInterval(unsigned short advertisingInterval) {
  this->_device->setAdvertisingInterval(advertisingInterval);
}

void BLEPeripheral::setConnectionInterval(unsigned short minimumConnectionInterval, unsigned short maximumConnectionInterval) {
  this->_device->setConnectionInterval(minimumConnectionInterval, maximumConnectionInterval);
}

void BLEPeripheral::disconnect() {
  this->_device->disconnect();
}

BLECentral BLEPeripheral::central() {
  this->poll();

  return this->_central;
}

bool BLEPeripheral::connected() {
  this->poll();

  return this->_central;
}

void BLEPeripheral::setEventHandler(BLEPeripheralEvent event, BLEPeripheralEventHandler eventHandler) {
  if (event < sizeof(this->_eventHandlers)) {
    this->_eventHandlers[event] = eventHandler;
  }
}

bool BLEPeripheral::characteristicValueChanged(BLECharacteristic& characteristic) {
  return this->_device->updateCharacteristicValue(characteristic);
}

bool BLEPeripheral::broadcastCharacteristic(BLECharacteristic& characteristic) {
  return this->_device->broadcastCharacteristic(characteristic);
}

bool BLEPeripheral::canNotifyCharacteristic(BLECharacteristic& characteristic) {
  return this->_device->canNotifyCharacteristic(characteristic);
}

bool BLEPeripheral::canIndicateCharacteristic(BLECharacteristic& characteristic) {
  return this->_device->canIndicateCharacteristic(characteristic);
}

bool BLEPeripheral::canReadRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->canReadRemoteCharacteristic(characteristic);
}

bool BLEPeripheral::readRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->readRemoteCharacteristic(characteristic);
}

bool BLEPeripheral::canWriteRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->canWriteRemoteCharacteristic(characteristic);
}

bool BLEPeripheral::writeRemoteCharacteristic(BLERemoteCharacteristic& characteristic, const unsigned char value[], unsigned char length) {
  return this->_device->writeRemoteCharacteristic(characteristic, value, length);
}

bool BLEPeripheral::canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->canSubscribeRemoteCharacteristic(characteristic);
}

bool BLEPeripheral::subscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->subscribeRemoteCharacteristic(characteristic);
}

bool BLEPeripheral::canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->canUnsubscribeRemoteCharacteristic(characteristic);
}

bool BLEPeripheral::unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->unsubcribeRemoteCharacteristic(characteristic);
}

void BLEPeripheral::BLEDeviceConnected(BLEDevice& /*device*/, const unsigned char* address) {
  this->_central.setAddress(address);

#ifdef BLE_PERIPHERAL_DEBUG
  Serial.print(F("Peripheral connected to central: "));
  Serial.println(this->_central.address());
#endif

  BLEPeripheralEventHandler eventHandler = this->_eventHandlers[BLEConnected];
  if (eventHandler) {
    eventHandler(this->_central);
  }
}

void BLEPeripheral::BLEDeviceDisconnected(BLEDevice& /*device*/) {
#ifdef BLE_PERIPHERAL_DEBUG
  Serial.print(F("Peripheral disconnected from central: "));
  Serial.println(this->_central.address());
#endif

  BLEPeripheralEventHandler eventHandler = this->_eventHandlers[BLEDisconnected];
  if (eventHandler) {
    eventHandler(this->_central);
  }

  this->_central.clearAddress();
}

void BLEPeripheral::BLEDeviceBonded(BLEDevice& /*device*/) {
#ifdef BLE_PERIPHERAL_DEBUG
  Serial.print(F("Peripheral bonded: "));
  Serial.println(this->_central.address());
#endif

  BLEPeripheralEventHandler eventHandler = this->_eventHandlers[BLEBonded];
  if (eventHandler) {
    eventHandler(this->_central);
  }
}

void BLEPeripheral::BLEDeviceRemoteServicesDiscovered(BLEDevice& /*device*/) {
#ifdef BLE_PERIPHERAL_DEBUG
  Serial.print(F("Peripheral discovered central remote services: "));
  Serial.println(this->_central.address());
#endif

  BLEPeripheralEventHandler eventHandler = this->_eventHandlers[BLERemoteServicesDiscovered];
  if (eventHandler) {
    eventHandler(this->_central);
  }
}

void BLEPeripheral::BLEDeviceCharacteristicValueChanged(BLEDevice& /*device*/, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength) {
  characteristic.setValue(this->_central, value, valueLength);
}

void BLEPeripheral::BLEDeviceCharacteristicSubscribedChanged(BLEDevice& /*device*/, BLECharacteristic& characteristic, bool subscribed) {
  characteristic.setSubscribed(this->_central, subscribed);
}

void BLEPeripheral::BLEDeviceRemoteCharacteristicValueChanged(BLEDevice& /*device*/, BLERemoteCharacteristic& remoteCharacteristic, const unsigned char* value, unsigned char valueLength) {
  remoteCharacteristic.setValue(this->_central, value, valueLength);
}

void BLEPeripheral::BLEDeviceAddressReceived(BLEDevice& /*device*/, const unsigned char* /*address*/) {
#ifdef BLE_PERIPHERAL_DEBUG
  char addressStr[18];

  BLEUtil::addressToString(address, addressStr);

  Serial.print(F("Peripheral address: "));
  Serial.println(addressStr);
#endif
}

void BLEPeripheral::BLEDeviceTemperatureReceived(BLEDevice& /*device*/, float /*temperature*/) {
}

void BLEPeripheral::BLEDeviceBatteryLevelReceived(BLEDevice& /*device*/, float /*batteryLevel*/) {
}

void BLEPeripheral::initLocalAttributes() {
  this->_localAttributes = (BLELocalAttribute**)malloc(BLELocalAttribute::numAttributes() * sizeof(BLELocalAttribute*));

  this->_localAttributes[0] = &this->_genericAccessService;
  this->_localAttributes[1] = &this->_deviceNameCharacteristic;
  this->_localAttributes[2] = &this->_appearanceCharacteristic;

  this->_localAttributes[3] = &this->_genericAttributeService;
  this->_localAttributes[4] = &this->_servicesChangedCharacteristic;

  this->_numLocalAttributes = 5;
}
