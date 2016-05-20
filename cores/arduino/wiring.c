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

#include "nrf_rtc.h"
#include "nrf_clock.h"

/*
 * Arduino Primo board initialization
 *
 * note: using of RTC tick event rather than SysTick allows the
 * CPU to be powered down.
 * NRF52 has 3 RTC instance, here RTC0 is used
 */
 void init( void )
{

	//LFCLK needed to be started before using the RTC
	//select source for LFCLK
	nrf_clock_xtalfreq_set(NRF_CLOCK_XTALFREQ_Default);
	nrf_clock_lf_src_set(NRF_CLOCK_LFCLK_Xtal);
	//start LFCLK
	nrf_clock_task_trigger(NRF_CLOCK_TASK_LFCLKSTART);

	//set prescaler value befor start RTC
	//f = 32.768 / (PRESCALER +1)
	//PRESCALER=32 to have ticks every 1 ms
	// nrf_rtc_prescaler_set(NRF_RTC0, 1024);
	
	//enable tick event and interrupt
	// NVIC_SetPriority(RTC0_IRQn, 6); //low priority
    // NVIC_ClearPendingIRQ(RTC0_IRQn);
    // NVIC_EnableIRQ(RTC0_IRQn);
	// nrf_rtc_event_clear(NRF_RTC0, NRF_RTC_EVENT_TICK);
	// nrf_rtc_event_enable(NRF_RTC0, NRF_RTC_EVENT_TICK);
	// nrf_rtc_int_enable(NRF_RTC0, NRF_RTC_INT_TICK_MASK);
	
	// start rtc
	// nrf_rtc_task_trigger(NRF_RTC0, NRF_RTC_TASK_START);	
}

#ifdef __cplusplus
}
#endif
