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
 
#include "SoftwareSerial.h" 
 
SoftwareSerial *SoftwareSerial::active_object = 0;
char SoftwareSerial::_receive_buffer[_SS_MAX_RX_BUFF]; 
volatile uint8_t SoftwareSerial::_receive_buffer_tail = 0;
volatile uint8_t SoftwareSerial::_receive_buffer_head = 0;


void uart_bb_rx_callback(uint8_t byte){
	SoftwareSerial::active_object->recv(byte);
}

SoftwareSerial::SoftwareSerial(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic /* = false */) :
  _buffer_overflow(false),
  _inverse_logic(inverse_logic)
{  
  _receivePin = receivePin;
  _transmitPin = transmitPin;
}


SoftwareSerial::~SoftwareSerial()
{
 end();
}

void SoftwareSerial::begin(long speed)
 {  
	_speed=speed;
	listen();
}

bool SoftwareSerial::listen()
{
  if (active_object != this)
  {
    if (active_object)
      active_object->stopListening();

    _buffer_overflow = false;
    _receive_buffer_head = _receive_buffer_tail = 0;
    active_object = this;
	static uart_bb_config_t config;
	config.pin_txd = g_APinDescription[_transmitPin].ulPin;
	config.pin_rxd = g_APinDescription[_receivePin].ulPin;
	config.txd_inverted = _inverse_logic;
	config.baudrate = _speed;
	config.rx_callback = uart_bb_rx_callback;
	uart_bb_init(&config);
	return true;
  }
 return false;
}

bool SoftwareSerial::stopListening()
{
   if (active_object == this)
   {  
	 active_object = NULL;
     return true;
   }
  return false;
}

void SoftwareSerial::end()
{
 stopListening();
}

int SoftwareSerial::read()
{
  if (!isListening()){
    return -1;}


  // Empty buffer?
  if (_receive_buffer_head == _receive_buffer_tail){
    return -1;}

  // Read from "head"
  uint8_t d = _receive_buffer[_receive_buffer_head]; // grab next byte
  _receive_buffer_head = (_receive_buffer_head + 1) % _SS_MAX_RX_BUFF;
  return d;
}  

int SoftwareSerial::available()
{
  if (!isListening())
    return 0;
  
  return (_receive_buffer_tail + _SS_MAX_RX_BUFF - _receive_buffer_head) % _SS_MAX_RX_BUFF;
}

size_t SoftwareSerial::write(uint8_t b)
{
	uart_bb_put(b);
	return 1;
}

void SoftwareSerial::flush()
{
  if (!isListening())
    return;
	
  _receive_buffer_head = _receive_buffer_tail = 0;
}

int SoftwareSerial::peek()
{
  if (!isListening())
    return -1;

  // Empty buffer?
  if (_receive_buffer_head == _receive_buffer_tail)
    return -1;

  // Read from "head"
  return _receive_buffer[_receive_buffer_head];
}


//private methods

void SoftwareSerial::recv(uint8_t byte)
{
      // if buffer full, set the overflow flag and return
    uint8_t next = (_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF;
    if (next != _receive_buffer_head)
    {
      // save new data in buffer: tail points to where byte goes
      _receive_buffer[_receive_buffer_tail] = byte; // save new byte
      _receive_buffer_tail = next;
    } 
    else 
    {
      _buffer_overflow = true;
    }
}