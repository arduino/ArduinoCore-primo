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


#ifndef PPI_h
#define PPI_h

#include "Arduino.h"
#include "nrf_ppi.h"
#include "nrf_timer.h"
#include "nrf_gpiote.h"
#include "nrf_lpcomp.h"


//enumerate events and tasks
//first nibble: peripheral involved (0 -> timer, 1 -> gpio, etc)
//second nibble: event or task index
typedef enum{
	TIMER = 0X00,
	PIN_HIGH = 0X10,
	PIN_LOW = 0X11,
	PIN_CHANGE = 0X12,
	COMP_DOWN = 0X20,
	COMP_UP = 0X21,
	COMP_CROSS = 0X22,
	NFC_FIELD_DETECTED = 0X30,
	POWER_WARNING = 0X40
}event_type;

typedef enum{
	TIMER_START = 0X00,
	TIMER_STOP = 0X01,
	PIN_SET = 0X10,
	PIN_CLEAR = 0X11,
	PIN_TOGGLE = 0X12,
	NFC_START_SENSE = 0X30,
	NFC_STOP_SENSE = 0X31
}task_type;

class PPIClass{

	public:
		/**
		* @brief
		* Name:
		*			setShortcut
		* Description:
		*			The function allows to make an action when a given event occurs without
		*			involving MCU.
		* Arguments:
		*			-event: event to which bind the related action.
		*			-task: action to be taken when the event occurs.
		*/
		int setShortcut(event_type event, task_type task);

		
		/**
		* @brief
		* Name:
		*			setInputPin
		* Description:
		*			Select the pin that will be taken into account in the next
		*			event -> action interaction.
		* Argument:
		*			pin: pin's number.
		*/
		void setInputPin(uint32_t pin);
		
		
		/**
		* @brief
		* Name:
		*			setOutputPin
		* Description:
		*			Select the target pin of the action.
		* Argument:
		*			pin: pin's number.
		*/
		void setOutputPin(uint32_t pin);
		
		
		/**
		* @brief
		* Name:
		*			setTimerInterval
		* Description:
		*			Select an interval time taken into account when the event
		*			is related to the timer.
		* Argument:
		*			msec: time interval (in milliseconds).
		*/
		void setTimerInterval(uint32_t msec);
		
		
		/**
		* @brief
		* Name:
		*			setCompReference
		* Description:
		*			Select an analog reference used when the event is related
		*			to the comparator.
		* Argument:
		*			pin: pin's number.
		*/
		void setCompReference(nrf_lpcomp_ref_t ref);
		
		
	private:
		
		void configureTimer(void);
		void configureGPIOTask(task_type task);
		void configureGPIOEvent(event_type event);
		void configureCompEvent(detect_mode mode);
};

extern PPIClass PPI;


#endif //PPI_h