/*
  PPI class for nRF52.
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


#include "PPI.h"

nrf_ppi_channel_t channels[20] = {NRF_PPI_CHANNEL0, NRF_PPI_CHANNEL1, NRF_PPI_CHANNEL2, NRF_PPI_CHANNEL3, NRF_PPI_CHANNEL4, NRF_PPI_CHANNEL5, NRF_PPI_CHANNEL6, NRF_PPI_CHANNEL7, NRF_PPI_CHANNEL8, NRF_PPI_CHANNEL9,
								  NRF_PPI_CHANNEL10, NRF_PPI_CHANNEL11, NRF_PPI_CHANNEL12, NRF_PPI_CHANNEL13, NRF_PPI_CHANNEL14, NRF_PPI_CHANNEL15, NRF_PPI_CHANNEL16, NRF_PPI_CHANNEL17, NRF_PPI_CHANNEL18, NRF_PPI_CHANNEL19};
nrf_gpiote_tasks_t gpio_taskNo[] = {NRF_GPIOTE_TASKS_OUT_0, NRF_GPIOTE_TASKS_OUT_1, NRF_GPIOTE_TASKS_OUT_2, NRF_GPIOTE_TASKS_OUT_3, NRF_GPIOTE_TASKS_OUT_4, NRF_GPIOTE_TASKS_OUT_5, NRF_GPIOTE_TASKS_OUT_6, NRF_GPIOTE_TASKS_OUT_7};
nrf_gpiote_events_t gpio_eventNo[] = {NRF_GPIOTE_EVENTS_IN_0, NRF_GPIOTE_EVENTS_IN_1, NRF_GPIOTE_EVENTS_IN_2, NRF_GPIOTE_EVENTS_IN_3, NRF_GPIOTE_EVENTS_IN_4, NRF_GPIOTE_EVENTS_IN_5, NRF_GPIOTE_EVENTS_IN_6, NRF_GPIOTE_EVENTS_IN_7};

uint8_t first_free_channel=0;
uint8_t gpiote_config_index=0;
uint8_t event_index;
uint8_t task_index;
								  
uint32_t milliSec=1000;
uint32_t outputPin=LED_BUILTIN;
nrf_lpcomp_ref_t reference=REF_VDD_1_2;
uint32_t inputPin=A0;


//public functions

int PPIClass::setShortcut(event_type event, task_type task){
	//check if there is still a free channel
	if(first_free_channel==20)
		return 0;
	
	switch(event & 0xF0){
		case 0x00:
			//event is related to timer
			configureTimer();
			switch(task & 0xF0){
				case 0x10: //task is related to GPIO
					configureGPIOTask(task);
					//enable PPI peripheral
					nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)nrf_timer_event_address_get(NRF_TIMER1, NRF_TIMER_EVENT_COMPARE0), nrf_gpiote_task_addr_get(gpio_taskNo[task_index]));
					nrf_ppi_channel_enable(channels[first_free_channel]);
				break;
				case 0x30: //task is related to NFC
					if((task & 0x0F)==0) //start sensing
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)nrf_timer_event_address_get(NRF_TIMER1, NRF_TIMER_EVENT_COMPARE0), 0x40005008);
					else	//stop sensing
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)nrf_timer_event_address_get(NRF_TIMER1, NRF_TIMER_EVENT_COMPARE0), 0x40005004);
					
					nrf_ppi_channel_enable(channels[first_free_channel]);
				break;
				default: //task not recognized
					return 0;
				break;
			}
			//start the timer
	 		nrf_timer_task_trigger(NRF_TIMER1, NRF_TIMER_TASK_START);
		break;
		case 0x10:
			//event is related to gpio
			configureGPIOEvent(event);
			switch(task & 0xF0){
				case 0x00: //task is related to timer
					if((task & 0x0F)==0){	//start task
						configureTimer();
						//enable PPI peripheral 
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)nrf_gpiote_event_addr_get(gpio_eventNo[event_index]), (uint32_t)nrf_timer_task_address_get(NRF_TIMER1, NRF_TIMER_TASK_START));
						nrf_ppi_channel_enable(channels[first_free_channel]);
					}
					else{	//stop task
						//enable PPI peripheral 
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)nrf_gpiote_event_addr_get(gpio_eventNo[event_index]), (uint32_t)nrf_timer_task_address_get(NRF_TIMER1, NRF_TIMER_TASK_STOP));
						nrf_ppi_channel_enable(channels[first_free_channel]);
					}
				break;
				case 0x10: //task is related to GPIO
					configureGPIOTask(task);
					//enable PPI peripheral 
					nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)nrf_gpiote_event_addr_get(gpio_eventNo[event_index]), nrf_gpiote_task_addr_get(gpio_taskNo[task_index]));
					nrf_ppi_channel_enable(channels[first_free_channel]);
				break;
				case 0x30: //task is related to NFC
					if((task & 0x0F)==0) //start sensing
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)nrf_gpiote_event_addr_get(gpio_eventNo[event_index]), 0x40005008);
					else //stop sensing
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)nrf_gpiote_event_addr_get(gpio_eventNo[event_index]), 0x40005004);
					nrf_ppi_channel_enable(channels[first_free_channel]);
				break;
				default: //task not recognized
					return 0;
				break;
			}
			//enable sensing
			nrf_gpiote_event_enable(event_index);
		break;
		case 0x20:
			//event is related to comparator
			nrf_lpcomp_event_t active_event;
			detect_mode mode;
			switch(event&0x0F){
				case 0:
					active_event=NRF_LPCOMP_EVENT_DOWN;
					mode=DOWN;
				break;
				case 1:
					active_event=NRF_LPCOMP_EVENT_UP;
					mode=UP;
				break;
				case 2:
					active_event=NRF_LPCOMP_EVENT_CROSS;
					mode=CROSS;
				break;
			}

			configureCompEvent(mode);
			switch(task & 0xF0){
				case 0x00: //task is related to timer
					if((task & 0x0F)==0){	//start task
						configureTimer();
						//enable PPI peripheral 
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)nrf_lpcomp_event_address_get(active_event), (uint32_t)nrf_timer_task_address_get(NRF_TIMER1, NRF_TIMER_TASK_START));
						nrf_ppi_channel_enable(channels[first_free_channel]);
					}
					else{	//stop task
						//enable PPI peripheral 
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)nrf_lpcomp_event_address_get(active_event), (uint32_t)nrf_timer_task_address_get(NRF_TIMER1, NRF_TIMER_TASK_STOP));
						nrf_ppi_channel_enable(channels[first_free_channel]);
					}
				break;
				case 0x10: //task is related to GPIO
					configureGPIOTask(task);
					//enable PPI peripheral
					nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)nrf_lpcomp_event_address_get(active_event), nrf_gpiote_task_addr_get(gpio_taskNo[task_index]));
					nrf_ppi_channel_enable(channels[first_free_channel]);
				break;
				case 0x30: //task is related to NFC
					if((task & 0x0F)==0) //start sensing
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)nrf_lpcomp_event_address_get(active_event), 0x40005008);
					else //stop sensing
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)nrf_lpcomp_event_address_get(active_event), 0x40005004);
					nrf_ppi_channel_enable(channels[first_free_channel]);
				break;
				default: //task not recognized
					return 0;
				break;
			}
			//start comparator
			nrf_lpcomp_task_trigger(NRF_LPCOMP_TASK_START);
		break;
		case 0x30:
			//event is related to NFC
			switch(task & 0xF0){
				case 0x00: //task is related to timer
					if((task & 0x0F)==0){	//start task
						configureTimer();
						//enable PPI peripheral 
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)0x40005104, (uint32_t)nrf_timer_task_address_get(NRF_TIMER1, NRF_TIMER_TASK_START));
						nrf_ppi_channel_enable(channels[first_free_channel]);
					}
					else{	//stop task
						//enable PPI peripheral 
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)0x40005104, (uint32_t)nrf_timer_task_address_get(NRF_TIMER1, NRF_TIMER_TASK_STOP));
						nrf_ppi_channel_enable(channels[first_free_channel]);
					}
				break;
				case 0x10: //task is related to GPIO
					configureGPIOTask(task);
					//enable PPI peripheral 
					nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)0x40005104, nrf_gpiote_task_addr_get(gpio_taskNo[task_index]));
					nrf_ppi_channel_enable(channels[first_free_channel]);
				break;
				default: //task not recognized
					return 0;
				break;
			}
		break;
		case 0x40:
			//event is related to Power
			sd_power_pof_enable(true);
			switch(task & 0xF0){
				case 0x00: //task is related to timer
					if((task & 0x0F)==0){	//start task
						configureTimer();
						//enable PPI peripheral 
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)0x40000108,(uint32_t)nrf_timer_task_address_get(NRF_TIMER1, NRF_TIMER_TASK_START));
						nrf_ppi_channel_enable(channels[first_free_channel]);
					}
					else{	//stop task
						//enable PPI peripheral 
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)0x40000108, (uint32_t)nrf_timer_task_address_get(NRF_TIMER1, NRF_TIMER_TASK_STOP));
						nrf_ppi_channel_enable(channels[first_free_channel]);
					}
				break;
				case 0x10: //task is related to GPIO
					configureGPIOTask(task);
					//enable PPI peripheral 
					nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)0x40000108, nrf_gpiote_task_addr_get(gpio_taskNo[task_index]));
					nrf_ppi_channel_enable(channels[first_free_channel]);
				break;
				case 0x30: //task is related to NFC
					if((task & 0x0F)==0) //start sensing
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)0x40000108, 0x40005008);
					else //stop sensing
						nrf_ppi_channel_endpoint_setup(channels[first_free_channel], (uint32_t)0x40000108, 0x40005004);
					nrf_ppi_channel_enable(channels[first_free_channel]);
				break;
				default: //task not recognized
					return 0;
				break;
			}
				
		break;
		default:
			return 0;	//event not recognized
		break;
	}
	first_free_channel++;
	return 1;
}


void PPIClass::setInputPin(uint32_t pin){
	inputPin=pin;
}

void PPIClass::setOutputPin(uint32_t pin){
	outputPin=pin;
}

void PPIClass::setTimerInterval(uint32_t msec){
	milliSec=msec;
}

void PPIClass::setCompReference(nrf_lpcomp_ref_t ref){
	reference=ref;
}

//private function

//functions to configure events
void PPIClass::configureTimer(){
	nrf_timer_mode_set(NRF_TIMER1, NRF_TIMER_MODE_TIMER);
	nrf_timer_bit_width_set(NRF_TIMER1, NRF_TIMER_BIT_WIDTH_32);
	nrf_timer_frequency_set(NRF_TIMER1, NRF_TIMER_FREQ_1MHz);
	//Clear the timer when it finishes to count
	nrf_timer_shorts_enable(NRF_TIMER1, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK);
	
	uint32_t ticks=nrf_timer_ms_to_ticks(milliSec, NRF_TIMER_FREQ_1MHz);
	nrf_timer_cc_write(NRF_TIMER1, NRF_TIMER_CC_CHANNEL0, ticks);
	
}

void PPIClass::configureGPIOEvent(event_type event){
	nrf_gpiote_polarity_t mode;
	switch(event&0x0F){
		case 0:
			mode=NRF_GPIOTE_POLARITY_LOTOHI;
		break;
		case 1:
			mode=NRF_GPIOTE_POLARITY_HITOLO;
		break;
		case 2:
			mode=NRF_GPIOTE_POLARITY_TOGGLE;
		break;
	}
	//if user use more than the allowed number of gpio, overwrite previous configuration
	if(gpiote_config_index==8)
		gpiote_config_index=0;
	nrf_gpiote_event_configure(gpiote_config_index, g_APinDescription[inputPin].ulPin, mode);

	event_index=gpiote_config_index;
	gpiote_config_index++;
}

void PPIClass::configureCompEvent(detect_mode mode){
	nrf_lpcomp_config_t config={reference, (nrf_lpcomp_detect_t)mode};
	nrf_lpcomp_configure(&config);
	if(inputPin<14 && inputPin>19)
		return;	//no analog pin is choosen
	nrf_lpcomp_input_t analog_pin[]={NRF_LPCOMP_INPUT_1, NRF_LPCOMP_INPUT_2, NRF_LPCOMP_INPUT_4, NRF_LPCOMP_INPUT_5, NRF_LPCOMP_INPUT_6, NRF_LPCOMP_INPUT_7};
	nrf_lpcomp_input_select(analog_pin[inputPin-14]);
	nrf_lpcomp_enable();

}


//function to configure tasks
void PPIClass::configureGPIOTask(task_type task){
	//if user use more than the allowed number of gpio, overwrite previous configuration
	if(gpiote_config_index==8)
		gpiote_config_index=0;
	nrf_gpiote_polarity_t job;
	nrf_gpiote_outinit_t initValue;
	switch(task&0x0F){
		case 0:
			job=NRF_GPIOTE_POLARITY_LOTOHI;
			initValue=NRF_GPIOTE_INITIAL_VALUE_LOW;
			break;
		case 1:
			job=NRF_GPIOTE_POLARITY_HITOLO;
			initValue=NRF_GPIOTE_INITIAL_VALUE_HIGH;
			break;
		case 2:
			job=NRF_GPIOTE_POLARITY_TOGGLE;
			initValue=NRF_GPIOTE_INITIAL_VALUE_LOW;
			break;
	}
	
	nrf_gpiote_task_configure(gpiote_config_index, g_APinDescription[outputPin].ulPin, job, initValue);
	nrf_gpiote_task_enable(gpiote_config_index);
	
	task_index=gpiote_config_index;
	gpiote_config_index++;
}

PPIClass PPI;
