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

#include "BLETypedCharacteristic.h"

template<typename T>
BLETypedCharacteristic<T>::BLETypedCharacteristic(const char *uuid, uint8_t properties) :
    BLECharacteristic(uuid, properties, 0, sizeof(T), false){
    attr_char_value.p_value = (uint8_t *)&_value;    
}

template<typename T>
BLETypedCharacteristic<T>::BLETypedCharacteristic(const char *uuid, uint8_t properties, T initData) :
    BLECharacteristic(uuid, properties, 0, sizeof(T), false){
}

template<typename T>
bool BLETypedCharacteristic<T>::setValue(T value, BLESetType setType){
    _value = value;
    setValueInSD((uint8_t *)_value, sizeof(T), setType);
    return true;
}

template<typename T>
T &BLETypedCharacteristic<T>::getValue(void){
    return _value;
}

template<typename T>
void BLETypedCharacteristic<T>::onGattsEventWrite(ble_gatts_evt_write_t *ble_gatts_evt_write){
    BLECharacteristic::onGattsEventWrite(ble_gatts_evt_write);
}