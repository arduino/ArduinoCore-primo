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

#ifndef __BLESERVICE_H
#define __BLESERVICE_H

#include <stdint.h>
#include "..\common\BLEUuid.h"
#include "BLECharacteristic.h"
#include "..\common\LinkedList.h"

class BLECharacteristic;

class BLEService {
public:	
	BLEService(BLEUuid uuid);
    BLEService(const char *uuidString);
    BLEService(uint16_t shortUuid);
	void addCharacteristic(BLECharacteristic &characteristic);
	BLEUuid getUuid(void);
	
	BLEService *getNextElement(void);
	void setNextElement(BLEService *element);
	LinkedList<BLECharacteristic *> getCharacteristicList(void);
    uint16_t getConHandle(){ return _conHandle; }
    void setConHandle(uint16_t conHandle){ _conHandle = conHandle; }
	void pushServiceToSD(void);
    void onGattsEventWrite(ble_gatts_evt_write_t *ble_gatts_evt_write);
    
private:
	uint16_t        _handle;
    BLEUuid         _uuid;
    uint16_t        _conHandle = BLE_CONN_HANDLE_INVALID;
	BLEService      *_nextElement = 0;
	LinkedList<BLECharacteristic *> _characteristicList;
};

#endif
