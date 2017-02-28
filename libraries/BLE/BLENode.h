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

#ifndef _BLE_NODE_H
#define _BLE_NODE_H

#include "Arduino.h"

class BLENode {
	
    friend class BLECentralRole;
	
public:
    bool connected();
    const char* address();
    char* rawAdvPck();
    uint8_t dataLen();
    uint8_t type();
    bool isScanRsp();
    int8_t rssi();
    void getFieldInAdvPck(uint8_t type, char* result, uint8_t& len);
    void printAdvertisement();
	
protected:
    BLENode();
    void setAdvPck(ble_gap_evt_adv_report_t advPck);
	
private:
    void printAdvData();

    unsigned char               _address[6];
    char                        _advPck[31];
    uint8_t                     _dlen;
    uint8_t                     _type;
    bool                        _sRsp;
    int8_t                      _rssi;
    char                        *_typeString[4] = {"Connectable undirected node", "Connectable directed node", "Non connectable undirected node", "Scannable undirected node"};
    char                        *_flagsString[5] = {"LimitedDiscoverable", "GeneralDiscoverable", "BrEdrNotSupported", "LeBrEdrController", "LeBrEdrHost"};
    ble_gap_evt_adv_report_t    _advReport;
};

typedef BLENode BLEPeripheralPeer;

#endif _BLE_NODE_H