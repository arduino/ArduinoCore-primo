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

#ifndef _BLE_CENTRAL_ROLE_H
#define _BLE_CENTRAL_ROLE_H

#define MAX_SCAN_INTERVAL 0X4000
#define MIN_SCAN_INTERVAL 0X0004

#include "BLENode.h"
#include "BLEManager.h"


typedef void (*BLECentralEventHandler)(BLENode& node);

class BLECentralRole {
public:
    BLECentralRole();

    void setScanParameters(short scanInterval, short scanWindow, short scanTimeout, bool activeScan);	
	
    // scan intervals in 0.625 ms increments,
    // must be between  0x0004 (2.5 ms) and 0x4000 (10.24 s)
    void setScanInterval(short scanInterval);
	
    // scan windows in 0.625 ms increments,
    // must be between  0x0004 (2.5 ms) and 0x4000 (10.24 s)
    void setScanWindow(short scanWindow);
	
    // scan timeout in 1 s increments,
    // must be between  0x0000 and 0xFFFF. 0X0000 disables timeout
    void setScanTimeout(short scanTimeout);
	
    void setActiveScan(bool activeScan);
	
    void startScan();
	
    void stopScan();
	
    void connect();
	
	void disconnect();
	
	void poll(ble_evt_t *bleEvt = 0);
	
	void setEventHandler(BLEPeripheralEvent event, BLECentralEventHandler);
	
private:
    short              _scanInterval = 0x0004;
    short              _scanWindow   = 0x0004;
    short              _scanTimeout  = 0;
    bool               _activeScan   = true;
    //uint8_t          _selective;

    BLENode       _node;
    BLECentralEventHandler      _eventHandlers[7];

};

#endif //_BLE_CENTRAL_ROLE_H