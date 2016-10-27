// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Modified by Arduino.org development team

#include "BLEService.h"

BLEService::BLEService(const char* uuid) :
  BLELocalAttribute(uuid, BLETypeService)
{
}
