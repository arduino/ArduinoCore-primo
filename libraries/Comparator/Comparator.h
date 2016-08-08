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


#ifndef Comparator_h
#define Comparator_h


#include "Arduino.h"
#include "nrf_lpcomp.h"


class Comparator{

	public:
		/**
		* @brief
		* 	Class constructor
		* Arguments:
		*			-input_pin: analog input pin
		*			-reference: internal or external voltage to compare
		*			-mode: type of event to report [UP - DOWN - CHANGE]
		*/
		Comparator(uint8_t input_pin, nrf_lpcomp_ref_t reference, detect_mode mode);
		
		/**
		* @brief
		* 	Class destructor
		*/
		~Comparator(void);
		
		/**
		* @brief
		* Name:
		*			begin
		* Description:
		*			function to inizialize and start the module
		*/
		void begin(void);
		
		
		/**
		* @brief
		* Name:
		*			compare
		* Description:
		*			enable interrupt on the compare event previously selected.
		* Argument:
		*			-function: callback function called when the target compare event occurs.
		*/
		void compare(void(*function)(void));
		
		
		/**
		* @brief
		* Name:
		*			status
		* Description:
		*			Indicates whether the voltage is higher or lower than the reference
		* Returned vaule:
		*			1 if higher - 0 if lower.
		*/
		uint32_t status(void);
		
		/**
		* @brief
		* Name:
		*			stop
		* Description:
		*			stop the module and disable it.
		*/
		void stop(void);

		uint8_t _input_pin;
		nrf_lpcomp_ref_t _reference;
		detect_mode _mode;
		
		// Callback user function
		void (*Callback)(void);
};


#endif //Comparator_h