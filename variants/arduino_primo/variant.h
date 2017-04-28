/*
  Copyright (c) 2016 Arduino Srl.  All right reserved.

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

#ifndef _VARIANT_ARDUINO_PRIMO_
#define _VARIANT_ARDUINO_PRIMO_

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/


/** Frequency of the board main oscillator */
#define VARIANT_MAINOSC		(32768ul)

/** Master clock frequency */
#define VARIANT_MCK			  (64000000ul)

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
 #include "Uart.h"
#endif // __cplusplus

 #ifdef __cplusplus
extern "C"{
#endif // __cplusplus

/**
 * Libc porting layers
 */
#if defined (  __GNUC__  )
#    include <syscalls.h> /** RedHat Newlib minimal stub */
#endif

/*----------------------------------------------------------------------------
 *        Pins
 *----------------------------------------------------------------------------*/

// Number of pins defined in PinDescription array
#define PINS_COUNT           (38u)
#define NUM_DIGITAL_PINS     (22u)
#define NUM_ANALOG_INPUTS    (5u)
#define NUM_ANALOG_OUTPUTS   (1u)

#define digitalPinToPort(P)        ( NRF_P0 )
#define digitalPinToBitMask(P)     ( 1 << g_APinDescription[P].ulPin )
#define digitalPinToTimer(P)       ( )
#define portOutputRegister(port)   ( &(port->OUT) )
#define portInputRegister(port)    ( &(port->IN)  )
#define portModeRegister(port)     ( &(port->DIR) )
#define analogInPinToBit(P)        ( g_APinDescription[P].ulPin )
#define digitalPinHasPWM(P)        ( g_APinDescription[P].ulPWMChannel != NOT_ON_PWM )
#define digitalPinToInterrupt(P)   ( P )

#define PIN_LED_9            (9u)
#define PIN_LED              PIN_LED_9
#define LED_BUILTIN          PIN_LED_9
#define BUZZER				 (35u)
#define USER1_BUTTON		 (34u)

/*
 * STM GPIO
 */
#define USER2_LED		 (38u)
#define POWER_LED		 (39u)
#define BLE_LED		 	 (40u)
#define GPIO_ESP_PW		 (41u)
#define GPIO_ESP_EN		 (42u)
#define BAT_VOL		 	 (43u)
#define USER2_BUTTON	 (44u)

static const uint8_t USER2_LED_H   		= 0b11100010;
static const uint8_t USER2_LED_L    	= 0b11100011;
static const uint8_t POWER_LED_H   		= 0b11100100;
static const uint8_t POWER_LED_L    	= 0b11100101;
static const uint8_t BLE_LED_H 			= 0b11100110;
static const uint8_t BLE_LED_L  		= 0b11100111;
static const uint8_t GPIO_ESP_PW_H  	= 0b11101000;
static const uint8_t GPIO_ESP_PW_L  	= 0b11101001;
static const uint8_t GPIO_ESP_EN_H  	= 0b11101010;
static const uint8_t GPIO_ESP_EN_L  	= 0b11101011;
static const uint8_t BAT_VOLT_IN    = 0b11101100;
static const uint8_t USER2_BUTTON_IN  	= 0b11101110;
static const uint8_t GPIO_USER1_IT  	= 0b11111000;

/*
 * SPI Interfaces
 */

#define SPI_INTERFACES_COUNT 1


#define PIN_SPI_MOSI         (27u)
#define PIN_SPI_MISO         (24u)
#define PIN_SPI_SCK          (26u)

static const uint8_t SS	  = 10;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 1

#define PIN_WIRE_SDA         (20u)
#define PIN_WIRE_SCL         (21u)



/*
 * Analog pins
 */
static const uint8_t A0  = 14;
static const uint8_t A1  = 15;
static const uint8_t A2  = 16;
static const uint8_t A3  = 17;
static const uint8_t A4  = 18;
static const uint8_t A5  = 19;


#ifdef __cplusplus
}
#endif


/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/
#ifdef __cplusplus

extern Uart Serial;

#endif

#define SERIAL_PORT_MONITOR         Serial

#endif /* _VARIANT_ARDUINO_PRIMO_ */
