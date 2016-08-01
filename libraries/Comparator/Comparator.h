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

//reference voltages
#define REF_VDD_1_8 NRF_LPCOMP_REF_SUPPLY_1_8
#define REF_VDD_1_4 NRF_LPCOMP_REF_SUPPLY_2_8
#define REF_VDD_3_8 NRF_LPCOMP_REF_SUPPLY_3_8
#define REF_VDD_1_2 NRF_LPCOMP_REF_SUPPLY_4_8
#define REF_VDD_5_8 NRF_LPCOMP_REF_SUPPLY_5_8
#define REF_VDD_3_4 NRF_LPCOMP_REF_SUPPLY_6_8
#define REF_VDD_7_8 NRF_LPCOMP_REF_SUPPLY_7_8
#define REF_VDD_1_16 NRF_LPCOMP_REF_SUPPLY_1_16
#define REF_VDD_3_16 NRF_LPCOMP_REF_SUPPLY_3_16
#define REF_VDD_5_16 NRF_LPCOMP_REF_SUPPLY_5_16
#define REF_VDD_7_16 NRF_LPCOMP_REF_SUPPLY_7_16
#define REF_VDD_9_16 NRF_LPCOMP_REF_SUPPLY_9_16
#define REF_VDD_11_16 NRF_LPCOMP_REF_SUPPLY_11_16
#define REF_VDD_13_16 NRF_LPCOMP_REF_SUPPLY_13_16
#define REF_VDD_15_16 NRF_LPCOMP_REF_SUPPLY_15_16
#define AREF NRF_LPCOMP_REF_EXT_REF0

#include "Arduino.h"
#include "nrf_lpcomp.h"

typedef enum{
	UP = NRF_LPCOMP_DETECT_UP,
	DOWN = NRF_LPCOMP_DETECT_DOWN,
	CROSS = NRF_LPCOMP_DETECT_CROSS
}detect_mode;

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
		*			function to inizialize the module
		*/
		void begin(void);
		
		
		/**
		* @brief
		* Name:
		*			compare
		* Description:
		*			start the comparator function with the previous initialization.
		* Argument:
		*			-function: callback function called when the target compare event occurs.
		*/
		void compare(void(*function)(void));
		
		
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