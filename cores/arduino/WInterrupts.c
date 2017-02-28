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

#include "WInterrupts.h"
#include "variant.h"
#include "wiring_digital.h"

#include "nrf_gpiote.h"
#include "nrf_gpio.h"

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

nrf_gpiote_events_t ch_evt[NUMBER_OF_GPIO_TE]={NRF_GPIOTE_EVENTS_IN_0,
											   NRF_GPIOTE_EVENTS_IN_1,
											   NRF_GPIOTE_EVENTS_IN_2,
											   NRF_GPIOTE_EVENTS_IN_3,
											   NRF_GPIOTE_EVENTS_IN_4,
											   NRF_GPIOTE_EVENTS_IN_5,
											   NRF_GPIOTE_EVENTS_IN_6,
											   NRF_GPIOTE_EVENTS_IN_7};
uint32_t int_msk[NUMBER_OF_GPIO_TE]={NRF_GPIOTE_INT_IN0_MASK,
									 NRF_GPIOTE_INT_IN1_MASK,
									 NRF_GPIOTE_INT_IN2_MASK,
									 NRF_GPIOTE_INT_IN3_MASK,
									 NRF_GPIOTE_INT_IN4_MASK,
									 NRF_GPIOTE_INT_IN5_MASK,
									 NRF_GPIOTE_INT_IN6_MASK,
									 NRF_GPIOTE_INT_IN7_MASK};
uint8_t freeChannels[NUMBER_OF_GPIO_TE]={1,1,1,1,1,1,1,1}; //0 not free - 1 free

static struct
{
  uint32_t _ulPin ;
  voidFuncPtr _callback ;
  nrf_gpiote_events_t _channel;
  uint32_t _mask;
} callbacksInt[NUMBER_OF_GPIO_TE] ;

/**
 * \brief ISR called for each event on pins
 *
 */
void GPIOTE_IRQHandler(void){
	//find the channel connected to this event
	uint8_t j;
	for(j=0; j<NUMBER_OF_GPIO_TE; j++)
		if(nrf_gpiote_event_is_set(callbacksInt[j]._channel))
			break;
	//disable related interrupt while ISR is served
	nrf_gpiote_event_clear(callbacksInt[j]._channel);
	nrf_gpiote_int_disable(callbacksInt[j]._mask);
	//call the related callback
	callbacksInt[j]._callback();
	//enable interrupt
    nrf_gpiote_int_enable(callbacksInt[j]._mask);
}

/**
 * \brief Specifies a named Interrupt Service Routine (ISR) to call when an interrupt occurs.
 *        Replaces any previous function that was attached to the interrupt.
 */

int attachInterrupt( uint32_t ulPin, voidFuncPtr callback, uint32_t ulMode )
{
  static int enabled = 0 ;
  uint32_t ulConfig ;
  uint32_t ulPos ;
  nrf_gpiote_polarity_t mode = NRF_GPIOTE_POLARITY_HITOLO;

  int j;
  //check for a free gpiote channel
  for(j=0; j<NUMBER_OF_GPIO_TE; j++)
	  if(freeChannels[j])
		  break;
	  
  //return if there aren't free channels available
  if(j==NUMBER_OF_GPIO_TE)
	return -1;

  //Assign callback to related interrupt and set the channel as busy
  callbacksInt[j]._ulPin = ulPin ;
  callbacksInt[j]._callback = callback ;
  callbacksInt[j]._channel=ch_evt[j];
  callbacksInt[j]._mask=int_msk[j];
  freeChannels[j]=0;
	
  //Configure the interrupt mode
  switch ( ulMode )
  { //gpiote channel does not support LOW and HIGH mode. These are manteined for compatibility
	case LOW:
		mode=NRF_GPIOTE_POLARITY_HITOLO; //same as FALLING
	break;
	
	case HIGH:
		mode=NRF_GPIOTE_POLARITY_LOTOHI; //same as RISING
	break;
	
	case CHANGE:
		mode=NRF_GPIOTE_POLARITY_TOGGLE;
	break;
	
	case FALLING:
		mode=NRF_GPIOTE_POLARITY_HITOLO;
	break;
	
	case RISING:
		mode=NRF_GPIOTE_POLARITY_LOTOHI;
	break;
  }
  
  //configure channel
  nrf_gpiote_event_configure(j, g_APinDescription[ulPin].ulPin, mode);
  //enable event on the specified channel
  nrf_gpiote_event_enable(j);
  //enable interrupt
  NVIC_SetPriority(GPIOTE_IRQn, 6); //low priority
  NVIC_ClearPendingIRQ(GPIOTE_IRQn);
  NVIC_EnableIRQ(GPIOTE_IRQn);
  nrf_gpiote_int_enable(int_msk[j]);
 return int_msk[j];
}

/**
 * \brief Turns off the given interrupt.
 */
void detachInterrupt( uint32_t ulPin )
{
	//search the channel connected to the pin
	uint8_t j;
	for(j=0; j<NUMBER_OF_GPIO_TE; j++)
		if(callbacksInt[j]._ulPin==ulPin)
			break;
	if(j==NUMBER_OF_GPIO_TE)
		return; //error - no channel found
	
	//disable event and interrupt and set the channel as free
	nrf_gpiote_event_disable(j);
	nrf_gpiote_int_disable(callbacksInt[j]._mask);
	freeChannels[j]=1;
}