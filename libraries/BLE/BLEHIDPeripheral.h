// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Modified by Arduino.org development team

#ifndef _BLE_HID_PERIPHERAL_H_
#define _BLE_HID_PERIPHERAL_H_

#include "BLEHID.h"
#include "BLEHIDReportMapCharacteristic.h"
#include "BLEPeripheral.h"

class BLEHIDPeripheral : public BLEPeripheral
{
  friend class BLEHID;

  public:
    BLEHIDPeripheral(unsigned char req = BLE_DEFAULT_REQ, unsigned char rdy = BLE_DEFAULT_RDY, unsigned char rst = BLE_DEFAULT_RST);
    ~BLEHIDPeripheral();

    void begin();

    void setReportIdOffset(unsigned char reportIdOffset);

    void poll();

    void addHID(BLEHID& hid);

  protected:
    static BLEHIDPeripheral* instance();

  private:
    static BLEHIDPeripheral*            _instance;

    BLEService                          _hidService;
    BLEHIDReportMapCharacteristic       _hidReportMapCharacteristic;
    BLEProgmemConstantCharacteristic    _hidInformationCharacteristic;
    BLEUnsignedCharCharacteristic       _hidControlPointCharacteristic;

    unsigned char                       _reportIdOffset;

    BLEHID**                            _hids;
    unsigned char                       _numHids;
};

#endif
