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
//
}

bool BLEPeripheral::begin(void){
    
	//Register service
	BLEService * service=serviceList.getFirstElement();
	while(service!=0){ //scan all services
		BLEUuid service_uuid=service->getUuid();
		ble_uuid_t sUuid={service_uuid.getAlias(), service_uuid.getType()};
		uint16_t service_handle;
		uint32_t err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &sUuid, &service_handle);
		if(err_code!=0) SDManager.registerError("BLEPeripheral::begin()", err_code, "add service failed");
	
		//Register all characteristics for the specific service
		BLECharacteristic *characteristic=service->getCharacteristicList().getFirstElement();
		while(characteristic!=0){
			//set characteristic's properties
			ble_gatts_char_md_t char_md;
			uint8_t properties=characteristic->getProperties();
			char_md.char_props.broadcast=properties & 0b00000001;
			char_md.char_props.read=(properties & 0b00000010)>>1;
			char_md.char_props.write_wo_resp=(properties & 0b00000100)>>2;
			char_md.char_props.write=(properties & 0b00001000)>>3;
			char_md.char_props.notify=(properties & 0b00010000)>>4;
			char_md.char_props.indicate=(properties & 0b00100000)>>5;
			char_md.char_props.auth_signed_wr=(properties & 0b01000000)>>6;
			
			//TODO: add user descriptor
			char_md.p_char_user_desc=NULL;
			//TODO: add format descriptor
			char_md.p_char_pf=NULL;
			ble_gatts_attr_md_t cccd_md;
			BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
			BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
			// cccd_md.read_perm  = {1, 1};
			// cccd_md.write_perm = {1, 1};
			cccd_md.vloc=BLE_GATTS_VLOC_STACK;
			char_md.p_cccd_md=&cccd_md;
			char_md.p_sccd_md=NULL;
			
			ble_gatts_attr_md_t attr_md;
			BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
			BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
			// attr_md.read_perm  = {1, 1};
			// attr_md.write_perm = {1, 1};
			attr_md.vloc       =  BLE_GATTS_VLOC_STACK;/*BLE_GATTS_VLOC_USER*/
			attr_md.rd_auth    = 0;
			attr_md.wr_auth    = 0;
			attr_md.vlen       = characteristic->getCharacteristicValue().getValueLength();
			
			ble_uuid_t cUuid={characteristic->getCharacteristicValue().getUuid().getAlias(), characteristic->getCharacteristicValue().getUuid().getType()};
			ble_gatts_attr_t attr_char_value;
			attr_char_value.p_uuid=&cUuid;
			attr_char_value.p_attr_md=&attr_md;
			attr_char_value.init_len=characteristic->getCharacteristicValue().getValueLength();
			attr_char_value.init_offs=0;
			attr_char_value.max_len=4;
			attr_char_value.p_value=characteristic->getCharacteristicValue().getValue();
			
			ble_gatts_char_handles_t char_handl;
			
			err_code=sd_ble_gatts_characteristic_add(service_handle, &char_md, &attr_char_value, &char_handl);
			if(err_code!=0) SDManager.registerError("BLEPeripheral::begin()", err_code, "add characteristic failed");
			 
			 //TODO : Register all descriptors for the specific characteristic
			 //{}
		 	
			 characteristic=characteristic->getNextElement();
			}
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
    advParams.timeout = 180;
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
//
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