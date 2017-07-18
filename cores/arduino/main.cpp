/*
  main.cpp - Main loop for Arduino sketches
  Copyright (c) 2005-2016 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#define ARDUINO_MAIN
#include "Arduino.h"


/*
 * \brief Main entry point of Arduino application
 */
int main(void)
{
	init();

	delay(0);

	SDManager.begin();

    // Register with the SoftDevice handler module for BLE events.
    softdevice_ble_evt_handler_set(ble_evt_dispatch);

	setup();

	if(dfuIsEnabled())
		add_dfu_service();	

	for(;;){ 
		loop();
		if (serialEventRun) serialEventRun();
	}
	return 0;
}