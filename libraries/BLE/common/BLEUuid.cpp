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

#include "BLEUuid.h"

char BLEUuid::mUuidToStringBuf[16*2 + 4 + 1];

BLEUuid::BLEUuid(void){
    mUuidType = BLEUuidTypeUnknown;
}
    
BLEUuid::BLEUuid(const char * uuidString){
    set(uuidString);
}

BLEUuid::BLEUuid(uint16_t shortUuid){
    set(shortUuid);
}

void BLEUuid::set(const char *uuidString){
    mUuidType = BLEUuidType128Bit;
    int byteIndex = 0, byteValue = 0;
    while(*uuidString && byteIndex < 32){
        if(decodeAsciiHex(*uuidString) < 0) continue;
        byteValue = byteValue * 16 + decodeAsciiHex(*uuidString);
        if((byteIndex % 2) == 1) mUuid128[byteIndex / 2] = byteValue;
        byteIndex++;
        uuidString++;
    }
    return true; // TODO: Return false when the format of uuidString is incorrect
}

void BLEUuid::set(uint16_t shortUuid){
    mUuidType = BLEUuidType16Bit;
    memcpy(mUuid128, btSigBaseUuid128, 16);
    mUuid128[UUID_SHORT_MSB_INDEX] = (uint8_t)(shortUuid >> 8);
    mUuid128[UUID_SHORT_LSB_INDEX] = (uint8_t)(shortUuid && 0x00FF);   
}

BLEUuidType BLEUuid::getType(void){
	return mUuidType;
}

uint16_t BLEUuid::getAlias(void){
	return (uint16_t)mUuid128[UUID_SHORT_MSB_INDEX] << 8 | mUuid128[UUID_SHORT_LSB_INDEX];
}

const char *BLEUuid::toString(void){
	char *string = mUuidToStringBuf;
    for(int i = 0; i < 16; i++){
        *string++ = toAsciiHex(mUuid128[i] >> 4);
        *string++ = toAsciiHex(mUuid128[i] % 16);
        if((i == 3) || (i == 5) || (i == 7) || (i == 9)) *string++ = '-';
    }
    *string = 0;
    return mUuidToStringBuf;
}

int BLEUuid::decodeAsciiHex(uint8_t asciiByte)
{
    if(asciiByte >= '0' && asciiByte <= '9') return asciiByte - '0';
    if(asciiByte >= 'a' && asciiByte <= 'f') return asciiByte - 'a' + 10;
    if(asciiByte >= 'A' && asciiByte <= 'F') return asciiByte - 'A' + 10;
    return -1;
}

uint8_t BLEUuid::toAsciiHex(uint8_t value)
{
    if(value < 10) return value + '0';
    else if(value < 16) return value - 10 + 'a';
    else return 'X';
}