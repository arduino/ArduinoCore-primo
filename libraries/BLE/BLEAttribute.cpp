// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Modified by Arduino.org development team

#include "BLEAttribute.h"
#include "BLEUuid.h"

BLEAttribute::BLEAttribute(const char* uuid, enum BLEAttributeType type) :
  _uuid(uuid),
  _type(type)
{
}

const char* BLEAttribute::uuid() const {
  return this->_uuid;
}

const char* BLEAttribute::rawUuid() const{
  return (char *)BLEUuid(this->_uuid).data();
}

enum BLEAttributeType BLEAttribute::type() const {
  return this->_type;
}
