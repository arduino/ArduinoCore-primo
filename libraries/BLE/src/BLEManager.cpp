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

#include "BLEManager.h"

BLEPeripheral *BLEManagerClass::_peripheralList[1] = {0};

BLEManagerClass::BLEManagerClass(){
   // _peripheralList[0] = 0;
}
	
/*BLEPeripheral &BLEManagerClass::getPeripheral(void){
	//
}

BLECentral &BLEManagerClass::getCentral(void){
	//
}

BLEBroadcaster &BLEManagerClass::getBroadcaster(void){
	//
}

BLEObserver &BLEManagerClass::getObserver(void){
	//
}*/
bool BLEManagerClass::registerPeripheral(BLEPeripheral *peripheral) {
    _peripheralList[0] = peripheral;
}

void BLEManagerClass::processBleEvents(ble_evt_t *bleEvent){
    if(_peripheralList[0] != 0){
        _peripheralList[0]->onBleEvent(bleEvent);
    }
}

void BLEManagerClass::defaultSerialErrorHandler(char *file, uint32_t errCode, char *msg){
    Serial.println("Error!");
    Serial.print("  Module:  ");
    Serial.println(file);
    Serial.print("  ErrCode: ");
    Serial.println(errCode, HEX);
    Serial.print("  Message: ");
    Serial.println(msg);
}
    
void BLEManagerClass::setErrorHandler(void (*errorHandlerCallback)(char *file, uint32_t errCode, char *msg)){
	SDManager.setErrorHandler(errorHandlerCallback);
}

BLEManagerClass BLEManager;	