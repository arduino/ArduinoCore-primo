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

#ifndef __BLE_MANAGER_H
#define __BLE_MANAGER_H

#include "ble.h"
//#include <BLEPeripheral.h>

typedef enum {BLEPeripheralEventConnected, BLEPeripheralEventDisconnected} BLEPeripheralEventType;
typedef void (*BLEPeripheralEventHandler)(void); 

class BLEManager  {
public:
	BLEManager();

    static bool registerPeripheralCallback(BLEPeripheralEventHandler callback);
	
    static void processBleEvents(ble_evt_t *bleEvent);
    
private:
    static BLEPeripheralEventHandler _blePeripheralEventHandlerList[1];
};

#endif
