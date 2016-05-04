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

#ifdef __cplusplus
 extern "C" {
#endif


void pinMode( uint32_t ulPin, uint32_t ulMode )
{
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

    case OUTPUT:
      // Set pin to output mode
		 nrf_gpio_pin_dir_set(g_APinDescription[ulPin].ulPin, NRF_GPIO_PIN_DIR_OUTPUT);
	  break ;

    default:
      // do nothing
    break ;
  }
}

void digitalWrite( uint32_t ulPin, uint32_t ulVal )
{
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