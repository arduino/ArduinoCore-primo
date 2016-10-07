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
	sd_power_system_off();
		
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
	uint32_t guilty;
	sd_power_reset_reason_get(&guilty);
	if(guilty & isGPIOMask){
		//RESETREAS is a cumulative register. We need to clear it by writing 1 in the relative field
		sd_power_reset_reason_clr(1 << 16);
		return GPIOReset;
	}
	if(guilty & isNFCMask){
		sd_power_reset_reason_clr(1 << 19);
		return NFCReset;
	}
	if(guilty & isCompMask){	
		sd_power_reset_reason_clr(1 << 17);
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
		nrf_timer_mode_set(NRF_TIMER2, NRF_TIMER_MODE_TIMER);
		nrf_timer_bit_width_set(NRF_TIMER2, NRF_TIMER_BIT_WIDTH_32);
		//When fTIMER <= 1 MHz the TIMER will use PCLK1M instead of PCLK16M for reduced power consumption.
		nrf_timer_frequency_set(NRF_TIMER2, NRF_TIMER_FREQ_1MHz);
		//Clear the timer when it finishes to count
		nrf_timer_shorts_enable(NRF_TIMER2, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK);
		
		//enable interrupt
		nrf_timer_int_enable(NRF_TIMER2, NRF_TIMER_INT_COMPARE0_MASK);
		NVIC_SetPriority(TIMER2_IRQn, 6); //low priority
		NVIC_ClearPendingIRQ(TIMER2_IRQn);
		NVIC_EnableIRQ(TIMER2_IRQn);
		
		uint32_t ticks=nrf_timer_ms_to_ticks(msec, NRF_TIMER_FREQ_1MHz);
		nrf_timer_cc_write(NRF_TIMER2, NRF_TIMER_CC_CHANNEL0, ticks);
		nrf_timer_task_trigger(NRF_TIMER2, NRF_TIMER_TASK_START);
	}
		
		
	if(mode==CONST_LATENCY)
		sd_power_mode_set(NRF_POWER_MODE_CONSTLAT);
	else
		sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
		
	while(!event){
			sd_app_evt_wait();
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
		nrf_rtc_prescaler_set(NRF_RTC1, 4095);
		//enable interrupt
		NVIC_SetPriority(RTC1_IRQn, 2); //high priority
		NVIC_ClearPendingIRQ(RTC1_IRQn);
		NVIC_EnableIRQ(RTC1_IRQn);
		nrf_rtc_event_clear(NRF_RTC1, NRF_RTC_EVENT_COMPARE_0);
		nrf_rtc_int_enable(NRF_RTC1, NRF_RTC_INT_COMPARE0_MASK);
		//Ticks every 125 ms -> 8 ticks to get one second
		nrf_rtc_cc_set(NRF_RTC1, 0, sec*8);
		
		//start RTC
		nrf_rtc_task_trigger(NRF_RTC1, NRF_RTC_TASK_START);
	}
	
	if(mode==CONST_LATENCY)
		sd_power_mode_set(NRF_POWER_MODE_CONSTLAT);
	else
		sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
		
	while(!event){
			sd_app_evt_wait();
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

void TIMER2_IRQHandler(void){
	nrf_timer_event_clear(NRF_TIMER2, NRF_TIMER_EVENT_COMPARE0);
	nrf_timer_task_trigger(NRF_TIMER2, NRF_TIMER_TASK_STOP);
	event=true;
	if(LowPower.functionCallback)
		LowPower.functionCallback();
}

void RTC1_IRQHandler(void)
{
	event=true;
	nrf_rtc_event_clear(NRF_RTC1, NRF_RTC_EVENT_COMPARE_0);
	nrf_rtc_task_trigger(NRF_RTC1, NRF_RTC_TASK_CLEAR);
	nrf_rtc_task_trigger(NRF_RTC1, NRF_RTC_TASK_STOP);
	if(LowPower.functionCallback)
		LowPower.functionCallback();		
}

#ifdef __cplusplus
}
#endif
