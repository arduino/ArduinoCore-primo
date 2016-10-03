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


#ifndef SoftDeviceManager_h
#define SoftDeviceManager_h

#include <stdint.h>
#include "RingBufferT.cpp"

#ifdef __cplusplus
extern "C"{
#endif

#include "nrf_sdm.h"
#include "softdevice_handler.h"
#include "ble.h"

#ifdef __cplusplus
}
#endif

#define ERROR_MESSAGE_NUM 20

#define SYSTEM_EVENT_CALLBACK_NUM   10

typedef struct{ 
	uint32_t errCode;
	char * file;
	char * msg;
}errorMessage;

class SoftDeviceManager{

	public:
		SoftDeviceManager();
	
		void begin(void);
		
		uint8_t isEnabled(void);
	
		// Handler for system events (flash, timeslot ++)
		void setSystemEventHandler(void(*systemEventHandler)(uint32_t));
        
        // Handler for BLE events

		// Protected peripheral access
		// TBD: Power management functions
		// use lowPower library for the functions below
		//void enterSleep(void);		
		//SystemWakeupSource &getWakeupSource(void);

		// TBD: Flash access
		void flashErasePage(uint32_t page);
		void flashReadArray(uint32_t flashAddress, uint32_t *buf, unsigned int bufLength);
		void flashWriteArray(const uint32_t *flashAddress, const uint32_t *buf, uint32_t bufLength);

		// TBD: app_timer
		// TBD: Interrupt API
		// TBD: Clock access
		// TBD: PPI functions
		// TBD: RNG functions
		// TBD: AES functions
		// TBD: TEMP functions
		// TBD: Timeslot Access

		// Error handling
		const char *getErrorDescription(uint32_t errorCode);
		void registerError(char *file, uint32_t errCode, char *msg);
		void setErrorHandler(void (*errorHandlerCallback)(char *file, uint32_t errCode, char *msg));
        void pollErrors(void);

	private:	
        // System events
        sys_evt_handler_t _systemEventCallbackList[SYSTEM_EVENT_CALLBACK_NUM];
		// Callback user function
		void (*errorCallback)(char *file, uint32_t errCode, char *msg) = 0;	
        RingBufferT<errorMessage, ERROR_MESSAGE_NUM> errorMessageFifo;
		
};

extern SoftDeviceManager SDManager;

#endif