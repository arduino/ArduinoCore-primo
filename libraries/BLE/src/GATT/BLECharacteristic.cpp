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

#include "BLECharacteristic.h"


BLECharacteristic::BLECharacteristic(const char * uuid, uint8_t properties, uint8_t *data, uint16_t dataLength, bool variableLength){
    memset((void *)_characteristicEventHandlers, 0, sizeof(_characteristicEventHandlers));
    _uuid.set(uuid);
	_properties = properties;
	fillCharStructures(properties, data, dataLength, variableLength);
}

BLECharacteristic::BLECharacteristic(const char * uuid, uint8_t properties){
    memset((void *)_characteristicEventHandlers, 0, sizeof(_characteristicEventHandlers));
	_uuid.set(uuid);
	_properties = properties;
	fillCharStructures(properties, NULL, 0, 0);
}

BLECharacteristic::BLECharacteristic(uint16_t shortUuid, uint8_t properties, uint8_t *data, uint16_t dataLength, bool variableLength){
	memset((void *)_characteristicEventHandlers, 0, sizeof(_characteristicEventHandlers));
    _uuid.set(shortUuid);
	_properties = properties;
	fillCharStructures(properties, data, dataLength, variableLength);
}

BLECharacteristic::BLECharacteristic(uint16_t shortUuid, uint8_t properties){
    memset((void *)_characteristicEventHandlers, 0, sizeof(_characteristicEventHandlers));
    _uuid.set(shortUuid);
	_properties = properties;
	fillCharStructures(properties, NULL, 0, 0);
}

void BLECharacteristic::addDescriptor(BLEDescriptor& descriptor){
	descriptorList.add(&descriptor);
}
		
void BLECharacteristic::setEventHandler(BLECharacteristicEventType event, BLECharacteristicEventHandlerType eventHandler){
	if(event < BLECharEventNUM)
        _characteristicEventHandlers[event] = eventHandler;
    else
        SDManager.registerError("BLECharacteristic::setEventHandler()", 0, "Invalid event type");
}		

void BLECharacteristic::setValueInSD(uint8_t *data_ptr, uint16_t length, BLESetType setType){
	uint32_t err_code;
    if(_added){
        if(setType == NOTIFICATION && _subscribed){
            ble_gatts_hvx_params_t hvx_params;
            memset((void *)&hvx_params, 0, sizeof(hvx_params));
            hvx_params.handle = char_handl.value_handle;
            hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset = 0;
            hvx_params.p_len  = &length;
            hvx_params.p_data = data_ptr;
            err_code = sd_ble_gatts_hvx(parentService->getConHandle(), &hvx_params);
            if(err_code != 0) SDManager.registerError("BLECharacteristic::setValue()", err_code, "Send notification failed");
        }
        else{
            ble_gatts_value_t value = {length, 0, data_ptr};
            err_code = sd_ble_gatts_value_set(BLE_CONN_HANDLE_INVALID, char_handl.value_handle, &value);
            if(err_code != 0) SDManager.registerError("BLECharacteristic::setValue()", err_code, "Set value failed");
        }
	}
	else{
		_setType = setType;
		attr_char_value.init_len = length;
		if(attr_char_value.max_len == 0)
			attr_char_value.max_len  = length;
		attr_char_value.p_value  = data_ptr;
	}    
}

void BLECharacteristic::setValue(uint8_t *data_ptr, uint16_t length, BLESetType setType){
    setValueInSD(data_ptr, length, setType);
}

void BLECharacteristic::setValue(uint8_t *data_ptr){
	setValue(data_ptr, strlen((char *)data_ptr));
}
	
void BLECharacteristic::setValue(uint8_t data_ptr){
	setValue(&data_ptr, 1);
}

void BLECharacteristic::setValue(const char *data_ptr){
	setValue((uint8_t *)data_ptr, strlen(data_ptr));
}

void BLECharacteristic::setMaxLength(uint16_t len){
	attr_char_value.max_len = len;
}

void BLECharacteristic::setVariableLength(bool variableLen){
	attr_md.vlen    = variableLen;
}

uint8_t BLECharacteristic::operator[](int index) const{
    if(index < 0 || index >= attr_char_value.max_len) return 0; 
    return attr_char_value.p_value[index];
}

uint8_t BLECharacteristic::getProperties(void){
	return _properties;
}

BLECharacteristic * BLECharacteristic::getNextElement(){
	return nextElement;
}

void BLECharacteristic::setNextElement(BLECharacteristic * element){
	nextElement = element;
}


void BLECharacteristic::pushCharacteristicToSD(uint16_t service_handle){
    if(_uuid.getType() == BLEUuidType128Bit) 
        if(!_uuid.register128bitUuid())
            SDManager.registerError("BLECharacteristic", 0, "Unable to register 128-bit UUID, limit reached");
	uint32_t err_code = sd_ble_gatts_characteristic_add(service_handle, &char_md, &attr_char_value, &char_handl);
	if(err_code != 0) SDManager.registerError("BLECharacteristic::pushCharacteristicToSD()", err_code, "add characteristic failed");
	
	//add all related descriptor to softdevice
	BLEDescriptor *descriptor=descriptorList.getFirstElement();
	while(descriptor != 0){
		descriptor->pushDescriptorToSD(char_handl.value_handle);
		descriptor=descriptor->getNextElement();
	}
	_added = true;
}

void BLECharacteristic::fillCharStructures(uint8_t properties, uint8_t *data, uint16_t dataLength, bool variableLength){
	memset(&char_md, 0, sizeof(char_md));
	//set characteristic properties
	char_md.char_props.broadcast        = (properties & BLEPropertyBroadcastMask)    ? 1 : 0;
	char_md.char_props.read             = (properties & BLEPropertyReadMask)         ? 1 : 0;
	char_md.char_props.write_wo_resp    = (properties & BLEPropertyWriteCommandMask) ? 1 : 0;
	char_md.char_props.write            = (properties & BLEPropertyWriteMask)        ? 1 : 0;
	char_md.char_props.notify           = (properties & BLEPropertyNotifyMask)       ? 1 : 0;
	char_md.char_props.indicate         = (properties & BLEPropertyIndicateMask)     ? 1 : 0;
	char_md.char_props.auth_signed_wr   = (properties & BLEPropertyAuthSighedWriteMask) ? 1 : 0;
	//TODO: add user descriptor
	char_md.p_char_user_desc = NULL;
	//TODO: add format descriptor
	char_md.p_char_pf = NULL;
	//if notify or indicate properties are set, set the cccd's structure
	if(((properties & BLEPropertyNotifyMask) != 0) || ((properties & BLEPropertyIndicateMask) != 0)){
		memset(&cccd_md, 0, sizeof(cccd_md));
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
		cccd_md.vloc = BLE_GATTS_VLOC_STACK;
		char_md.p_cccd_md = &cccd_md;
	}
	else
		char_md.p_cccd_md = NULL;
	
	char_md.p_sccd_md = NULL;
	
	//set attribute metadata structure
	memset(&attr_md, 0, sizeof(attr_md));
	if(properties & BLEPropertyReadMask)
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	if(properties & BLEPropertyWriteMask)
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
	attr_md.vloc    =  BLE_GATTS_VLOC_STACK;
	attr_md.rd_auth = 0;
	attr_md.vlen    = variableLength;
	if(properties & BLEPropertyWriteCommandMask)
		attr_md.wr_auth = 1;
	
	//set characteristic value structure
	memset(&attr_char_value, 0, sizeof(attr_char_value));
	cUuid.uuid = _uuid.getAlias();
	cUuid.type = _uuid.getType();
	attr_char_value.p_uuid = &cUuid;
	attr_char_value.p_attr_md = &attr_md;
	attr_char_value.init_offs = 0;
	//if data = NULL user will insert data with setValue funciton
	if(data != NULL){
		attr_char_value.init_len = dataLength;
		attr_char_value.max_len = dataLength;
		attr_char_value.p_value = data;
	}
}

void BLECharacteristic::onGattsEventWrite(ble_gatts_evt_write_t *ble_gatts_evt_write){
    if(ble_gatts_evt_write->handle == char_handl.value_handle){
        memcpy(&attr_char_value.p_value[ble_gatts_evt_write->offset], ble_gatts_evt_write->data, ble_gatts_evt_write->len);
        // Characteristic value updated, call event handler
        if(_characteristicEventHandlers[BLECharEventValueChanged] != 0)
            _characteristicEventHandlers[BLECharEventValueChanged](*this);        
    }else if(ble_gatts_evt_write->handle == char_handl.cccd_handle){
        // Update the subscribed variable based on the notification flag in the CCCD
        _subscribed = (ble_gatts_evt_write->data[0] & BLE_GATT_HVX_NOTIFICATION) != 0;
        if(_subscribed){
            if(_characteristicEventHandlers[BLECharEventSubscribed] != 0)
                _characteristicEventHandlers[BLECharEventSubscribed](*this); 
        }
        else{
            if(_characteristicEventHandlers[BLECharEventUnsubscribed] != 0)
                _characteristicEventHandlers[BLECharEventUnsubscribed](*this); 
        }
    }
}
