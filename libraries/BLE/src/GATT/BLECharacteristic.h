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

#ifndef __BLECHARACTERISTIC_H
#define __BLECHARACTERISTIC_H

#include <stdint.h>
#include "..\common\BLEUuid.h"
#include "BLEProperties.h"
#include "BLEDescriptor.h"
#include "..\common\LinkedList.h"
#include "..\common\LinkedList.cpp"
#include "BLEService.h"

class BLECharacteristic;
class BLEService;

typedef enum {BLECharEventDataReceived, BLECharEventNUM} BLECharacteristicEventType;
typedef void (*BLECharacteristicEventHandlerType)(BLECharacteristic &characteristic);

typedef enum {SET, NOTIFICATION, INDICATION} BLESetType;

class BLECharacteristic {
	public:
		BLECharacteristic(const char *uuid, uint8_t properties, uint8_t *data, uint16_t dataLength, bool variableLength);
		BLECharacteristic(const char *uuid, uint8_t properties);
		BLECharacteristic(uint16_t shortUuid, uint8_t properties, uint8_t *data, uint16_t dataLength, bool variableLength);
		BLECharacteristic(uint16_t shortUuid, uint8_t properties);
		void addDescriptor(BLEDescriptor& descriptor);
		void setEventHandler(BLECharacteristicEventType event, BLECharacteristicEventHandlerType eventHandler);
		void setValue(uint8_t *data_ptr, uint16_t length, BLESetType setType = NOTIFICATION);
		void setValue(uint8_t *data_ptr);
		void setValue(uint8_t data_ptr);
		void setValue(const char *data_ptr);
		uint8_t getProperties(void);
		BLECharacteristic *getNextElement(void);
		void setNextElement(BLECharacteristic *element);
        void setParentService(BLEService *parent){ parentService = parent; }
		void pushCharacteristicToSD(uint16_t service_handle);
		void onGattsEventWrite(ble_gatts_evt_write_t *ble_gatts_evt_write);
        
	private:
		BLEUuid                             _uuid;
		ble_gatts_char_md_t                 char_md;
		ble_gatts_attr_md_t                 attr_md;
		ble_gatts_attr_t                    attr_char_value;
		ble_gatts_char_handles_t            char_handl;
		ble_gatts_attr_md_t                 cccd_md;
		ble_uuid_t                          cUuid;
		uint8_t                             _properties;
        bool                                _added = false;
        bool                                _subscribed = false;
		BLESetType                          _setType;
		BLECharacteristic                   *nextElement = 0;
        BLEService                          *parentService = 0;
		LinkedList<BLEDescriptor *>         descriptorList;
        BLECharacteristicEventHandlerType   _characteristicEventHandlers[BLECharEventNUM];
		
		void fillCharStructures(uint8_t properties, uint8_t *data, uint16_t dataLength, bool variableLength);
};
		
#endif
