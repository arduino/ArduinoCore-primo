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

#include "Uart.h"
#include "WVariant.h"
#include "wiring_digital.h"

//tx and rx buffer for EasyDMA
static uint8_t tx_buffer[1];
static uint8_t rx_buffer[1];

Uart::Uart(uint8_t _pinRX, uint8_t _pinTX)
{
	uc_pinRX = _pinRX;
	uc_pinTX = _pinTX;
}


void Uart::begin(unsigned long baudrate)
{
	begin(baudrate, (uint8_t)SERIAL_8N1);
}

void Uart::begin(unsigned long baudrate, uint8_t config)
{
	//set tx and rx buffer for EasyDMA
	nrf_uarte_tx_buffer_set(NRF_UARTE0, tx_buffer, 1);
	nrf_uarte_rx_buffer_set(NRF_UARTE0, rx_buffer, 1);
	
	//configure TX and RX pins
	pinMode(uc_pinRX, INPUT);	
	pinMode(uc_pinTX, OUTPUT);
	digitalWrite(uc_pinTX, HIGH);
	
	//set UARTE peripheral
	nrf_uarte_baudrate_set(NRF_UARTE0, adaptBaudrate(baudrate));	
	nrf_uarte_configure(NRF_UARTE0, extractParity(config), NRF_UARTE_HWFC_DISABLED);
	nrf_uarte_txrx_pins_set(NRF_UARTE0, g_APinDescription[uc_pinTX].ulPin, g_APinDescription[uc_pinRX].ulPin);
	
	//enable peripheral
	nrf_uarte_enable(NRF_UARTE0);
	
	//enable rx interrupt
	nrf_uarte_int_enable(NRF_UARTE0, NRF_UARTE_INT_ENDRX_MASK);
	NVIC_SetPriority(UART0_IRQn, 6); //low priority
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);
	nrf_uarte_event_clear(NRF_UARTE0, NRF_UARTE_EVENT_ENDRX);
	nrf_uarte_shorts_enable(NRF_UARTE0, NRF_UARTE_SHORT_ENDRX_STARTRX);
	nrf_uarte_task_trigger(NRF_UARTE0, NRF_UARTE_TASK_STARTRX);
	while(!nrf_uarte_event_check(NRF_UARTE0, NRF_UARTE_EVENT_RXSTARTED))
		; //wait until rx starts
	
}

void Uart::end()
{
	//stop rx
	nrf_uarte_shorts_disable(NRF_UARTE0, NRF_UARTE_SHORT_ENDRX_STARTRX);
	nrf_uarte_task_trigger(NRF_UARTE0, NRF_UARTE_TASK_STOPRX);
	//disable interrupt
	nrf_uarte_int_disable(NRF_UARTE0, NRF_UARTE_INT_ENDRX_MASK);
	//disable peripheral
	nrf_uarte_disable(NRF_UARTE0);
	//disconnect pins
	nrf_uarte_txrx_pins_disconnect(NRF_UARTE0);
	rxBuffer.clear();
}

void Uart::flush()
{
	nrf_uarte_task_trigger(NRF_UARTE0, NRF_UARTE_TASK_STARTTX);
	rxBuffer.clear();
}

void Uart::IrqHandler()
{
	nrf_uarte_event_clear(NRF_UARTE0, NRF_UARTE_EVENT_ENDRX);
	rxBuffer.store_char(rx_buffer[0]);
}

int Uart::available()
{
	return rxBuffer.available();
}

int Uart::peek()
{
	return rxBuffer.peek();
}

int Uart::read()
{
	return rxBuffer.read_char();
}


size_t Uart::write(const uint8_t data)
{
	tx_buffer[0] = data;
	nrf_uarte_event_clear(NRF_UARTE0, NRF_UARTE_EVENT_ENDTX);
	nrf_uarte_task_trigger(NRF_UARTE0, NRF_UARTE_TASK_STARTTX);
	while(!nrf_uarte_event_check(NRF_UARTE0, NRF_UARTE_EVENT_ENDTX))
		;//wait until trasmission is complete
	return 1;
}


 size_t Uart::write(const char * data)
{
	size_t writed = 0;
	while(*data != '\0')
	{
		writed += Uart::write(*data);
		++data;
	}
	
	return writed;
}


nrf_uarte_baudrate_t Uart::adaptBaudrate(unsigned long baudRate){
	if (baudRate<1800)
			return NRF_UARTE_BAUDRATE_1200;
	if (baudRate<3600)
			return NRF_UARTE_BAUDRATE_2400;
	if (baudRate<7200)
			return NRF_UARTE_BAUDRATE_4800;
	if (baudRate<12000)
			return NRF_UARTE_BAUDRATE_9600;
	if (baudRate<16800)
			return NRF_UARTE_BAUDRATE_14400;
	if (baudRate<24000)
			return NRF_UARTE_BAUDRATE_19200;
	if (baudRate<33600)
			return NRF_UARTE_BAUDRATE_28800;
	if (baudRate<48000)
			return NRF_UARTE_BAUDRATE_38400;
	if (baudRate<67200)
			return NRF_UARTE_BAUDRATE_57600;
	if (baudRate<96000)
			return NRF_UARTE_BAUDRATE_76800;
	if (baudRate<172800)
			return NRF_UARTE_BAUDRATE_115200;
	if (baudRate<240200)
			return NRF_UARTE_BAUDRATE_230400;
	if (baudRate<355400)
			return NRF_UARTE_BAUDRATE_250000;
	if (baudRate<691200)
			return NRF_UARTE_BAUDRATE_460800;
	if (baudRate<960800)
			return NRF_UARTE_BAUDRATE_921600;
	else
			return NRF_UARTE_BAUDRATE_1000000;
}

nrf_uarte_parity_t Uart::extractParity(uint8_t config)
{
	switch(config & HARDSER_PARITY_MASK)
	{
		case HARDSER_PARITY_NONE:
		default:
			return NRF_UARTE_PARITY_EXCLUDED;

		case HARDSER_PARITY_EVEN:
			return NRF_UARTE_PARITY_INCLUDED;
	}
}

#ifdef __cplusplus
extern "C" {
#endif

void UARTE0_UART0_IRQHandler(){
	Serial.IrqHandler();
}
#ifdef __cplusplus
}
#endif
