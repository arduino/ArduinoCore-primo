/*
  Comparator class for nRF52.
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

#include "Comparator.h"

nrf_lpcomp_input_t analog_pin[]={NRF_LPCOMP_INPUT_1, NRF_LPCOMP_INPUT_2, NRF_LPCOMP_INPUT_4, NRF_LPCOMP_INPUT_5, NRF_LPCOMP_INPUT_6, NRF_LPCOMP_INPUT_7};

Comparator * CompPointer=NULL;

Comparator::Comparator(uint8_t input_pin, nrf_lpcomp_ref_t reference, detect_mode mode){
	_input_pin=input_pin;
	_reference=reference;
	_mode=mode;
	CompPointer=this;
}

Comparator::~Comparator(){
	stop();
}

void Comparator::begin(){
	nrf_lpcomp_config_t config={_reference, (nrf_lpcomp_detect_t)_mode};
	nrf_lpcomp_configure(&config);
	if(_input_pin<14 && _input_pin>19)
		return;	//no analog pin is choosen
	nrf_lpcomp_input_select(analog_pin[_input_pin-14]);

	//enable hysteresis
	NRF_LPCOMP->HYST=1;
	
	//start comparator module
	nrf_lpcomp_enable();
	nrf_lpcomp_task_trigger(NRF_LPCOMP_TASK_START);
	while(!nrf_lpcomp_event_check(NRF_LPCOMP_EVENT_READY));
}

void Comparator::compare(void(*function)(void)){
	Callback=function;

	//enable interrupt
	NVIC_SetPriority(COMP_LPCOMP_IRQn, 6); //low priority
	NVIC_ClearPendingIRQ(COMP_LPCOMP_IRQn);
	NVIC_EnableIRQ(COMP_LPCOMP_IRQn);
	//enable interrupt just for cross event
	//cross event will be always generate for both up and down event
	nrf_lpcomp_event_clear(NRF_LPCOMP_EVENT_CROSS);
	nrf_lpcomp_int_enable(1<<3);
}

uint32_t Comparator::status(){
	nrf_lpcomp_task_trigger(NRF_LPCOMP_TASK_SAMPLE);
	while(!nrf_lpcomp_event_check(NRF_LPCOMP_EVENT_READY))
		;	//wait until sample is ready

	return nrf_lpcomp_result_get();
}

void Comparator::stop(){
	//stop the module and disable it
	nrf_lpcomp_task_trigger(NRF_LPCOMP_TASK_STOP);
	nrf_lpcomp_disable();
}

#ifdef __cplusplus
extern "C"{
#endif	

void COMP_LPCOMP_IRQHandler(void){
	nrf_lpcomp_event_clear(NRF_LPCOMP_EVENT_CROSS);
	if(CompPointer->_mode==DOWN)
		if(nrf_lpcomp_event_check(NRF_LPCOMP_EVENT_DOWN)){
			nrf_lpcomp_event_clear(NRF_LPCOMP_EVENT_DOWN);
			CompPointer->Callback();
			return;
		}
	if(CompPointer->_mode==UP){
		if(nrf_lpcomp_event_check(NRF_LPCOMP_EVENT_UP)){
			nrf_lpcomp_event_clear(NRF_LPCOMP_EVENT_UP);
			CompPointer->Callback();
			return;
		}
	}
	if(CompPointer->_mode==CROSS)
		CompPointer->Callback();
}

#ifdef __cplusplus
}
#endif
