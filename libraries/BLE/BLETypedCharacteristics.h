// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Modified by Arduino.org development team

#ifndef _BLE_TYPED_CHARACTERISTICS_H_
#define _BLE_TYPED_CHARACTERISTICS_H_

#include "BLETypedCharacteristic.h"

class BLEBoolCharacteristic : public BLETypedCharacteristic<bool> {
  public:
    BLEBoolCharacteristic(const char* uuid, unsigned char properties);
};

class BLECharCharacteristic : public BLETypedCharacteristic<char> {
  public:
    BLECharCharacteristic(const char* uuid, unsigned char properties);
};

class BLEUnsignedCharCharacteristic : public BLETypedCharacteristic<unsigned char> {
  public:
    BLEUnsignedCharCharacteristic(const char* uuid, unsigned char properties);
};

class BLEShortCharacteristic : public BLETypedCharacteristic<short> {
  public:
    BLEShortCharacteristic(const char* uuid, unsigned char properties);
};

class BLEUnsignedShortCharacteristic : public BLETypedCharacteristic<unsigned short> {
  public:
    BLEUnsignedShortCharacteristic(const char* uuid, unsigned char properties);
};

class BLEIntCharacteristic : public BLETypedCharacteristic<int> {
  public:
    BLEIntCharacteristic(const char* uuid, unsigned char properties);
};

class BLEUnsignedIntCharacteristic : public BLETypedCharacteristic<unsigned int> {
  public:
    BLEUnsignedIntCharacteristic(const char* uuid, unsigned char properties);
};

class BLELongCharacteristic : public BLETypedCharacteristic<long> {
  public:
    BLELongCharacteristic(const char* uuid, unsigned char properties);
};

class BLEUnsignedLongCharacteristic : public BLETypedCharacteristic<unsigned long> {
  public:
    BLEUnsignedLongCharacteristic(const char* uuid, unsigned char properties);
};

class BLEFloatCharacteristic : public BLETypedCharacteristic<float> {
  public:
    BLEFloatCharacteristic(const char* uuid, unsigned char properties);
};

class BLEDoubleCharacteristic : public BLETypedCharacteristic<double> {
  public:
    BLEDoubleCharacteristic(const char* uuid, unsigned char properties);
};


//


class BLERemoteBoolCharacteristic : public BLERemoteTypedCharacteristic<bool> {
  public:
    BLERemoteBoolCharacteristic(const char* uuid, unsigned char properties);
};

class BLERemoteCharCharacteristic : public BLERemoteTypedCharacteristic<char> {
  public:
    BLERemoteCharCharacteristic(const char* uuid, unsigned char properties);
};

class BLERemoteUnsignedCharCharacteristic : public BLERemoteTypedCharacteristic<unsigned char> {
  public:
    BLERemoteUnsignedCharCharacteristic(const char* uuid, unsigned char properties);
};

class BLERemoteShortCharacteristic : public BLERemoteTypedCharacteristic<short> {
  public:
    BLERemoteShortCharacteristic(const char* uuid, unsigned char properties);
};

class BLERemoteUnsignedShortCharacteristic : public BLERemoteTypedCharacteristic<unsigned short> {
  public:
    BLERemoteUnsignedShortCharacteristic(const char* uuid, unsigned char properties);
};

class BLERemoteIntCharacteristic : public BLERemoteTypedCharacteristic<int> {
  public:
    BLERemoteIntCharacteristic(const char* uuid, unsigned char properties);
};

class BLERemoteUnsignedIntCharacteristic : public BLERemoteTypedCharacteristic<unsigned int> {
  public:
    BLERemoteUnsignedIntCharacteristic(const char* uuid, unsigned char properties);
};

class BLERemoteLongCharacteristic : public BLERemoteTypedCharacteristic<long> {
  public:
    BLERemoteLongCharacteristic(const char* uuid, unsigned char properties);
};

class BLERemoteUnsignedLongCharacteristic : public BLERemoteTypedCharacteristic<unsigned long> {
  public:
    BLERemoteUnsignedLongCharacteristic(const char* uuid, unsigned char properties);
};

class BLERemoteFloatCharacteristic : public BLERemoteTypedCharacteristic<float> {
  public:
    BLERemoteFloatCharacteristic(const char* uuid, unsigned char properties);
};

class BLERemoteDoubleCharacteristic : public BLERemoteTypedCharacteristic<double> {
  public:
    BLERemoteDoubleCharacteristic(const char* uuid, unsigned char properties);
};


#endif
