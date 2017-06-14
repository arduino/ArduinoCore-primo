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
  _messageEventHandler(NULL),
  _bleBondStore(),

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
  memset(this->_advData, 0x00, 32);
  memset(this->_scanData, 0x00, 32);
  
  this->_advData[0] = 2;
  this->_advData[1] = 0x01;
  this->_advData[2] = 0x06;
  
  this->_advDataLength = 3;
  
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
  this->_device->begin((unsigned char)this->_advDataLength, (unsigned char *)this->_advData,
                        (unsigned char)this->_scanDataLength, (unsigned char *)this->_scanData,
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
	
	addFieldInPck(type, uuidLength, (unsigned char *)advServiceUuid.data());
}

void BLEPeripheral::setServiceSolicitationUuid(const char* serviceSolicitationUuid) { 
  BLEUuid sSolUuid = BLEUuid(serviceSolicitationUuid);

  uint8_t len = sSolUuid.length();
  uint8_t type = (len > 2) ? 0x15 : 0x14;
 
  addFieldInPck(type, len, (unsigned char *)sSolUuid.data());
}

void BLEPeripheral::setManufacturerData(const unsigned char manufacturerData[], unsigned char manufacturerDataLength) {
  addFieldInPck(0xff, manufacturerDataLength, (unsigned char *) manufacturerData);
}

void BLEPeripheral::setLocalName(const char* localName) {
  uint8_t len = strlen(localName);
  uint8_t type = 0x09;
  if(len > BLE_SCAN_DATA_MAX_VALUE_LENGTH){
	  len = BLE_SCAN_DATA_MAX_VALUE_LENGTH;
	  type = 0x08;
  }
  addFieldInPck(type, len, (unsigned char *)localName);
}

void BLEPeripheral::setConnectable(bool connectable) {
  this->_device->setConnectable(connectable);
}

bool  BLEPeripheral::setTxPower(int txPower) {
  addFieldInPck(0x0A, 1, (unsigned char *)&txPower);
  return this->_device->setTxPower(txPower);
}

void BLEPeripheral::setBondStore(BLEBondStore& bondStore) {
  this->_device->setBondStore(bondStore);
}

void BLEPeripheral::enableBond(BLEBondingType type){
  switch(type){
    case DISPLAY_PASSKEY:
      this->_device->_mitm = true;
      this->_device->_io_caps = BLE_GAP_IO_CAPS_DISPLAY_ONLY;
      setSecParams(1, 1, 0, BLE_GAP_IO_CAPS_DISPLAY_ONLY);
    break;
    case CONFIRM_PASSKEY:
      this->_device->_mitm = true;
      this->_device->_io_caps = BLE_GAP_IO_CAPS_KEYBOARD_ONLY;
      setSecParams(1, 1, 0, BLE_GAP_IO_CAPS_KEYBOARD_ONLY);
    break;
    default:

    break;
  }
	
}

void BLEPeripheral::clearBondStoreData() {
  // device manager can be uninitialized if this function is
  // called before loop function
  initDM();
  eraseBond();
}

char * BLEPeripheral::getPasskey(){
  if(this->_device->_passkey[0] != 0)
    return (char *)this->_device->_passkey;
}

void BLEPeripheral::sendPasskey(char passkey[]){
  this->_device->sendPasskey(passkey);
}

void BLEPeripheral::confirmPasskey(bool confirm){
  this->_device->confirmPasskey(confirm);
}

void BLEPeripheral::setDeviceName(const char* deviceName) {
  this->_deviceNameCharacteristic.setValue(deviceName);
}

void BLEPeripheral::setAppearance(unsigned short appearance) {
  addFieldInPck(0x19, 2, (unsigned char *)&appearance);
  this->_appearanceCharacteristic.setValue((unsigned char *)&appearance, sizeof(appearance));
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

BLEStatus BLEPeripheral::status(){
  return (BLEStatus)this->_device->_status;
}

void BLEPeripheral::printBleMessage(int eventCode, int messageCode){
  if(eventCode > sizeof(this->_evt_code_to_string))
    return;
  Serial.print(_evt_code_to_string[eventCode]);
  switch(eventCode){
    case 0:
      Serial.println(_hci_messages[messageCode]);
      break;
    case 1:
      Serial.println(_gap_sec_status[messageCode]);
      break;
    case 2:
    case 3:
    case 4:
    case 5:
      if(messageCode == 0 || messageCode == 1)
        Serial.println(_gatt_status[messageCode]);
      else if(messageCode == 0x0180)
        Serial.println(_gatt_status[20]);
      else if(messageCode == 0x019F)
        Serial.println(_gatt_status[21]);
      else if(messageCode >= 0x01FD)
        Serial.println(_gatt_status[messageCode - 487]);
      else
        Serial.println(_gatt_status[(messageCode & 0x00FF) + 2]);
    break;
    default:
      Serial.println("Unknown event code");
    break;
  }
}

void BLEPeripheral::setEventHandler(BLEPeripheralEvent event, BLEPeripheralEventHandler eventHandler) {
  if (event < sizeof(this->_eventHandlers)) {
    this->_eventHandlers[event] = eventHandler;
  }
}

void BLEPeripheral::setEventHandler(BLEPeripheralEvent event, BLEMessageEventHandler eventHandler){
  // only allow BLEMessage event to have a different kind of handler function
  if(event == BLEMessage)
    this->_messageEventHandler = eventHandler;
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

void BLEPeripheral::BLEDevicePasskeyReceived(BLEDevice& /*device*/){
  BLEPeripheralEventHandler eventHandler = this->_eventHandlers[BLEPasskeyReceived];
  if (eventHandler) {
    eventHandler(this->_central);
  }
}

void BLEPeripheral::BLEDevicePasskeyRequested(BLEDevice& /*device*/){
  BLEPeripheralEventHandler eventHandler = this->_eventHandlers[BLEPasskeyRequested];
  if (eventHandler) {
    eventHandler(this->_central);
  }
}

void BLEPeripheral::BLEMessageReceived(BLEDevice& /*device*/, int eventCode, int messageCode){
  if(this->_messageEventHandler != NULL)
    _messageEventHandler(eventCode, messageCode);
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

void BLEPeripheral::addFieldInPck(uint8_t type, uint8_t len, unsigned char* data){
	uint8_t *packet;
	uint8_t *pckLen;
	bool inPck = false;
	uint8_t pos = 0;

	//check if there is room enough in one of the two packets
	if(this->_advDataLength < BLE_ADVERTISEMENT_DATA_MAX_VALUE_LENGTH + 2 && ((BLE_ADVERTISEMENT_DATA_MAX_VALUE_LENGTH + 2) - this->_advDataLength) >= len){
		packet = (uint8_t *)_advData;
		pckLen = &_advDataLength;
	}
	else if(this->_scanDataLength < BLE_SCAN_DATA_MAX_VALUE_LENGTH && (BLE_SCAN_DATA_MAX_VALUE_LENGTH - this->_scanDataLength) >= len){
		packet = (uint8_t *)_scanData;
		pckLen = &_scanDataLength;
	}
	// if the room is not enough return
	else
		return;
	
	//check if the field is already in the packet
	for(pos = 0; pos < BLE_GAP_ADV_MAX_SIZE && packet[pos] != 0; pos += packet[pos] + 1)
		if(packet[pos+1] == type){
			inPck = true;
			break;
		}
	if(inPck){
		//in case of fixed length types, do a simple update of the existing values
		if(type == 0x0A || type == 0x19)
			memcpy(&packet[pos + 2], data, len);
		//otherwise append the value at the end of the list
		else{
			//move fields if necessary to make room for the extension
			for(int i = *pckLen + len - 1; i >= (pos + packet[pos] + 1 + len); i--){
				packet[i] = packet[i - len];
			}
			//copy the new data into the field and update the length byte
			memcpy(&packet[pos + packet[pos] + 1], data, len);
			packet[pos] += len;
			*pckLen += len;
		}
	}
	else{
		//add the packet
		packet[*pckLen] = len + 1;
		packet[*pckLen + 1] = type;
		
		memcpy(&packet[*pckLen + 2], data, len);
		
		*pckLen += len + 2;
	}
}

void BLEPeripheral::callEvtListener(uint32_t type, uint32_t code){
	if(type == 1) // BLEBonded event
		this->BLEDeviceBonded(*this->_device);
	else if(type == 2) // BLEMessage event
		this->BLEMessageReceived(*this->_device, AUTH_STATUS, code);
}