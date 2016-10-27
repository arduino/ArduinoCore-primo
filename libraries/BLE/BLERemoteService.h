// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Modified by Arduino.org development team

#ifndef _BLE_REMOTE_SERVICE_H_
#define _BLE_REMOTE_SERVICE_H_

#include "BLERemoteAttribute.h"

class BLERemoteService : public BLERemoteAttribute
{
  public:
    BLERemoteService(const char* uuid);
};

#endif
