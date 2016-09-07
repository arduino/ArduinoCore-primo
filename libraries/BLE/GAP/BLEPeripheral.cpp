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