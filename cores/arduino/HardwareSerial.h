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

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>

#include "Stream.h"

//NRF52 provide even parity only
#define HARDSER_PARITY_EVEN (0x1ul)
#define HARDSER_PARITY_NONE (0x3ul)
#define HARDSER_PARITY_MASK	(0xFul)

#define HARDSER_STOP_BIT_1		(0x10ul)

#define HARDSER_DATA_8	 	(0x400ul)

#define SERIAL_8N1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_NONE | HARDSER_DATA_8) 
#define SERIAL_8E1	(HARDSER_STOP_BIT_1 | HARDSER_PARITY_EVEN | HARDSER_DATA_8)

class HardwareSerial : public Stream
{
  public:
    virtual void begin(unsigned long) {};
    virtual void end() {};
    virtual int available(void) = 0;
    virtual int peek(void) = 0;
    virtual int read(void) = 0;
    virtual void flush(void) = 0;
    virtual size_t write(uint8_t) = 0;
    using Print::write; // pull in write(str) and write(buf, size) from Print
    virtual operator bool() = 0;
};

extern void serialEventRun(void) __attribute__((weak));

#endif
