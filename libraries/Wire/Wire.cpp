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

extern "C" {
#include <string.h>
}

#include <Arduino.h>
#include <wiring_private.h>

#include "Wire.h"

TwoWire::TwoWire(uint8_t pinSDA, uint8_t pinSCL)
{
  this->_uc_pinSDA=pinSDA;
  this->_uc_pinSCL=pinSCL;
  transmissionBegun = false;
}

void TwoWire::begin(void) {
  //Master Mode
  _TWIInstance=NRF_TWIM1;
  NRF_GPIO->PIN_CNF[g_APinDescription[_uc_pinSDA].ulPin] = ((GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)   \
                                                            | (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos)     \
                                                            | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)   \
                                                            | (GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos) \
                                                            | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos));
  NRF_GPIO->PIN_CNF[g_APinDescription[_uc_pinSCL].ulPin] = ((GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)   \
                                                            | (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos)     \
                                                            | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)   \
                                                            | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)  \
                                                            | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos));
   
  //set pins, frequency and enable peripheral
  nrf_twim_pins_set(_TWIInstance, g_APinDescription[_uc_pinSCL].ulPin, g_APinDescription[_uc_pinSDA].ulPin);
  nrf_twim_frequency_set(_TWIInstance, TWI_CLOCK);
  nrf_twim_enable(_TWIInstance);
  //set master mode
  _master=1;
}

void TwoWire::begin(uint8_t address) {
  //Slave mode
  _TWISlave=NRF_TWIS1;
  NRF_GPIO->PIN_CNF[g_APinDescription[_uc_pinSDA].ulPin] = ((GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)     \
                                                            | (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos)       \
                                                            | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)     \
                                                            | (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) \
                                                            | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos));
  NRF_GPIO->PIN_CNF[g_APinDescription[_uc_pinSCL].ulPin] = ((GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)     \
                                                            | (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos)       \
                                                            | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)     \
                                                            | (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) \
                                                            | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos));

  //set pins, address and enable peripheral
  nrf_twis_pins_set(_TWISlave, g_APinDescription[_uc_pinSCL].ulPin, g_APinDescription[_uc_pinSDA].ulPin);
  nrf_twis_address_set(_TWISlave, 0, address);
  
  
  nrf_twis_event_clear(_TWISlave, NRF_TWIS_EVENT_READ);
  nrf_twis_event_clear(_TWISlave, NRF_TWIS_EVENT_WRITE);
  nrf_twis_int_enable(_TWISlave, NRF_TWIS_INT_WRITE_MASK | NRF_TWIS_INT_READ_MASK);
	 
  //enable interrupt for read and write request event
  NVIC_SetPriority(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn, 2); //high priority
  NVIC_ClearPendingIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);
  NVIC_EnableIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);

  //enable peripheral
  nrf_twis_enable(_TWISlave);
  //set slave mode
  _master=0;
}

void TwoWire::setClock(uint32_t baudrate) {

  nrf_twim_frequency_t freq = NRF_TWIM_FREQ_100K; //default
  if(baudrate==250000)
	freq = NRF_TWIM_FREQ_250K;
  else
	if(baudrate==400000)
	  freq = NRF_TWIM_FREQ_400K;
  
  nrf_twim_disable(_TWIInstance);
  nrf_twim_frequency_set(_TWIInstance, freq);
  nrf_twim_enable(_TWIInstance);
}

void TwoWire::end() {
  if(_master)
	nrf_twim_disable(_TWIInstance);
  else
	nrf_twis_disable(_TWISlave);
}


uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit)
{
  if(quantity == 0)
    return 0;
  
  uint8_t i;
  //buffer for EasyDMA
  static uint8_t rx_buffer[SERIAL_BUFFER_SIZE];
  
  nrf_twim_rx_buffer_set(_TWIInstance, rx_buffer, quantity);
  nrf_twim_address_set(_TWIInstance, address);

  if(stopBit)
	//trigger stop task immidiately after received last data
	nrf_twim_shorts_set(_TWIInstance, NRF_TWIM_SHORT_LASTRX_STOP_MASK);
  
  nrf_twim_event_clear(_TWIInstance, NRF_TWIM_EVENT_STOPPED);
  nrf_twim_event_clear(_TWIInstance, NRF_TWIM_EVENT_SUSPENDED);
  nrf_twim_event_clear(_TWIInstance, NRF_TWIM_EVENT_LASTRX);
  nrf_twim_event_clear(_TWIInstance, NRF_TWIM_EVENT_ERROR);

  nrf_twim_task_trigger(_TWIInstance, NRF_TWIM_TASK_RESUME);
  nrf_twim_task_trigger(_TWIInstance, NRF_TWIM_TASK_STARTRX);
  if(stopBit){
	//wait transmission's end and check for errors
    while(!nrf_twim_event_check(_TWIInstance, NRF_TWIM_EVENT_STOPPED)){
	  if(nrf_twim_event_check(_TWIInstance, NRF_TWIM_EVENT_ERROR))
		//TWI master does not stop by itself when an error occurs.
		//The stop task must to be issued as part of the error handler.
		//Reference on page 304 of datasheet
	    nrf_twim_task_trigger(_TWIInstance, NRF_TWIM_TASK_STOP);
	  }
	suspended=0;
  }
  else{
	//wait last byte, then suspend twi instead of stop it
	//so no stop bit will be trasmitted
	while(!nrf_twim_event_check(_TWIInstance, NRF_TWIM_EVENT_LASTRX)){
	  if(nrf_twim_event_check(_TWIInstance, NRF_TWIM_EVENT_ERROR)){
		nrf_twim_task_trigger(_TWIInstance, NRF_TWIM_TASK_STOP);
		//if an error occurs exit from this while or it will loop forever
		break;
	  }		
	}
	nrf_twim_task_trigger(_TWIInstance, NRF_TWIM_TASK_SUSPEND);
    while(!nrf_twim_event_check(_TWIInstance, NRF_TWIM_EVENT_SUSPENDED));
	suspended=1;
  }
  
  //fill ring buffer with read data
  for(i=0; i<quantity; i++)
	rxBuffer.store_char(rx_buffer[i]);

  return quantity;
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity)
{
  return requestFrom(address, quantity, true);
}

void TwoWire::beginTransmission(uint8_t address) {
  // save address of target and clear buffer
  txAddress = address;
  txBuffer.clear();

  transmissionBegun = true;
}


// Errors:
//  0 : Success
//  1 : Data too long
//  2 : NACK on transmit of address
//  3 : NACK on transmit of data
//  4 : Other error
uint8_t TwoWire::endTransmission(bool stopBit)
{
  uint8_t size;
  transmissionBegun = false ;

  //tx buffer for EasyDMA
  static uint8_t tx_buffer[SERIAL_BUFFER_SIZE];
  
  //copy the data to send in the buffer
  for(size=0; txBuffer.available(); size++)
	  tx_buffer[size]=txBuffer.read_char();
  
  nrf_twim_tx_buffer_set(_TWIInstance, tx_buffer, size);
  nrf_twim_address_set(_TWIInstance, txAddress);
  if(stopBit)
	//trigger stop task immidiately after received last data
	nrf_twim_shorts_set(_TWIInstance, NRF_TWIM_SHORT_LASTTX_STOP_MASK);
  else
	//suspend twi instead of stop it so no stop bit will be trasmitted
	nrf_twim_shorts_set(_TWIInstance, NRF_TWIM_SHORT_LASTTX_SUSPEND_MASK);

  nrf_twim_event_clear(_TWIInstance, NRF_TWIM_EVENT_STOPPED);
  nrf_twim_event_clear(_TWIInstance, NRF_TWIM_EVENT_ERROR);
  nrf_twim_event_clear(_TWIInstance, NRF_TWIM_EVENT_SUSPENDED);
  nrf_twim_event_clear(_TWIInstance, NRF_TWIM_EVENT_LASTTX);
  
  nrf_twim_task_trigger(_TWIInstance, NRF_TWIM_TASK_RESUME);  
  nrf_twim_task_trigger(_TWIInstance, NRF_TWIM_TASK_STARTTX);
  if(stopBit){
	//wait transmission's end and check for errors
    while(!nrf_twim_event_check(_TWIInstance, NRF_TWIM_EVENT_STOPPED)){
	    if(nrf_twim_event_check(_TWIInstance, NRF_TWIM_EVENT_ERROR)){
		   //TWI master does not stop by itself when an error occurs.
		   //The stop task must to be issued as part of the error handler.
		   //Reference on page 304 of datasheet
		   nrf_twim_task_trigger(_TWIInstance, NRF_TWIM_TASK_STOP);
		   if(nrf_twim_errorsrc_get_and_clear(_TWIInstance)==NRF_TWIM_ERROR_ADDRESS_NACK)
			  return 2;
		  else
			  return 3;
		}
	}
	suspended=0;
  }
  else{
	  while(!nrf_twim_event_check(_TWIInstance, NRF_TWIM_EVENT_SUSPENDED)){
	    if(nrf_twim_event_check(_TWIInstance, NRF_TWIM_EVENT_ERROR)){
		  nrf_twim_task_trigger(_TWIInstance, NRF_TWIM_TASK_STOP);
		  if(nrf_twim_errorsrc_get_and_clear(_TWIInstance)==NRF_TWIM_ERROR_ADDRESS_NACK)
			  return 2;
		  else
			  return 3;
		}
	  }
	suspended=1;
  }
  return 0;  
}

uint8_t TwoWire::endTransmission()
{
  return endTransmission(true);
}


size_t TwoWire::write(uint8_t ucData)
{
  if(_master){
    // No writing, without begun transmission or a full buffer
    if ( !transmissionBegun || txBuffer.isFull() )
    {
      return 0 ;
    }

    txBuffer.store_char( ucData ) ;

    return 1 ;
  }
  else{
	//buffer for EasyDMA
	static uint8_t tx_buffer;
	tx_buffer=ucData;
	
	
	nrf_twis_event_clear(_TWISlave, NRF_TWIS_EVENT_TXSTARTED);
	//set buffer and trigger "prepare tx" task
	nrf_twis_tx_prepare(_TWISlave, &tx_buffer, 1);
	
	while(!nrf_twis_event_check(_TWISlave, NRF_TWIS_EVENT_TXSTARTED));

    return 1;
   
   }

   return 0;
}


size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
  if(_master){	//master mode
    // Try to store all data
    for(size_t i = 0; i < quantity; ++i)
    {
      //Return the number of data stored, when the buffer is full (if write return 0)
      if(!write(data[i]))
        return i;
    }
  }
  else{	//slave mode
    int i;
    //buffer for EasyDMA
    static uint8_t * tx_buffer;
	tx_buffer=(uint8_t *)malloc(sizeof(uint8_t)*quantity);
    //copy the content of data in tx_buffer
	for(i=0; i<quantity; i++)
		tx_buffer[i]=data[i];
	
	nrf_twis_event_clear(_TWISlave, NRF_TWIS_EVENT_TXSTARTED);
	//set buffer and trigger "prepare tx" task
	nrf_twis_tx_prepare(_TWISlave, tx_buffer, quantity);
	
	while(!nrf_twis_event_check(_TWISlave, NRF_TWIS_EVENT_TXSTARTED));
  }
  //All data stored
  return quantity;
}

int TwoWire::available(void)
{
  return rxBuffer.available();
}

int TwoWire::read(void)
{
  return rxBuffer.read_char();
}

int TwoWire::peek(void)
{
  return rxBuffer.peek();
}

void TwoWire::flush(void)
{
  // Do nothing, use endTransmission(..) to force
  // data transfer.
}

void TwoWire::onReceive(void(*function)(int))
{
  onReceiveCallback = function;
}

void TwoWire::onRequest(void(*function)(void))
{
  onRequestCallback = function;
}

void TwoWire::onService(void)
{
  if(!_master){ //execute this function only if we are slave
	if(nrf_twis_event_check(_TWISlave, NRF_TWIS_EVENT_WRITE)){
	  int i;
	  //master want to write
	  nrf_twis_event_clear(_TWISlave, NRF_TWIS_EVENT_WRITE);
	  //read message from master
	  //rx buffer for EasyDMA
	  static uint8_t rx_buffer[SERIAL_BUFFER_SIZE];
	  //set buffer and trigger "prepare rx" task
	  nrf_twis_rx_prepare(_TWISlave, rx_buffer, SERIAL_BUFFER_SIZE);
	  //wait the end of transmission
	  while(!nrf_twis_event_check(_TWISlave, NRF_TWIS_EVENT_STOPPED));
	
	  //check how many byte were received
	  //if more than SERIAL_BUFFER_SIZE were received they were automatically NACKed
	  uint8_t received=nrf_twis_rx_amount_get(_TWISlave);
	  //save received data in RingBuffer instance
	  for(i=0; i<received; i++)
	    rxBuffer.store_char(rx_buffer[i]);
	  
	  //Calling onReceiveCallback, if exists
      if(onReceiveCallback)
      {
        onReceiveCallback(available());
      }
	  
	  rxBuffer.clear();
	}
	else if(nrf_twis_event_check(_TWISlave, NRF_TWIS_EVENT_READ)){

	  //It's a request, master want to read
	  nrf_twis_event_clear(_TWISlave, NRF_TWIS_EVENT_READ);
	  //Calling onRequestCallback, if exists
      if(onRequestCallback)
      {
        onRequestCallback();
      }
	}
  }	
}

TwoWire Wire(PIN_WIRE_SDA, PIN_WIRE_SCL);

// define Wire1 for Primo Core only
#ifdef ARDUINO_NRF52_PRIMO_CORE
TwoWire Wire1(PIN_WIRE_SDA1, PIN_WIRE_SCL1);
#endif //ARDUINO_NRF52_PRIMO_CORE

#ifdef __cplusplus
extern "C" {
#endif

void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void){ 
	Wire.onService();
}

#ifdef __cplusplus
}
#endif