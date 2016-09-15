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

#ifndef __BLEUUID_H
#define __BLEUUID_H

#include <stdint.h>
#include <string.h>
#include "ble.h"

#define UUID_SHORT_MSB_INDEX 13
#define UUID_SHORT_LSB_INDEX 12

const uint8_t btSigBaseUuid128[] = 	{00,00,00,00,00,00,0x10,00,0x80,00,00,0x80,0x5F,0x9B,0x34,0xFB};

typedef enum {BLEUuidTypeUnknown = BLE_UUID_TYPE_UNKNOWN, BLEUuidType16Bit = BLE_UUID_TYPE_BLE, BLEUuidType128Bit = BLE_UUID_TYPE_VENDOR_BEGIN} BLEUuidType;

class BLEUuid{
public:
    BLEUuid(void);
    BLEUuid(const char *uuidString);
    BLEUuid(uint16_t shortUuid);
    bool            set(const char *uuidString);
    void            set(uint16_t shortUuid);
    const uint8_t*  getBuffer(void) const { return _uuid128; }
	uint8_t         getType(void) const;
	uint16_t        getAlias(void) const;
	const char*     toString(void) const;
    BLEUuid&        operator=(const BLEUuid &b);
    bool            operator==(const BLEUuid &b) const;
    bool            operator!=(const BLEUuid &b) const;
    bool            register128bitUuid(void);
    
private:
    int             decodeAsciiHex(uint8_t asciiByte) const;
    uint8_t         toAsciiHex(uint8_t value) const;
    uint8_t         _uuidType;
    uint8_t         _uuid128[16];
    static char     _uuidToStringBuf[16*2 + 4 + 1];
};

#endif
