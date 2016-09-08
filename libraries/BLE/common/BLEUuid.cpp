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

bool BLEUuid::set(const char *uuidString){
    // Go through the string byte by byte. Decode hex values while ignoring all other symbols.
    int byteIndex = 0, byteValue = 0;
    while(*uuidString && byteIndex < 32){
        if(decodeAsciiHex(*uuidString) >= 0) {
            byteValue = byteValue << 4 | decodeAsciiHex(*uuidString);
            if((byteIndex % 2) == 1) {
                mUuid128[byteIndex / 2] = byteValue;
            }
            byteIndex++;
        }
        uuidString++;
    }
    // Copy the alias into a temporary buffer, and set the alias in mUuid128 to 0 so that we can compare to the BT SIG BASE UUID
    uint8_t aliasBuf[2];
    memcpy(aliasBuf, &mUuid128[2], 2);
    memset(&mUuid128[2], 0, 2);
    // Set the UUID type to 16 or 128 bit depending on the UUID base
    if(memcmp(mUuid128, btSigBaseUuid128, 16) == 0) mUuidType == BLEUuidType16Bit;
    else mUuidType = BLEUuidType128Bit;
    memcpy(&mUuid128[2], aliasBuf, 2);
    // If we haven't reached the end of the string, or we haven't filled all bytes in the UUID buffer, the input UUID was incorrectly formatted
    if(*uuidString != 0 || byteIndex < 32) {
        mUuidType = BLEUuidTypeUnknown;
        return false;
    }
    else return true; 
}

void BLEUuid::set(uint16_t shortUuid){
    mUuidType = BLEUuidType16Bit;
    memcpy(mUuid128, btSigBaseUuid128, 16);
    mUuid128[UUID_SHORT_MSB_INDEX] = (uint8_t)(shortUuid >> 8);
    mUuid128[UUID_SHORT_LSB_INDEX] = (uint8_t)(shortUuid && 0x00FF);   
}

uint16_t BLEUuid::getAlias(void) const{
	return (uint16_t)mUuid128[UUID_SHORT_MSB_INDEX] << 8 | mUuid128[UUID_SHORT_LSB_INDEX];
}

const char *BLEUuid::toString(void) const{
    if(mUuidType == BLEUuidTypeUnknown) return "Unknown UUID!";
	char *string = mUuidToStringBuf;
    for(int i = 0; i < 16; i++){
        *string++ = toAsciiHex(mUuid128[i] >> 4);
        *string++ = toAsciiHex(mUuid128[i] % 16);
        if((i == 3) || (i == 5) || (i == 7) || (i == 9)) *string++ = '-';
    }
    *string = 0;
    return mUuidToStringBuf;
}

BLEUuid &BLEUuid::operator=(const BLEUuid &b){
    memcpy(mUuid128, b.getBuffer(), 16);
    mUuidType = b.getType();
    return *this;
}

bool BLEUuid::operator==(const BLEUuid &b) const{
    switch(mUuidType){
        case BLEUuidTypeUnknown:
            return false;
        case BLEUuidType16Bit:
            return getAlias() == b.getAlias();
        case BLEUuidType128Bit:
            return memcmp(mUuid128, b.getBuffer(), 16) == 0;
    }
}

bool BLEUuid::operator!=(const BLEUuid &b) const{
    switch(mUuidType){
        case BLEUuidTypeUnknown:
            return true;
        case BLEUuidType16Bit:
            return getAlias() != b.getAlias();
        case BLEUuidType128Bit:
            return memcmp(mUuid128, b.getBuffer(), 16) != 0;
    }    
}
    
int BLEUuid::decodeAsciiHex(uint8_t asciiByte) const
{
    if(asciiByte >= '0' && asciiByte <= '9') return asciiByte - '0';
    if(asciiByte >= 'a' && asciiByte <= 'f') return asciiByte - 'a' + 10;
    if(asciiByte >= 'A' && asciiByte <= 'F') return asciiByte - 'A' + 10;
    return -1;
}

uint8_t BLEUuid::toAsciiHex(uint8_t value) const
{
    if(value < 10) return value + '0';
    else if(value < 16) return value - 10 + 'a';
    else return 'X';
}