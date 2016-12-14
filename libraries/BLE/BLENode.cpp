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

#include "BLENode.h"
#include "BLEUtil.h"

BLENode::BLENode() : 
_advPck ({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}),
_dlen (0)
{}

void BLENode::setAdvPck(ble_gap_evt_adv_report_t advPck){
	_advReport=advPck;
    for(uint8_t i = 0; i < 6; i++)
        _address[i] = advPck.peer_addr.addr[i];
    _dlen    = advPck.dlen;
    for(uint8_t j = 0; j < _dlen; j++)
        _advPck[j] = advPck.data[j];
    _type    = advPck.type;
    _sRsp    = advPck.scan_rsp;
    _rssi    = advPck.rssi;
}

bool BLENode::connected(){
	//
}

const char* BLENode::address(){
    static char address[18];

    BLEUtil::addressToString(this->_address, address);

    return address;
}

char* BLENode::rawAdvPck(){
	return _advPck;
}

uint8_t BLENode::dataLen(){
    return _dlen;
}

uint8_t BLENode::type(){
    return _type;
}

bool BLENode::isScanRsp(){
    return (bool)_sRsp;
}

int8_t BLENode::rssi(){
    return _rssi;
}

void BLENode::getFieldInAdvPck(uint8_t type, char* result, uint8_t& len){
    len = 0;
    for(int i = 0; i < _dlen; i += _advPck[i] + 1){
        if(_advPck[i + 1] == type){
            memcpy(result, &_advPck[i + 2], _advPck[i] - 1);
            result[_advPck[i]-1] = '\0';
			len =_advPck[i] - 1;
            break;
        }
    }
}

void BLENode::printAdvertisement(){
    if(!_sRsp){
        Serial.println("------------------------------------------------------------");
        Serial.print("Received adverstisement packet from node ");
        Serial.println(this->address());
        Serial.print("RSSI "); Serial.print(_rssi); Serial.print(" (dBm). ");
        Serial.println(_typeString[_type]);
        printAdvData();
    }
    else{
        Serial.print("Scan response received from node ");
        Serial.println(this->address());
        printAdvData();
    }
}

void BLENode::printAdvData(){
    char data[31];
    uint8_t len;

    getFieldInAdvPck(BLE_GAP_AD_TYPE_FLAGS, data, len);
    if(len != 0){
        Serial.print("Flags : ");
        switch(data[0]){
            case 0x01:
                Serial.println(_flagsString[0]);
            break;
            case 0x02:
                Serial.println(_flagsString[1]);
            break;
            case 0x04:
                Serial.println(_flagsString[2]);
            break;
            case 0x05:
                Serial.print(_flagsString[0]);
                Serial.print(", ");
                Serial.println(_flagsString[2]);
            case 0x06:
                Serial.print(_flagsString[1]);
                Serial.print(", ");
                Serial.println(_flagsString[2]);
            break;
            case 0x08:
                Serial.println(_flagsString[3]);
            break;
            case 0x10:
                Serial.println(_flagsString[4]);
            break;
            default:
                Serial.println();
            break;
        }
    }

    getFieldInAdvPck(BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE, data, len);
    if(len != 0){
        Serial.print("Incomplete list of 16-bit Service UUIDs: ");
        for(int i = 0; i < len; i+=2){
            Serial.print(" 0x");
            if(data[i] <= 0x0F)
                Serial.print("0");						
            Serial.print(data[i + 1], HEX);
            if(data[i + 1] <= 0x0F)
                Serial.print("0");						
            Serial.print(data[i], HEX);
        }
    Serial.println();
    }

    getFieldInAdvPck(BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE, data, len);
    if(len != 0){
        Serial.print("Complete list of 16-bit Service UUIDs: ");
        for(int i = 0; i < len; i+=2){
            Serial.print(" 0x");
            if(data[i] <= 0x0F)
                Serial.print("0");						
            Serial.print(data[i+1], HEX);
            if(data[i + 1] <= 0x0F)
                Serial.print("0");						
            Serial.print(data[i], HEX);
        }
        Serial.println();
	}
	
    getFieldInAdvPck(BLE_GAP_AD_TYPE_SOLICITED_SERVICE_UUIDS_16BIT, data, len);
    if(len != 0){
        Serial.print("List of 16-bit Solicited Service UUIDs: ");
        for(int i = 0; i < len; i+=2){
            Serial.print(" 0x");
            if(data[i] <= 0x0F)
                Serial.print("0");						
            Serial.print(data[i + 1], HEX);
            if(data[i + 1] <= 0x0F)
                Serial.print("0");						
            Serial.print(data[i], HEX);
        }
        Serial.println();
	}
	
    getFieldInAdvPck(BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE, data, len);
    if(len != 0){
       Serial.print("Incomplete list of 128-bit Service UUIDs: ");
        for(int j = 15; j >= 0; j--){
            if(data[j] <= 0x0F)
                Serial.print("0");
            Serial.print(data[j], HEX);
            if(j == 6 || j == 8 || j == 10 || j == 12)
                Serial.print("-");	
            }
            Serial.println();
    }

    getFieldInAdvPck(BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE, data, len);
    if(len != 0){
       Serial.print("Complete list of 128-bit Service UUIDs: ");
        for(int j = 15; j >= 0; j--){
            if(data[j] <= 0x0F)
                Serial.print("0");
            Serial.print(data[j], HEX);
            if(j == 6 || j == 8 || j == 10 || j == 12)
                Serial.print("-");	
            }
            Serial.println();
    }
	
    getFieldInAdvPck(BLE_GAP_AD_TYPE_SOLICITED_SERVICE_UUIDS_128BIT, data, len);
    if(len != 0){
       Serial.print("List of 128-bit Solicited Service UUIDs: ");
        for(int j = 15; j >= 0; j--){
            if(data[j] <= 0x0F)
                Serial.print("0");
            Serial.print(data[j], HEX);
            if(j == 6 || j == 8 || j == 10 || j == 12)
                Serial.print("-");	
            }
            Serial.println();
    }
	
    getFieldInAdvPck(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, data, len);
    if(len != 0){
        Serial.print("Short Local Name: ");
        for(int j = 0; j < len; j++)
            Serial.print((char)data[j]);
        Serial.println();
    }
	
    getFieldInAdvPck(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, data, len);
    if(len != 0){
        Serial.print("Complete Local Name: ");
        for(int j = 0; j < len; j++)
            Serial.print((char)data[j]);
        Serial.println();
    }
	
    getFieldInAdvPck(BLE_GAP_AD_TYPE_TX_POWER_LEVEL, data, len);
    if(len != 0){
        Serial.print("Tx Power Level: ");
        Serial.print((int8_t)data[0]);
        Serial.println(" dBm");
    }
	
    getFieldInAdvPck(BLE_GAP_AD_TYPE_APPEARANCE, data, len);
    if(len != 0){
        uint16_t aCode=data[0] | (data[1] << 8);
        Serial.print("Appearance: ["); Serial.print(aCode); Serial.println("] ");
    }

    getFieldInAdvPck(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, data, len);
    if(len != 0){
        Serial.print("Manufacturer data: 0x");
        for(int j = 0; j < len; j++){
            if(data[j] <= 0x0F)
                Serial.print("0");
            Serial.print(data[j], HEX);
        }
        Serial.println();
    }
}
