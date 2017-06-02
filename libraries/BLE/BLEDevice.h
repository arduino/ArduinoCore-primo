// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Modified by Arduino.org development team

#ifndef _BLE_DEVICE_H_
#define _BLE_DEVICE_H_

#include "BLEBondStore.h"
#include "BLECharacteristic.h"
#include "BLELocalAttribute.h"
#include "BLERemoteAttribute.h"
#include "BLERemoteCharacteristic.h"
#include "BLERemoteService.h"

struct BLEEirData
{
  unsigned char length;
  unsigned char type;
  unsigned char data[BLE_EIR_DATA_MAX_VALUE_LENGTH];
};

enum BLEEventNo{
  DISCONNECTED = 0,
  AUTH_STATUS,
  SERVICE_DISC_RESP,
  CHARACT_DISC_RESP,
  READ_RESPONSE,
  WRITE_RESPONSE
};

class BLEDevice;

class BLEDeviceEventListener
{
  public:
    virtual void BLEDeviceConnected(BLEDevice& /*device*/, const unsigned char* /*address*/) { }
    virtual void BLEDeviceDisconnected(BLEDevice& /*device*/) { }
    virtual void BLEDeviceBonded(BLEDevice& /*device*/) { }
    virtual void BLEDeviceRemoteServicesDiscovered(BLEDevice& /*device*/) { }
    virtual void BLEDevicePasskeyReceived(BLEDevice& /*device*/) {}
    virtual void BLEDevicePasskeyRequested(BLEDevice& /*device*/) {}
    virtual void BLEMessageReceived(BLEDevice& /*device*/, int eventCode, int messageCode);
	
    virtual void BLEDeviceCharacteristicValueChanged(BLEDevice& /*device*/, BLECharacteristic& /*characteristic*/, const unsigned char* /*value*/, unsigned char /*valueLength*/) { }
    virtual void BLEDeviceCharacteristicSubscribedChanged(BLEDevice& /*device*/, BLECharacteristic& /*characteristic*/, bool /*subscribed*/) { }

    virtual void BLEDeviceRemoteCharacteristicValueChanged(BLEDevice& /*device*/, BLERemoteCharacteristic& /*characteristic*/, const unsigned char* /*value*/, unsigned char /*valueLength*/) { }
	
    virtual void BLEDeviceAddressReceived(BLEDevice& /*device*/, const unsigned char* /*address*/) { }
    virtual void BLEDeviceTemperatureReceived(BLEDevice& /*device*/, float /*temperature*/) { }
    virtual void BLEDeviceBatteryLevelReceived(BLEDevice& /*device*/, float /*batteryLevel*/) { }
};


class BLEDevice
{
  friend class BLEPeripheral;

  protected:
    BLEDevice();

    virtual ~BLEDevice();

    void setEventListener(BLEDeviceEventListener* eventListener);

    void setAdvertisingInterval(unsigned short advertisingInterval);
    void setConnectionInterval(unsigned short minimumConnectionInterval, unsigned short maximumConnectionInterval);
    void setConnectable(bool connectable);
    void setBondStore(BLEBondStore& bondStore);
    void sendPasskey(char passkey[]);
    void confirmPasskey(bool confirm);

    virtual void begin(unsigned char /*advertisementDataSize*/,
                unsigned char * /*advertisementData*/,
                unsigned char /*scanDataSize*/,
                unsigned char * /*scanData*/,
                BLELocalAttribute** /*localAttributes*/,
                unsigned char /*numLocalAttributes*/,
                BLERemoteAttribute** /*remoteAttributes*/,
                unsigned char /*numRemoteAttributes*/) { }
				
    virtual void poll(ble_evt_t *bleEvent = 0) { }

    virtual void end() { }

    virtual bool setTxPower(int /*txPower*/) { return false; }

    virtual void startAdvertising() { }
    virtual void disconnect() { }

    virtual bool updateCharacteristicValue(BLECharacteristic& /*characteristic*/) { return false; }
    virtual bool broadcastCharacteristic(BLECharacteristic& /*characteristic*/) { return false; }
    virtual bool canNotifyCharacteristic(BLECharacteristic& /*characteristic*/) { return false; }
    virtual bool canIndicateCharacteristic(BLECharacteristic& /*characteristic*/) { return false; }

    virtual bool canReadRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool readRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool canWriteRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool writeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/, const unsigned char /*value*/[], unsigned char /*length*/) { return false; }
    virtual bool canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool subscribeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }

    virtual void requestAddress() { }
    virtual void requestTemperature() { }
    virtual void requestBatteryLevel() { }

  protected:
    uint16_t                      _connectionHandle;
    unsigned short                _advertisingInterval;
    unsigned short                _minimumConnectionInterval;
    unsigned short                _maximumConnectionInterval;
    bool                          _connectable;
    bool                          _mitm;
    uint8_t                       _io_caps;
    uint8_t                       _passkey[6];
    bool                          _userConfirm;
    BLEBondStore*                 _bondStore;
    BLEDeviceEventListener*       _eventListener;
    uint8_t                       _status;

};

#endif
