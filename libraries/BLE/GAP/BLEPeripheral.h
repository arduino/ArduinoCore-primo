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

#ifndef __BLEPERIPHERAL_H
#define __BLEPERIPHERAL_H

#include <BLEManager.h>
#include "BLEGattServer.h"
#include "BLEService.h"
#include "BLELinkStatus.h"
#include "BLEAdvertisement.h"
#include "ble_types.h"

typedef enum {BLEPeripheralEventConnected, BLEPeripheralEventDisconnected, BLEPeripheralEventTimeout, BLEPeripheralEventNUM} BLEPeripheralEventType;
typedef void (*BLEPeripheralEventHandler)(BLEPeripheral &peripheral); 

class BLEPeripheral : public BLEGattServer, public BLELinkStatus, public BLEAdvertisement {
public:
    BLEPeripheral(void);

	void setAdvertisement(BLEAdvertisement &advertisement);
    BLEAdvertisement &getAdvertisement();

    void setDeviceName(const char *deviceName);
    void setAppearance(const unsigned short appearance);
    void setPreferredConnectionParameters(uint16_t minConnInterval, uint16_t maxConnInterval, uint16_t slaveLatency, uint16_t supervisingTimeout);

    void setEventHandler(BLEPeripheralEventType event, BLEPeripheralEventHandler eventHandler);

    bool begin(void);
    void poll(void);
    void end(void);
    bool disconnect(void);
    
    void onBleEvent(ble_evt_t *bleEvent);
    
private:
    ble_gap_adv_params_t        _advParams;
    BLEPeripheralEventHandler   _peripheralEventHandlers[BLEPeripheralEventNUM];
};

#endif
