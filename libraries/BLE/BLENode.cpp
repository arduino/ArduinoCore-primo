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

BLENode::BLENode(){}

void BLENode::setAdvPck(ble_gap_evt_adv_report_t advPck){
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

const uint8_t * BLENode::rawAdvPck(){
    return (uint8_t *)_advPck;
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
	uint16_t aCode;
    for(int i = 0; i < _dlen; i += _advPck[i] + 1){
        switch(_advPck[i+1]){
            case BLE_GAP_AD_TYPE_FLAGS:
                Serial.print("Flags : ");
                switch(_advPck[i+2]){
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
            break;
            case BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE:
            case BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE:
            case BLE_GAP_AD_TYPE_SOLICITED_SERVICE_UUIDS_16BIT:
                if(_advPck[i+1] == BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE)
                    Serial.print("Complete list of 16-bit Service UUIDs:");
                else if(_advPck[i+1] == BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE)
                    Serial.print("Incomplete list of 16-bit Service UUIDs:");
                else
                    Serial.print("List of 16-bit Solicited Serivce UUIDs: ");
                for(int j = 0; j < (_advPck[i] - 1) / 2; j++){
                    Serial.print(" 0x");
                    if(_advPck[i + 3 + (j * 2)] <= 0x0F)
                        Serial.print("0");						
                    Serial.print(_advPck[i + 3 + (j * 2)], HEX);
                    if(_advPck[i + 2 + (j * 2)] <= 0x0F)
                        Serial.print("0");						
                    Serial.print(_advPck[i + 2 + (j * 2)], HEX);
                }
                Serial.println();
            break;
            case BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE:
            case BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE:
            case BLE_GAP_AD_TYPE_SOLICITED_SERVICE_UUIDS_128BIT:
                if(_advPck[i+1] == BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE)			
                    Serial.print("Complete list of 128-bit Service UUIDs: ");			
                else if(_advPck[i+1] == BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE)
                    Serial.print("Incomplete list of 128-bit Service UUIDs: ");
                else
                    Serial.print("List of 128-bit Solicited Service UUIDs");
                for(int j = 15; j >= 0; j--){
                    if(_advPck[i + 2 + j] <= 0x0F)
                        Serial.print("0");
                    Serial.print(_advPck[i + 2 + j], HEX);
                    if(j == 6 || j == 8 || j == 10 || j == 12)
                        Serial.print("-");	
                }
                Serial.println();
            break;
            case BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME:
            case BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME:
                if(_advPck[i+1] == BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME)			
                    Serial.print("Complete Local Name: ");
                else
                    Serial.print("Short Local Name: ");
                for(int j = 0; j < _advPck[i] - 1; j++)
                    Serial.print((char)_advPck[i + 2 + j]);
                Serial.println();
            break;
            case BLE_GAP_AD_TYPE_TX_POWER_LEVEL:
                Serial.print("Tx Power Level: ");
                Serial.print((int8_t)_advPck[i + 2]);
                Serial.println(" dBm");
            break;
            case BLE_GAP_AD_TYPE_APPEARANCE:
                aCode=_advPck[i + 2] | (_advPck[i + 3] << 8);
                Serial.print("Appearance: ["); Serial.print(aCode); Serial.println("] ");
                break;
            case BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA:
                Serial.print("Manufacturer data: 0x");
                for(int j = 0; j < _advPck[i] -1; j++){
                    if(_advPck[i + 2 + j] <= 0x0F)
                        Serial.print("0");
    				Serial.print(_advPck[i + 2 + j], HEX);
                }
                Serial.println();
            break;
        }
    }
}
