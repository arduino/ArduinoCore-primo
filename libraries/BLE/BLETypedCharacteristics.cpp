// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Modified by Arduino.org development team

#include "BLETypedCharacteristics.h"

BLECharCharacteristic::BLECharCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<char>(uuid, properties) {
}

BLEUnsignedCharCharacteristic::BLEUnsignedCharCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<unsigned char>(uuid, properties) {
}

BLEShortCharacteristic::BLEShortCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<short>(uuid, properties) {
}

BLEUnsignedShortCharacteristic::BLEUnsignedShortCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<unsigned short>(uuid, properties) {
}

BLEIntCharacteristic::BLEIntCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<int>(uuid, properties) {
}

BLEUnsignedIntCharacteristic::BLEUnsignedIntCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<unsigned int>(uuid, properties) {
}

BLELongCharacteristic::BLELongCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<long>(uuid, properties) {
}

BLEUnsignedLongCharacteristic::BLEUnsignedLongCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<unsigned long>(uuid, properties) {
}

BLEFloatCharacteristic::BLEFloatCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<float>(uuid, properties) {
}

BLEDoubleCharacteristic::BLEDoubleCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<double>(uuid, properties) {
}


//


BLERemoteCharCharacteristic::BLERemoteCharCharacteristic(const char* uuid, unsigned char properties) :
  BLERemoteTypedCharacteristic<char>(uuid, properties) {
}

BLERemoteUnsignedCharCharacteristic::BLERemoteUnsignedCharCharacteristic(const char* uuid, unsigned char properties) :
  BLERemoteTypedCharacteristic<unsigned char>(uuid, properties) {
}

BLERemoteShortCharacteristic::BLERemoteShortCharacteristic(const char* uuid, unsigned char properties) :
  BLERemoteTypedCharacteristic<short>(uuid, properties) {
}

BLERemoteUnsignedShortCharacteristic::BLERemoteUnsignedShortCharacteristic(const char* uuid, unsigned char properties) :
  BLERemoteTypedCharacteristic<unsigned short>(uuid, properties) {
}

BLERemoteIntCharacteristic::BLERemoteIntCharacteristic(const char* uuid, unsigned char properties) :
  BLERemoteTypedCharacteristic<int>(uuid, properties) {
}

BLERemoteUnsignedIntCharacteristic::BLERemoteUnsignedIntCharacteristic(const char* uuid, unsigned char properties) :
  BLERemoteTypedCharacteristic<unsigned int>(uuid, properties) {
}

BLERemoteLongCharacteristic::BLERemoteLongCharacteristic(const char* uuid, unsigned char properties) :
  BLERemoteTypedCharacteristic<long>(uuid, properties) {
}

BLERemoteUnsignedLongCharacteristic::BLERemoteUnsignedLongCharacteristic(const char* uuid, unsigned char properties) :
  BLERemoteTypedCharacteristic<unsigned long>(uuid, properties) {
}

BLERemoteFloatCharacteristic::BLERemoteFloatCharacteristic(const char* uuid, unsigned char properties) :
  BLERemoteTypedCharacteristic<float>(uuid, properties) {
}

BLERemoteDoubleCharacteristic::BLERemoteDoubleCharacteristic(const char* uuid, unsigned char properties) :
  BLERemoteTypedCharacteristic<double>(uuid, properties) {
}
