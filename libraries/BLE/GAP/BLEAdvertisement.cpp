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

void BLEAdvertisement::debugPrintAdvPacket(char *msg){
    static uint8_t tmpBuf[128];
    static const uint8_t hexbuf[]={'0', '1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    for(int i = 0; i < 31; i++){
        tmpBuf[i*3+0] = hexbuf[mAdvPacket[i]>>4];
        tmpBuf[i*3+1] = hexbuf[mAdvPacket[i]%16];
        tmpBuf[i*3+2] = '-';
    }
    tmpBuf[31*3] = 0;
    SDManager.registerError(msg, 0, (char *)tmpBuf);
}

BLEAdvertisement::BLEAdvertisement(void){
	// By default, include the flags type in the advPacket
    mAdvPacket[0] = 2;
    mAdvPacket[1] = BLE_GAP_AD_TYPE_FLAGS;
    mAdvPacket[2] = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
    mAdvPacket[3] = 0;
    
    // By default, the scan response packet is empty
    mScanRspPacket[0] = 0;
    
    // Enable the scan response packet by default
    mScanResponseEnabled = true;
}

bool BLEAdvertisement::setFlags(uint8_t bleAdvFlags){
    if(updateFieldInPackets(BLE_GAP_AD_TYPE_FLAGS, &bleAdvFlags, 1)) return true;
    else {
        SDManager.registerError("BLEAdvertisement::setFlags()", 0, "Unable to add field to advertise data");
        return false;
    }
}

bool BLEAdvertisement::setLocalName(const char* localName){
    if(updateFieldInPackets(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, (const uint8_t *)localName, strlen(localName))) return true;
    else {
        SDManager.registerError("BLEAdvertisement::setLocalName()", 0, "Unable to add field to advertise data");
        return false;
    }   
}

bool BLEAdvertisement::setAppearance(uint16_t appearance){
    if(updateFieldInPackets(BLE_GAP_AD_TYPE_APPEARANCE, (const uint8_t *)&appearance, 2)) return true;
    else {
        SDManager.registerError("BLEAdvertisement::setAppearance()", 0, "Unable to add field to advertise data");
        return false;
    }
}

bool BLEAdvertisement::setAdvertisedServiceData(const char* serviceDataUuid, uint8_t* serviceData, uint8_t serviceDataLength){
	//
}

bool BLEAdvertisement::setAdvertisedServiceUuid(const char* serviceUuid){
	//
}

bool BLEAdvertisement::setAdvertisedServiceUuid(uint16_t shortUuid){
    if(updateFieldInPackets(BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE, (const uint8_t *)&shortUuid, 2)) return true;
    else{
        SDManager.registerError("BLEAdvertisement::setTxPower()", 0, "Unable to add field to advertise data");
        return false;
    }
}
	
bool BLEAdvertisement::setTxPower(int8_t txPower){
    if(updateFieldInPackets(BLE_GAP_AD_TYPE_TX_POWER_LEVEL, (const uint8_t *)&txPower, 1)) return true;
    else{
        SDManager.registerError("BLEAdvertisement::setTxPower()", 0, "Unable to add field to advertise data");
        return false;
    }
}
	
void BLEAdvertisement::enableScanResponse(bool enable){
	mScanResponseEnabled = enable;
}

// Private methods
void BLEAdvertisement::addFieldToPacket(uint8_t *advPacket, uint8_t adType, const uint8_t *data, int dataLength){
    int endOfPacket = lastByteInPacket(advPacket);
    advPacket[endOfPacket] = (uint8_t)dataLength + 1;
    advPacket[endOfPacket + 1] = adType;
    memcpy(&advPacket[endOfPacket + 2], data, dataLength);
    advPacket[endOfPacket + 2 + dataLength] = 0;
}

bool BLEAdvertisement::updateFieldInPackets(uint8_t adType, const uint8_t *data, int dataLength){
    int positionInPacket;
    uint8_t *fieldInPacket;
    uint8_t *sufficientSpaceInPacket;
    // Check if the adType is already present in either the adv or scan response packet
    if((positionInPacket = adTypePresent(mAdvPacket, adType)) >= 0) fieldInPacket = mAdvPacket;
    else if(mScanResponseEnabled && (positionInPacket = adTypePresent(mScanRspPacket, adType)) >= 0) fieldInPacket = mScanRspPacket;
    else{
        // If the adType is not found, check if there is room in either packet to add the field
        fieldInPacket = 0;
        if(availableBytes(mAdvPacket) >= (dataLength + 2)) sufficientSpaceInPacket = mAdvPacket;
        else if(mScanResponseEnabled && availableBytes(mScanRspPacket) >= (dataLength + 2)) sufficientSpaceInPacket = mScanRspPacket;
        else sufficientSpaceInPacket = 0;
    }    
    if(fieldInPacket){
        switch(adType){
            // In case of fixed length types, do a simple update of the existing values
            case BLE_GAP_AD_TYPE_FLAGS:
            case BLE_GAP_AD_TYPE_TX_POWER_LEVEL:
            case BLE_GAP_AD_TYPE_APPEARANCE:
                if(fieldInPacket[positionInPacket] == (dataLength + 1)){
                    memcpy(&fieldInPacket[positionInPacket + 2], data, dataLength);
                    return true;
                }
                break;
            // In the case of list/array types, we have to inject additional values
            case BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE:
                if(availableBytes(fieldInPacket) >= dataLength){
                    
                }
                else return false;
                break;
        }
    }
    else if(sufficientSpaceInPacket){
        switch(adType){
            case BLE_GAP_AD_TYPE_FLAGS:
            case BLE_GAP_AD_TYPE_TX_POWER_LEVEL:
            case BLE_GAP_AD_TYPE_APPEARANCE:
            case BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME:   
            case BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME:
            case BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE:
                addFieldToPacket(sufficientSpaceInPacket, adType, data, dataLength);
                return true;
                break;     
        }            
    }
    return false;
}

int BLEAdvertisement::lastByteInPacket(uint8_t *advPacket){
    int positionInPacket = 0;
    while(advPacket[positionInPacket] != 0 && positionInPacket < BLE_GAP_ADV_MAX_SIZE){
        positionInPacket += (advPacket[positionInPacket] + 1);
    }
    return positionInPacket;     
}

int BLEAdvertisement::bytesFree(uint8_t *advPacket){
    return BLE_GAP_ADV_MAX_SIZE - lastByteInPacket(advPacket);
}

int BLEAdvertisement::availableBytes(uint8_t *advPacket){
    int positionInPacket = 0;
    while(advPacket[positionInPacket] != 0 && positionInPacket < BLE_GAP_ADV_MAX_SIZE){
        positionInPacket += (advPacket[positionInPacket] + 1);
    }
    if(positionInPacket < BLE_GAP_ADV_MAX_SIZE) return BLE_GAP_ADV_MAX_SIZE - positionInPacket;
    else return 0;
}

int BLEAdvertisement::adTypePresent(const uint8_t *advPacket, uint8_t adType){
    int positionInPacket = 0;
    while(advPacket[positionInPacket] != 0 && positionInPacket < BLE_GAP_ADV_MAX_SIZE){
        if(advPacket[positionInPacket + 1] == adType) return positionInPacket;
        positionInPacket += (advPacket[positionInPacket] + 1);
    }
    return -1;
}

void BLEAdvertisement::pushAdvPacketsToSD(void){
    uint32_t errorCode;

    errorCode = sd_ble_gap_adv_data_set(mAdvPacket, (uint8_t)lastByteInPacket(mAdvPacket), mScanRspPacket, (uint8_t)lastByteInPacket(mScanRspPacket));
    if(errorCode != 0) SDManager.registerError("BLEAdvertisement::pushAdvPacketsToSD()", errorCode, "sd_ble_gap_adv_data_set() returned an error");
}
