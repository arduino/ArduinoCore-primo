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

#ifndef TwoWire_h
#define TwoWire_h

#include "Stream.h"
#include "variant.h"

#include "nrf_twim.h"
#include "nrf_twis.h"
#include "RingBuffer.h"

#define BUFFER_LENGTH 32

 // WIRE_HAS_END means Wire has end()
#define WIRE_HAS_END 1

class TwoWire : public Stream
{
  public:
    TwoWire(uint8_t pinSDA, uint8_t pinSCL);
    void begin();
    void begin(uint8_t);
    void end();
    void setClock(uint32_t);

    void beginTransmission(uint8_t);
    uint8_t endTransmission(bool stopBit);
    uint8_t endTransmission(void);

    uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit);
    uint8_t requestFrom(uint8_t address, size_t quantity);

    size_t write(uint8_t data);
    size_t write(const uint8_t * data, size_t quantity);

    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    virtual void flush(void);
    void onReceive(void(*)(int));
    void onRequest(void(*)(void));

    using Print::write;

    void onService(void);

  private:
    NRF_TWIM_Type * _TWIInstance;
	NRF_TWIS_Type * _TWISlave;
    uint8_t _uc_pinSDA;
    uint8_t _uc_pinSCL;
	uint8_t _master=1;	//1 = master - 0 = slave
	uint8_t suspended=0;
	
    bool transmissionBegun;

    // RX Buffer
    RingBuffer rxBuffer;

    //TX buffer
    RingBuffer txBuffer;
    uint8_t txAddress;



    // Callback user functions
    void (*onRequestCallback)(void);
    void (*onReceiveCallback)(int);


    // TWI clock frequency
    static const nrf_twim_frequency_t TWI_CLOCK = NRF_TWIM_FREQ_100K;

};

  extern TwoWire Wire;

  extern TwoWire Wire1;
  
#endif
