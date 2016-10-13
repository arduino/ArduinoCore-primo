/*
  SoftwareSerial.cpp - library for Arduino Primo
  Copyright (c) 2016 Arduino. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
 */
 
#ifndef SoftwareSerial_h
#define SoftwareSerial_h
 
#include <inttypes.h>
#include <Stream.h>
#include <variant.h>

#ifdef __cplusplus
extern "C"{
#endif
  #include "uart_bitbang.h"
#ifdef __cplusplus
}
#endif

#define _SS_MAX_RX_BUFF 64 // RX buffer size

class SoftwareSerial : public Stream
{ 
public:
  SoftwareSerial(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic = false);
  ~SoftwareSerial();
  void begin(long speed);
  bool listen();
  void end();
  bool isListening() { return this == active_object; }
  bool stopListening();
  bool overflow() { bool ret = _buffer_overflow; if (ret) _buffer_overflow = false; return ret; }
  int peek();

  virtual size_t write(uint8_t byte);
  virtual int read();
  void recv(uint8_t byte);
  virtual int available();
  virtual void flush();
  operator bool() { return true; }
  
  using Print::write;
  
  static SoftwareSerial *active_object;
  
private:
  // per object data
  uint8_t _transmitPin;
  uint8_t _receivePin;
  long _speed;
  
  uint16_t _buffer_overflow:1;
  uint16_t _inverse_logic:1;

  // static data

  static char _receive_buffer[_SS_MAX_RX_BUFF]; 
  static volatile uint8_t _receive_buffer_tail;
  static volatile uint8_t _receive_buffer_head;
 
  
 };
 
 #endif SoftwareSerial_h