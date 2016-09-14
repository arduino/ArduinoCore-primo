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

#ifndef __BLELINKSTATUS_H
#define __BLELINKSTATUS_H

#include <stdint.h>
#include "BLEAddress.h"

typedef uint16_t BLESecurityLevel;

class BLELinkStatus {
public:
	bool isConnected();
	bool isBonded();
	int8_t getRSSI();  // NOT IMPLEMENTED
	BLEAddress &getPeerAddress(); // NOT IMPLEMENTED
	uint32_t getConIntervalUs();
    uint32_t getConSupervisingTimeoutMs();
    uint32_t getSlaveLatency();
	BLESecurityLevel &getSecurityLevel(); // NOT IMPLEMENTED
    
protected:
    bool                    _lsConnected = false;
    bool                    _lsBonded = false;
    int8_t                  _lsRssi;
    
    uint16_t                _lsConHandle = BLE_CONN_HANDLE_INVALID;
    ble_gap_addr_t          _lsPeerAddress;
    uint8_t                 _lsConRole;
    ble_gap_conn_params_t   _lsConParameters = {0, 0, 0, 0};
};

#endif
