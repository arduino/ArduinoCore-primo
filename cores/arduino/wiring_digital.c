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


#include "wiring_digital.h"
#include "WVariant.h"
#include "nrf_gpio.h"

#include "nrf_twim.h"
#include "nrf_twis.h"
#include "variant.h"
#include "nrf52.h"

#ifdef __cplusplus
 extern "C" {
#endif
/////////////////////////////////////////////////////////////////////
NRF_TWIM_Type * _TWIInstance;
NRF_TWIS_Type * _TWISlave;

// TWI clock frequency
static const nrf_twim_frequency_t TWI_CLOCK = NRF_TWIM_FREQ_100K;

uint8_t _master=1;	//1 = master - 0 = slave

//TX buffer
uint8_t txBuffer_size = 1;
uint8_t txBuffer[1];
uint8_t txAddress;
uint8_t read_char[] = {0,0};

bool transmissionBegun;
uint8_t suspended=0;

#ifdef ARDUINO_NRF52_PRIMO
void TwoWire_begin(void) {
   //Master Mode
   _TWIInstance=NRF_TWIM1;
   nrf_gpio_pin_dir_set(g_APinDescription[PIN_WIRE_SCL].ulPin, NRF_GPIO_PIN_DIR_INPUT);
   nrf_gpio_pin_dir_set(g_APinDescription[PIN_WIRE_SDA].ulPin, NRF_GPIO_PIN_DIR_INPUT);

   //set pins, frequency and enable peripheral
   nrf_twim_pins_set(_TWIInstance, g_APinDescription[PIN_WIRE_SCL].ulPin, g_APinDescription[PIN_WIRE_SDA].ulPin);
   nrf_twim_frequency_set(_TWIInstance, TWI_CLOCK);
   nrf_twim_enable(_TWIInstance);
   //set master mode
   _master=1;
}

void TwoWire_beginTransmission(uint8_t address) {
  txAddress = address;
  transmissionBegun = true;
}

// Errors:
//  0 : Success
//  1 : Data too long
//  2 : NACK on transmit of address
//  3 : NACK on transmit of data
//  4 : Other error
uint8_t TwoWire_sendTransmission(bool stopBit)
{

  transmissionBegun = false ;

  nrf_twim_tx_buffer_set(_TWIInstance, txBuffer, txBuffer_size);
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
	    if(nrf_twim_event_check(_TWIInstance, NRF_TWIM_EVENT_ERROR))
		  if(nrf_twim_errorsrc_get_and_clear(_TWIInstance)==NRF_TWIM_ERROR_ADDRESS_NACK)
			  return 2;
		  else
			  return 3;
	  }
	suspended=1;
  }
  return 0;
}

uint8_t TwoWire_endTransmission()
{
  return TwoWire_sendTransmission(true);
}

uint8_t TwoWire_write(uint8_t ucData)
{
  if(_master){
    // No writing, without begun transmission or a full buffer
    if ( !transmissionBegun )
    {
      return 0 ;
    }

    txBuffer[0]= ucData;

    return 1 ;
  }
  else{
	  txBuffer[0]= ucData;


	nrf_twis_event_clear(_TWISlave, NRF_TWIS_EVENT_TXSTARTED);
	//set buffer and trigger "prepare tx" task
	nrf_twis_tx_prepare(_TWISlave, &txBuffer, 1);

	while(!nrf_twis_event_check(_TWISlave, NRF_TWIS_EVENT_TXSTARTED));

    return 1;

   }

   return 0;
}

uint8_t TwoWire_requestFrom(uint8_t address, size_t quantity, bool stopBit)
{
  if(quantity == 0)
    return 0;

  uint8_t i;
  //buffer for EasyDMA
  static uint8_t rx_buffer[64];

  nrf_twim_rx_buffer_set(_TWIInstance, rx_buffer, quantity);
  nrf_twim_address_set(_TWIInstance, address);

  if(stopBit)
	//trigger stop task immidiately after received last data
	nrf_twim_shorts_set(_TWIInstance, NRF_TWIM_SHORT_LASTRX_STOP_MASK);

  nrf_twim_event_clear(_TWIInstance, NRF_TWIM_EVENT_STOPPED);
  nrf_twim_event_clear(_TWIInstance, NRF_TWIM_EVENT_SUSPENDED);
  nrf_twim_event_clear(_TWIInstance, NRF_TWIM_EVENT_LASTRX);
  nrf_twim_event_clear(_TWIInstance, NRF_TWIM_EVENT_ERROR);

  nrf_twim_task_trigger(_TWIInstance, NRF_TWIM_TASK_STARTRX);
  if(suspended)
	nrf_twim_task_trigger(_TWIInstance, NRF_TWIM_TASK_RESUME);
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
	  if(nrf_twim_event_check(_TWIInstance, NRF_TWIM_EVENT_ERROR))
		//if an error occurs exit from this while or it will loop forever
		break;
	}
	nrf_twim_task_trigger(_TWIInstance, NRF_TWIM_TASK_SUSPEND);
    while(!nrf_twim_event_check(_TWIInstance, NRF_TWIM_EVENT_SUSPENDED));
	suspended=1;
  }

  //fill ring buffer with read data
  for(i=0; i<quantity; i++)
	  read_char[i] = rx_buffer[i];

  return quantity;
}


int TwoWire_read(void)
{
	if (read_char[0] == read_char[1])
	{
		return read_char[0];
	}
	else
	{
		return -1;
	}
}

#endif //ARDUINO_NRF52_PRIMO
///////////////////////////////////////////////////////////////////////

void pinMode( uint32_t ulPin, uint32_t ulMode )
{

#ifdef ARDUINO_NRF52_PRIMO

  if ( (ulPin == 34) && (ulMode == STM32_IT) )
  {
	  nrf_delay_ms(15);
	  TwoWire_begin();
	  TwoWire_beginTransmission(0x48);
	  TwoWire_write(GPIO_USER1_IT);
	  TwoWire_endTransmission();
	  return ;
  }

  if ( (ulPin == 43) && (ulMode == INPUT) )
  {
	  nrf_delay_ms(15);
	  TwoWire_begin();
	  TwoWire_beginTransmission(0x48);
	  TwoWire_write(BAT_VOLT_IN);
	  TwoWire_endTransmission();
	  return ;
  }
  
#endif //ARDUINO_NRF52_PRIMO

  if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
  {
    return ;
  }

  switch ( ulMode )
  {
    case INPUT:
      // Set pin to input mode
	  nrf_gpio_pin_dir_set(g_APinDescription[ulPin].ulPin, NRF_GPIO_PIN_DIR_INPUT);
    break ;

    case INPUT_PULLUP:
      // Set pin to input mode with pull-up resistor enabled
	  nrf_gpio_cfg_input(g_APinDescription[ulPin].ulPin, NRF_GPIO_PIN_PULLUP);     
	break ;
	
	case INPUT_PULLDOWN:
      // Set pin to input mode with pull-down resistor enabled
	  nrf_gpio_cfg_input(g_APinDescription[ulPin].ulPin, NRF_GPIO_PIN_PULLDOWN);
	break ;

    case OUTPUT:
      // Set pin to output mode
	  nrf_gpio_pin_dir_set(g_APinDescription[ulPin].ulPin, NRF_GPIO_PIN_DIR_OUTPUT);
		 
	  #ifdef ARDUINO_NRF52_PRIMO_CORE
	  // LEDs on Core work with reverse logic. Turn them off if output mode is selected
	  if(ulPin == 10 || ulPin == 11 || ulPin == 12 || ulPin == 13)
		digitalWrite(ulPin, LOW);
	  #endif //ARDUINO_NRF52_PRIMO_CORE
	break ;

    default:
      // do nothing
    break ;

  }
}

void digitalWrite( uint32_t ulPin, uint32_t ulVal )
{

// in Primo Core leds have an inverted logic. Reverse ulVal in order to restore the normal logic
#ifdef ARDUINO_NRF52_PRIMO_CORE
	if(ulPin == 10 || ulPin == 11 || ulPin == 12 || ulPin == 13)
		ulVal = !ulVal;
#endif //ARDUINO_NRF52_PRIMO_CORE

#ifdef ARDUINO_NRF52_PRIMO

	switch ( ulPin )
	{
		case 38:
			nrf_delay_ms(15);
			if(ulVal == HIGH)
			{
				TwoWire_begin();
				TwoWire_beginTransmission(0x48);
				TwoWire_write(USER2_LED_H);
				TwoWire_endTransmission();
			}
			else if (ulVal == LOW)
			{
				 TwoWire_begin();
				 TwoWire_beginTransmission(0x48);
				 TwoWire_write(USER2_LED_L);
				 TwoWire_endTransmission();
			}
		break ;

		case 39:
			nrf_delay_ms(15);
			if(ulVal == HIGH)
			{
				 TwoWire_begin();
				 TwoWire_beginTransmission(0x48);
				 TwoWire_write(POWER_LED_H);
				 TwoWire_endTransmission();
			}
			else if (ulVal == LOW)
			{
				 TwoWire_begin();
				 TwoWire_beginTransmission(0x48);
				 TwoWire_write(POWER_LED_L);
				 TwoWire_endTransmission();
			 }
		break ;

		case 40:
			nrf_delay_ms(15);
			if(ulVal == HIGH)
			{
				 TwoWire_begin();
				 TwoWire_beginTransmission(0x48);
				 TwoWire_write(BLE_LED_H);
				 TwoWire_endTransmission();
			}
			else if (ulVal == LOW)
			{
				 TwoWire_begin();
				 TwoWire_beginTransmission(0x48);
				 TwoWire_write(BLE_LED_L);
				 TwoWire_endTransmission();
			 }
		break ;

		case 41:
			nrf_delay_ms(15);
			if(ulVal == HIGH)
			{
				 TwoWire_begin();
				 TwoWire_beginTransmission(0x48);
				 TwoWire_write(GPIO_ESP_PW_H);
				 TwoWire_endTransmission();
			}
			else if (ulVal == LOW)
			{
				 TwoWire_begin();
				 TwoWire_beginTransmission(0x48);
				 TwoWire_write(GPIO_ESP_PW_L);
				 TwoWire_endTransmission();
			 }
		break ;

		case 42:
			nrf_delay_ms(15);
			if(ulVal == HIGH)
			{
				 TwoWire_begin();
				 TwoWire_beginTransmission(0x48);
				 TwoWire_write(GPIO_ESP_EN_H);
				 TwoWire_endTransmission();
			}
			else if (ulVal == LOW)
			{
				 TwoWire_begin();
				 TwoWire_beginTransmission(0x48);
				 TwoWire_write(GPIO_ESP_EN_L);
				 TwoWire_endTransmission();
			 }
		break ;

    	default:
    		// do nothing
    	break ;
	}

#endif //ARDUINO_NRF52_PRIMO

  if ( ulPin >= 38 && ulPin <= 42)
  {
	  return ;
  }

  /* Handle the case the pin isn't usable as PIO */
  if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
  {
    return ;
  }

  if(ulVal == LOW)
	  nrf_gpio_pin_clear(g_APinDescription[ulPin].ulPin);
  else
	  nrf_gpio_pin_set(g_APinDescription[ulPin].ulPin);

  return ;
}

int digitalRead( uint32_t ulPin )
{

#ifdef ARDUINO_NRF52_PRIMO

   switch ( ulPin )
	{
   	   case 44:
			 nrf_delay_ms(15);
			 TwoWire_begin();
			 TwoWire_beginTransmission(0x48);
			 TwoWire_write(USER2_BUTTON_IN);
			 TwoWire_endTransmission();
			 nrf_delay_ms(15);
			 TwoWire_requestFrom(0x48, 2, true);
			 char c = TwoWire_read();
			 if (c == 0xC3)
			 {
				 return HIGH ;
			 }
			 else if (c == 0x3C)
				 return LOW ;
			 else
				 return -1;
   	   break ;
   	   default:
   		// do nothing
   	   break ;
	}

#endif //ARDUINO_NRF52_PRIMO

  /* Handle the case the pin isn't usable as PIO */
  if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
  {
    return LOW ;
  }

  if ( (nrf_gpio_pin_read(g_APinDescription[ulPin].ulPin)) != 0 )
  {
    return HIGH ;
  }

  return LOW ;
}

#ifdef __cplusplus
}
#endif
