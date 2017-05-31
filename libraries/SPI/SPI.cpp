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

#include "SPI.h"

SPIClass::SPIClass(NRF_SPI_Type *SPIInstance, uint8_t uc_pinMISO, uint8_t uc_pinSCK, uint8_t uc_pinMOSI)
{
   _SPIInstance = SPIInstance;

    _uc_pinMiso = uc_pinMISO;
    _uc_pinSCK = uc_pinSCK;
    _uc_pinMosi = uc_pinMOSI;
}


void SPIClass::begin()
{
    pinMode(_uc_pinSCK, OUTPUT);
    pinMode(_uc_pinMosi, OUTPUT);
    pinMode(_uc_pinMiso, INPUT);

    _SPIInstance->PSELSCK = g_APinDescription[_uc_pinSCK].ulPin;
    _SPIInstance->PSELMOSI = g_APinDescription[_uc_pinMosi].ulPin;
    _SPIInstance->PSELMISO = g_APinDescription[_uc_pinMiso].ulPin;

	// Default speed set to 4Mhz, SPI mode set to MODE 0 and Bit order set to MSB first.
	_SPIInstance->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_M4 << SPI_FREQUENCY_FREQUENCY_Pos);
	setDataMode(SPI_MODE0);
	setBitOrder(MSBFIRST);
	_order=MSBFIRST;

	_SPIInstance->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
}


void SPIClass::beginSlave()
{
	pinMode(_uc_pinSCK, INPUT);
    pinMode(_uc_pinMosi, OUTPUT);
    pinMode(_uc_pinMiso, INPUT);


    _SPIInstance->PSELSCK = g_APinDescription[_uc_pinSCK].ulPin;
    _SPIInstance->PSELMOSI = g_APinDescription[_uc_pinMosi].ulPin;
    _SPIInstance->PSELMISO = g_APinDescription[_uc_pinMiso].ulPin;

	
	setDataMode(SPI_MODE0);
	setBitOrder(MSBFIRST);
	_order=MSBFIRST;

	_SPIInstance->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
}


void SPIClass::end()
{
	_SPIInstance->ENABLE &= ~(SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
}

#ifdef ARDUINO_NRF52_PRIMO_CORE

void SPIClass::begin(int uc_pinMISO, int uc_pinMOSI, int uc_pinSCK)
{
    _uc_pinMiso = uc_pinMISO;
    _uc_pinSCK = uc_pinSCK;
    _uc_pinMosi = uc_pinMOSI;

    pinMode(_uc_pinSCK, OUTPUT);
    pinMode(_uc_pinMosi, OUTPUT);
    pinMode(_uc_pinMiso, INPUT);

	_SPIInstance->PSELSCK = g_APinDescription[_uc_pinSCK].ulPin;
    _SPIInstance->PSELMOSI = g_APinDescription[_uc_pinMosi].ulPin;
    _SPIInstance->PSELMISO = g_APinDescription[_uc_pinMiso].ulPin;

	// Default speed set to 4Mhz, SPI mode set to MODE 0 and Bit order set to MSB first.
	_SPIInstance->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_M4 << SPI_FREQUENCY_FREQUENCY_Pos);
	setDataMode(SPI_MODE0);
	setBitOrder(MSBFIRST);
	_order=MSBFIRST;

	_SPIInstance->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
}


void SPIClass::beginSlave(int uc_pinMISO, int uc_pinMOSI, int uc_pinSCK)
{
    _uc_pinMiso = uc_pinMISO;
    _uc_pinSCK = uc_pinSCK;
    _uc_pinMosi = uc_pinMOSI;

	pinMode(_uc_pinSCK, INPUT);
    pinMode(_uc_pinMosi, OUTPUT);
    pinMode(_uc_pinMiso, INPUT);


    _SPIInstance->PSELSCK = g_APinDescription[_uc_pinSCK].ulPin;
    _SPIInstance->PSELMOSI = g_APinDescription[_uc_pinMosi].ulPin;
    _SPIInstance->PSELMISO = g_APinDescription[_uc_pinMiso].ulPin;

	
	setDataMode(SPI_MODE0);
	setBitOrder(MSBFIRST);
	_order=MSBFIRST;

	_SPIInstance->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
}
#endif //ARDUINO_NRF52_PRIMO_CORE

void SPIClass::beginTransaction(SPISettings settings)
{
	uint8_t bitOrder;
	uint8_t SpiClockMode;
	
	interruptSave = intStatus();
	noInterrupts();
	
	end();

	pinMode(_uc_pinSCK, OUTPUT);
    pinMode(_uc_pinMosi, OUTPUT);
    pinMode(_uc_pinMiso, INPUT);

	_SPIInstance->PSELSCK = g_APinDescription[_uc_pinSCK].ulPin;
    _SPIInstance->PSELMOSI = g_APinDescription[_uc_pinMosi].ulPin;
    _SPIInstance->PSELMISO = g_APinDescription[_uc_pinMiso].ulPin;

#ifdef ARDUINO_NRF52_PRIMO_CORE
	if(settings._uc_pinMiso != 0 && settings._uc_pinMosi != 0 && settings._uc_pinSCK != 0){
		// override configuration with pins selected by user
		pinMode(settings._uc_pinSCK, OUTPUT);
		pinMode(settings._uc_pinMosi, OUTPUT);
		pinMode(settings._uc_pinMiso, INPUT);

		_SPIInstance->PSELSCK = g_APinDescription[settings._uc_pinSCK].ulPin;
		_SPIInstance->PSELMOSI = g_APinDescription[settings._uc_pinMosi].ulPin;
		_SPIInstance->PSELMISO = g_APinDescription[settings._uc_pinMiso].ulPin;
	}
#endif //ARDUINO_NRF52_PRIMO_CORE
	
	if(settings.interface_clock < 182000)
		setClockDivider(SPI_CLOCK_DIV128);
	else if(settings.interface_clock < 375000)
		setClockDivider(SPI_CLOCK_DIV64);
	else if(settings.interface_clock < 750000)
		setClockDivider(SPI_CLOCK_DIV32);
	else if(settings.interface_clock < 1500000)
		setClockDivider(SPI_CLOCK_DIV16);
	else if(settings.interface_clock < 3000000)
		setClockDivider(SPI_CLOCK_DIV8);
	else if(settings.interface_clock < 6000000)
		setClockDivider(SPI_CLOCK_DIV4);
	else
		setClockDivider(SPI_CLOCK_DIV2);

	setDataMode(settings.data_mode);
	setBitOrder((BitOrder)settings.bit_order);
	_order=(BitOrder)settings.bit_order;
	
	_SPIInstance->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
	inTransactionFlag=1;
}

void SPIClass::endTransaction(void)
{
	inTransactionFlag=0;
	interrupts();
}	

void SPIClass::setBitOrder(BitOrder order)
{
	if (order == LSBFIRST)
		_SPIInstance->CONFIG |= (SPI_CONFIG_ORDER_LsbFirst << SPI_CONFIG_ORDER_Pos);
	else
  		_SPIInstance->CONFIG &= ~(SPI_CONFIG_ORDER_LsbFirst << SPI_CONFIG_ORDER_Pos);
	_order=order;
}

void SPIClass::setDataMode(uint8_t uc_mode)
{
	switch(uc_mode)
	{
		case SPI_MODE0:
		default:
			_SPIInstance->CONFIG &= ~(SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos); //CPOL=0
			_SPIInstance->CONFIG &= ~(SPI_CONFIG_CPHA_Trailing << SPI_CONFIG_CPHA_Pos);	//CPHA=0
			break;
			
		case SPI_MODE1:
			_SPIInstance->CONFIG &= ~(SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos); //CPOL=0
			_SPIInstance->CONFIG |= (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);  //CPHA=1
			break;
		
		case SPI_MODE2:
			_SPIInstance->CONFIG |= (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);  //CPOL=1
			_SPIInstance->CONFIG &= ~(SPI_CONFIG_CPHA_Trailing << SPI_CONFIG_CPHA_Pos);	//CPHA=0
			break;
			
		case SPI_MODE3:
			_SPIInstance->CONFIG |= (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);  //CPOL=1
			_SPIInstance->CONFIG |= (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);  //CPHA=1
			break;
	}
}


void SPIClass::setClockDivider(uint16_t div)
{
	if(div==SPI_CLOCK_DIV2){
    	_SPIInstance->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_M8 << SPI_FREQUENCY_FREQUENCY_Pos);
		return;
	}
	if(div==SPI_CLOCK_DIV4){
    	_SPIInstance->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_M4 << SPI_FREQUENCY_FREQUENCY_Pos);
		return;
	}
	if(div==SPI_CLOCK_DIV8){
    	_SPIInstance->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_M2 << SPI_FREQUENCY_FREQUENCY_Pos);
		return;
	}
	if(div==SPI_CLOCK_DIV16){
    	_SPIInstance->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_M1 << SPI_FREQUENCY_FREQUENCY_Pos);
		return;
	}
	if(div==SPI_CLOCK_DIV32){
		_SPIInstance->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_K500 << SPI_FREQUENCY_FREQUENCY_Pos);
		return;
	}
	if(div==SPI_CLOCK_DIV64){
    	_SPIInstance->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_K250 << SPI_FREQUENCY_FREQUENCY_Pos);
		return;
	}
	if(div==SPI_CLOCK_DIV128){
    	_SPIInstance->FREQUENCY = (SPI_FREQUENCY_FREQUENCY_K125 << SPI_FREQUENCY_FREQUENCY_Pos);
		return;
	}
}

byte SPIClass::transfer(uint8_t data)
{
	// clear ready flag
	_SPIInstance->EVENTS_READY = 0;
    _SPIInstance->TXD = data;
	// wait for a complete trasmission
    while (_SPIInstance->EVENTS_READY == 0)
      ;
	
    // return received data
    return _SPIInstance->RXD;;
}

byte SPIClass::read()
{
	//read data
	byte data=_SPIInstance->RXD;
	
	//write received data
	_SPIInstance->EVENTS_READY = 0;
	_SPIInstance->TXD = data;
	while (_SPIInstance->EVENTS_READY == 0)
      ;//wait for a complete trasmission
  
	return data;
}

void SPIClass::usingInterrupt(uint8_t intNum)
{
	uint8_t irestore;
	uint32_t mask;
	
	irestore=intStatus();
	noInterrupts();
	if(intNum > 19)
	{
		return;
	}
	else
	{
		mask=g_APinDescription[intNum].ulPin;
		interruptMode=1;
		interruptMask=mask;
	}
	if (irestore) interrupts();		
}

void SPIClass::write(uint8_t data)
{
	_SPIInstance->EVENTS_READY = 0;
	_SPIInstance->TXD = data;
	while (_SPIInstance->EVENTS_READY == 0)
      ;//wait for a complete trasmission
}

void SPIClass::transfer(void *data, size_t count){
	uint8_t *p;
	
	if(count==0) return;
	p=(uint8_t *)data;
	
	//write first byte
	_SPIInstance->EVENTS_READY = 0;
	_SPIInstance->TXD = *p;
	//wait for a complete trasmission
	while (_SPIInstance->EVENTS_READY == 0)
		;
  
	while(--count > 0)
	{
		
		uint8_t out = *(p+1);
		//read first byte
		uint8_t in = _SPIInstance->RXD;
		//write next byte
		_SPIInstance->EVENTS_READY = 0;
		_SPIInstance->TXD = *p;
		//wait for a complete trasmission
		while (_SPIInstance->EVENTS_READY == 0)
			;
		//save read data in previous location
		*p++=in;
	}
	//read last byte
	*p=_SPIInstance->RXD;	
}	

uint16_t SPIClass::transfer16(uint16_t data)
{
	union{
		uint16_t value;
		struct{
			uint8_t lsb;
			uint8_t msb;
		};
	}in, out;
	
	in.value = data;
	
	if(_order==MSBFIRST){	
		//send msb
		_SPIInstance->EVENTS_READY = 0;
		_SPIInstance->TXD = in.msb;
		while (_SPIInstance->EVENTS_READY == 0)
			;//wait for a complete trasmission
		//read msb
		out.msb = _SPIInstance->RXD;
		
		//send lsb
		_SPIInstance->EVENTS_READY = 0;
		_SPIInstance->TXD = in.lsb;
		while (_SPIInstance->EVENTS_READY == 0)
			;//wait for a complete trasmission
		//read lsb
		out.lsb = _SPIInstance->RXD;
	}
	else{
		//send lsb
		_SPIInstance->EVENTS_READY = 0;
		_SPIInstance->TXD = in.lsb;
		while (_SPIInstance->EVENTS_READY == 0)
			;//wait for a complete trasmission
		//read lsb
		out.lsb = _SPIInstance->RXD;
		
		//send msb
		_SPIInstance->EVENTS_READY = 0;
		_SPIInstance->TXD = in.msb;
		while (_SPIInstance->EVENTS_READY == 0)
			;//wait for a complete trasmission
		//read msb
		out.msb = _SPIInstance->RXD;
	}
	
	return out.value;
}

void SPIClass::attachInterrupt(void)
{
	// Should be enableInterrupt()
}

void SPIClass::detachInterrupt(void)
{
	// Should be disableInterrupt()
}

SPIClass SPI(NRF_SPI2, PIN_SPI_MISO, PIN_SPI_SCK, PIN_SPI_MOSI);
