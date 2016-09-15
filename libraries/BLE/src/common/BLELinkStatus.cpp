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

#include "BLELinkStatus.h"

bool BLELinkStatus::isConnected(){
	return _lsConnected;
}
	
bool BLELinkStatus::isBonded(){
	return _lsBonded;
}

int8_t BLELinkStatus::getRSSI(){
	return _lsRssi;
}

BLEAddress &BLELinkStatus::getPeerAddress(){
	//
}

uint32_t BLELinkStatus::getConIntervalUs(){
    return _lsConParameters.min_conn_interval * 1250;
}

uint32_t BLELinkStatus::getConSupervisingTimeoutMs(){
    return _lsConParameters.conn_sup_timeout * 10;
}

uint32_t BLELinkStatus::getSlaveLatency(){
    return _lsConParameters.slave_latency;
}

BLESecurityLevel &BLELinkStatus::getSecurityLevel(){
	//
}
