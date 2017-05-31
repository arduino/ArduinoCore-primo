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


#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include "variant.h"
#include "wiring_constants.h"
#include <Arduino.h>


//#define LSBFIRST 0 //already defined in wiring_constants.h
//#define MSBFIRST 1 

#define SPI_MODE0 0x00
#define SPI_MODE1 0x01
#define SPI_MODE2 0x02
#define SPI_MODE3 0x03

// For compatibility with sketch of other platforms
// This values are ignored in nrf52
#define SPI_CLOCK_DIV2 6       //8 MHz
#define SPI_CLOCK_DIV4 12      //4 MHz
#define SPI_CLOCK_DIV8 24      //2 MHz
#define SPI_CLOCK_DIV16 48     //1 MHz
#define SPI_CLOCK_DIV32 96     //500 KHz
#define SPI_CLOCK_DIV64 192    //250 KHz
#define SPI_CLOCK_DIV128 384   //125 KHz

#ifndef intStatus
#define intStatus() __intStatus()
static inline unsigned char __intStatus(void) __attribute__((always_inline, unused));
static inline unsigned char __intStatus(void) {
	unsigned int primask, faultmask;
	asm volatile ("mrs %0, primask" : "=r" (primask));
	if (primask) return 0;
	asm volatile ("mrs %0, faultmask" : "=r" (faultmask));
	if (faultmask) return 0;
	return 1;
}
#endif

class SPISettings {
	public:
		SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
		{
			interface_clock = clock;
			bit_order = bitOrder;
			data_mode = dataMode;
		}
		
		#ifdef ARDUINO_NRF52_PRIMO_CORE
		SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode, int uc_pinMISO, int uc_pinMOSI, int uc_pinSCK)
		{
			interface_clock = clock;
			bit_order = bitOrder;
			data_mode = dataMode;
			_uc_pinMiso = uc_pinMISO;
			_uc_pinSCK = uc_pinSCK;
			_uc_pinMosi = uc_pinMOSI;
		}
		#endif //ARDUINO_NRF52_PRIMO_CORE

		SPISettings(void)
		{
			interface_clock = 4000000;
			bit_order = MSBFIRST;
			data_mode = SPI_MODE0;
		}
	private:
		uint32_t interface_clock;
		uint8_t bit_order;
		uint8_t data_mode;
		#ifdef ARDUINO_NRF52_PRIMO_CORE
			uint8_t _uc_pinMiso = 0;
			uint8_t _uc_pinMosi = 0;
			uint8_t _uc_pinSCK = 0;
		#endif //ARDUINO_NRF52_PRIMO_CORE
	
	friend class SPIClass;	
};

class SPIClass {
  public:
    SPIClass(NRF_SPI_Type *SPIInstance,  uint8_t uc_pinMISO, uint8_t uc_pinSCK, uint8_t uc_pinMOSI);

	byte transfer(uint8_t data); 
	void transfer(void *data, size_t count);
	byte read(void);
	uint16_t transfer16(uint16_t data);
	void write(uint8_t data);
	// SPI Configuration methods
	void beginTransaction(SPISettings settings);	
	void endTransaction(void);	
	
	void attachInterrupt();		
	void detachInterrupt();		
	
	void usingInterrupt(uint8_t interruptNumber); 
	
	void begin(void);
	void beginSlave();
	void end(void);	

#ifdef ARDUINO_NRF52_PRIMO_CORE
	void begin(int uc_pinMISO, int uc_pinMOSI, int uc_pinSCK);
	void beginSlave(int uc_pinMISO, int uc_pinMOSI, int uc_pinSCK);
#endif	
	
	void setBitOrder(BitOrder order);
	void setDataMode(uint8_t uc_mode);
	void setClockDivider(uint16_t uc_div); 

  private:
	NRF_SPI_Type *_SPIInstance;
	uint8_t _uc_pinMiso;
	uint8_t _uc_pinMosi;
	uint8_t _uc_pinSCK;
	BitOrder _order;
	uint8_t interruptMode;
	uint32_t interruptMask;
	uint8_t interruptSave;
	uint8_t inTransactionFlag;
};

extern SPIClass SPI;

#endif //_SPI_H_INCLUDED