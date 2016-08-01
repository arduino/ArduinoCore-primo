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

volatile bool event = false;

nrf_lpcomp_input_t aPin[]={NRF_LPCOMP_INPUT_1, NRF_LPCOMP_INPUT_2, NRF_LPCOMP_INPUT_4, NRF_LPCOMP_INPUT_5, NRF_LPCOMP_INPUT_6, NRF_LPCOMP_INPUT_7};

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

void LowPowerClass::wakeUpByGPIO(uint8_t pinNo, uint8_t level){
	//Let the pin be sensitive to specified level
	if(level==LOW)
		nrf_gpio_cfg_sense_input(g_APinDescription[pinNo].ulPin, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
	else
		nrf_gpio_cfg_sense_input(g_APinDescription[pinNo].ulPin, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
}

void LowPowerClass::wakeUpByNFC(){
	NRF_NFCT->TASKS_SENSE=1;
}

void LowPowerClass::wakeUpByComp(uint8_t pin, nrf_lpcomp_ref_t reference, detect_mode mode){
	nrf_lpcomp_config_t config={reference, (nrf_lpcomp_detect_t)mode};
	nrf_lpcomp_configure(&config);
	if(pin<14 && pin>19)
		return;	//no analog pin is choosen
	nrf_lpcomp_input_select(aPin[pin-14]);
	nrf_lpcomp_enable();
	nrf_lpcomp_task_trigger(NRF_LPCOMP_TASK_START);
	while(!nrf_lpcomp_event_check(NRF_LPCOMP_EVENT_READY));
}

resetReason LowPowerClass::whoIs(){
	uint32_t guilty = NRF_POWER->RESETREAS;
	if(guilty & isGPIOMask){
		//RESETREAS is a cumulative register. We need to clear it by writing 1 in the relative field
		NRF_POWER->RESETREAS = (1 << 16);
		return GPIOReset;
	}
	if(guilty & isNFCMask){
		NRF_POWER->RESETREAS = (1 << 19);
		return NFCReset;
	}
	if(guilty & isCompMask){	
		NRF_POWER->RESETREAS = (1 << 17);
		return CompReset;
	}
	return OTHER;
}

void LowPowerClass::standbyMsec(uint32_t msec, void(*function)(void), standbyType mode){

	//register callback
	functionCallback=function;
	
	event=false;
	
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
	
	while(!event){
		__WFI();
		__WFE();
	}
}


void LowPowerClass::standbyMsec(uint32_t msec, void(*function)(void)){
	standbyMsec(msec, function, LOW_POWER);
}

void LowPowerClass::standby(uint32_t sec, void(*function)(void), standbyType mode){
	//register callback
	functionCallback=function;
	
	event=false;
	
	if(sec>0){
		//LFCLK needed to be started before using the RTC
		nrf_clock_xtalfreq_set(NRF_CLOCK_XTALFREQ_Default);
		nrf_clock_lf_src_set((nrf_clock_lfclk_t)NRF_CLOCK_LFCLK_Xtal);
		nrf_clock_task_trigger(NRF_CLOCK_TASK_LFCLKSTART);
		nrf_rtc_prescaler_set(NRF_RTC2, 4095);
		//enable interrupt
		NVIC_SetPriority(RTC2_IRQn, 2); //high priority
		NVIC_ClearPendingIRQ(RTC2_IRQn);
		NVIC_EnableIRQ(RTC2_IRQn);
		nrf_rtc_event_clear(NRF_RTC2, NRF_RTC_EVENT_COMPARE_0);
		nrf_rtc_int_enable(NRF_RTC2, NRF_RTC_INT_COMPARE0_MASK);
		//Ticks every 125 ms -> 8 ticks to get one second
		nrf_rtc_cc_set(NRF_RTC2, 0, sec*8);
		
		//start RTC
		nrf_rtc_task_trigger(NRF_RTC2, NRF_RTC_TASK_START);
	}
	if(mode==CONST_LATENCY)
		NRF_POWER->TASKS_CONSTLAT=1UL;
	else
		NRF_POWER->TASKS_LOWPWR=1UL;
	
	while(!event){
		__WFI();
		__WFE();	
	}

}

void LowPowerClass::standby(uint32_t sec, void(*function)(void)){
	standby(sec, function, LOW_POWER);
}

void LowPowerClass::wakeUpNow(){
	event=true;
}

LowPowerClass LowPower;

#ifdef __cplusplus
extern "C"{
#endif	

void TIMER0_IRQHandler(void){
	nrf_timer_event_clear(NRF_TIMER0, NRF_TIMER_EVENT_COMPARE0);
	event=true;
	if(LowPower.functionCallback)
		LowPower.functionCallback();
	
}

void RTC2_IRQHandler(void)
{
	event=true;
	nrf_rtc_event_clear(NRF_RTC2, NRF_RTC_EVENT_COMPARE_0);
	nrf_rtc_task_trigger(NRF_RTC2, NRF_RTC_TASK_CLEAR);
	nrf_rtc_task_trigger(NRF_RTC2, NRF_RTC_TASK_STOP);
	if(LowPower.functionCallback)
		LowPower.functionCallback();		
}

#ifdef __cplusplus
}
#endif
