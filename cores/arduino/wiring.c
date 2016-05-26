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

#include "variant.h"
#include "wiring_digital.h"
#include "wiring.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Arduino Primo board initialization
 *
 * Configure SysTick in order to tick every 1 ms.
 * needed for millis function.
 */
 void init( void )
{
	// Set Systick to 1ms interval, common to all Cortex-M variants
	if ( SysTick_Config( SystemCoreClock / 1000 ) )
	{
		// Capture error
		while ( 1 ) ;
	}
}

#ifdef __cplusplus
}
#endif
