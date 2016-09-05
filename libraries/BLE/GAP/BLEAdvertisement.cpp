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

#include "SoftDeviceManager.h"
#include "BLEAdvertisement.h"

BLEAdvertisement::BLEAdvertisement(void){
	// By default, include the flags type in the advPacket
    advPacket[0] = BLE_GAP_AD_TYPE_FLAGS;
    advPacket[1] = 2;
    advPacket[2] = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
    advPacket[3] = 0;
    
    // By default, the scan response packet is empty
    scanRspPacket[0] = 0;
    
    // Enable the scan response packet by default
    scanResponseEnabled = true;
}

bool BLEAdvertisement::setFlags(uint32_t bleAdvFlags){
    // Since the flags is always located at the start of the advPacket, we can just add them directly.
	advPacket[2] = (uint8_t)bleAdvFlags;
}

bool BLEAdvertisement::setLocalName(const char* localName){
    if(adTypePresent(advPacket, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME)){
    }
    else{
        int endOfPacket = lastByteInPacket(advPacket);
        if((BLE_GAP_ADV_MAX_SIZE - endOfPacket) >= (strlen(localName) + 2)){
            advPacket[endOfPacket] = BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME;
            advPacket[endOfPacket + 1] = strlen(localName) + 1;
            memcpy(&advPacket[endOfPacket + 2], localName, strlen(localName));
        }
    }
}

bool BLEAdvertisement::setAdvertisedServiceData(const char* serviceDataUuid, uint8_t* serviceData, uint8_t serviceDataLength){
	//
}

bool BLEAdvertisement::setAdvertisedServiceUUID(const char* serviceUuid){
	//
}
	
bool BLEAdvertisement::setTxPower(int8_t txPower){
	//
}
	
void BLEAdvertisement::enableScanResponse(bool enable){
	scanResponseEnabled = enable;
}

// Private methods
int BLEAdvertisement::lastByteInPacket(uint8_t *advPacket){
    int positionInPacket = 0;
    while(advPacket[positionInPacket] != 0 && positionInPacket < BLE_GAP_ADV_MAX_SIZE){
        positionInPacket += advPacket[positionInPacket + 1] + 1;
    }
    return positionInPacket;     
}

int BLEAdvertisement::availableBytes(uint8_t *advPacket){
    int positionInPacket = 0;
    while(advPacket[positionInPacket] != 0 && positionInPacket < BLE_GAP_ADV_MAX_SIZE){
        positionInPacket += advPacket[positionInPacket + 1] + 1;
    }
    if(positionInPacket < BLE_GAP_ADV_MAX_SIZE) return BLE_GAP_ADV_MAX_SIZE - positionInPacket;
    else return 0;
}

bool BLEAdvertisement::adTypePresent(uint8_t *advPacket, uint8_t adType){
    int positionInPacket = 0;
    while(advPacket[positionInPacket] != 0 && positionInPacket < BLE_GAP_ADV_MAX_SIZE){
        if(advPacket[positionInPacket] == adType) return true;
        positionInPacket += advPacket[positionInPacket + 1] + 1;
    }
    return false;
}

void BLEAdvertisement::pushAdvPacketsToSD(void){
    uint32_t errorCode;
    errorCode = sd_ble_gap_adv_data_set(advPacket, (uint8_t)(BLE_GAP_ADV_MAX_SIZE - availableBytes(advPacket)), scanRspPacket, (uint8_t)(BLE_GAP_ADV_MAX_SIZE - availableBytes(scanRspPacket)));
    if(errorCode != 0) SDManager.registerError("BLEAdvertisement.cpp", errorCode, "sd_ble_gap_adv_data_set() return not OK");
}
