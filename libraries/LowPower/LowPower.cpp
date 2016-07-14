/*
  LowPower class for nRF52.
  Written by Chiara Ruggeri (chiara@arduino.org)
  
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

  Enjoy!  
*/


#include "LowPower.h"


void LowPowerClass::powerOFF(){
	//Enter in systemOff mode only when no EasyDMA transfer is active
	//this is achieved by disabling all peripheral that use it
	NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Disabled;								//disable UART
	NRF_SAADC ->ENABLE = (SAADC_ENABLE_ENABLE_Disabled << SAADC_ENABLE_ENABLE_Pos);	//disable ADC
	NRF_PWM0  ->ENABLE = (PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos);		//disable all pwm instance
	NRF_PWM1  ->ENABLE = (PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos);
	NRF_PWM2  ->ENABLE = (PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos);
	NRF_TWIM1 ->ENABLE = (TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos);	//disable TWI Master
	NRF_TWIS1 ->ENABLE = (TWIS_ENABLE_ENABLE_Disabled << TWIS_ENABLE_ENABLE_Pos);	//disable TWI Slave
	
	//Enter in System OFF mode
	NRF_POWER->SYSTEMOFF = POWER_SYSTEMOFF_SYSTEMOFF_Enter;
		
	/*Only for debugging purpose, will not be reached without connected debugger*/
    while(1);
}

void LowPowerClass::WakeUpByGPIO(uint8_t pinNo, uint8_t level){
	//Let the pin be sensitive to specified level
	if(level==LOW)
		nrf_gpio_cfg_sense_input(g_APinDescription[pinNo].ulPin, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
	else
		nrf_gpio_cfg_sense_input(g_APinDescription[pinNo].ulPin, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
}

void LowPowerClass::WakeUpByNFC(){
	NRF_NFCT->TASKS_SENSE=1;
}

uint32_t LowPowerClass::WhoIs(){
	uint32_t guilty = NRF_POWER->RESETREAS;
	if(guilty & isGPIOMask){
		//RESETREAS is a cumulative register. We need to clear it by writing 1 in the relative field
		NRF_POWER->RESETREAS = (1 << 16);
		return 1;
	}
	if(guilty & isNFCMask){
		NRF_POWER->RESETREAS = (1 << 19);
		return 2;
	}
	return 0;
}

void LowPowerClass::Idle(uint32_t msec, void(*function)(void), idleType mode){

	//register callback
	functionCallback=function;
	
	if(msec>0){
		//Configure timer
		nrf_timer_mode_set(NRF_TIMER0, NRF_TIMER_MODE_TIMER);
		nrf_timer_bit_width_set(NRF_TIMER0, NRF_TIMER_BIT_WIDTH_32);
		//When fTIMER <= 1 MHz the TIMER will use PCLK1M instead of PCLK16M for reduced power consumption.
		nrf_timer_frequency_set(NRF_TIMER0, NRF_TIMER_FREQ_1MHz);
		//Clear the timer when it finishes to count
		nrf_timer_shorts_enable(NRF_TIMER0, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK);
		
		//enable interrupt
		nrf_timer_int_enable(NRF_TIMER0, NRF_TIMER_INT_COMPARE0_MASK);
		NVIC_SetPriority(TIMER0_IRQn, 6); //low priority
		NVIC_ClearPendingIRQ(TIMER0_IRQn);
		NVIC_EnableIRQ(TIMER0_IRQn);
		
		uint32_t ticks=nrf_timer_ms_to_ticks(msec, NRF_TIMER_FREQ_1MHz);
		nrf_timer_cc_write(NRF_TIMER0, NRF_TIMER_CC_CHANNEL0, ticks);
		nrf_timer_task_trigger(NRF_TIMER0, NRF_TIMER_TASK_START);
	}
	
	if(mode==CONST_LATENCY)
		NRF_POWER->TASKS_CONSTLAT=1UL;
	else
		NRF_POWER->TASKS_LOWPWR=1UL;
	
	while(1){
		__WFI();
		__WFE();
		
	}
}


void LowPowerClass::Idle(uint32_t msec, void(*function)(void)){
	Idle(msec, function, LOW_POWER);
}


LowPowerClass LowPower;

#ifdef __cplusplus
extern "C"{
#endif	

void TIMER0_IRQHandler(void){
	nrf_timer_event_clear(NRF_TIMER0, NRF_TIMER_EVENT_COMPARE0);
	if(LowPower.functionCallback)
		LowPower.functionCallback();		
}

#ifdef __cplusplus
}
#endif
