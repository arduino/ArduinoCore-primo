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

#include "SoftDeviceManager.h"
#include "BLEManager.h"

SoftDeviceManager::SoftDeviceManager(){
    // Clear the callback lists
    memset((void *)_systemEventCallbackList, 0, sizeof(_systemEventCallbackList[0]) * SYSTEM_EVENT_CALLBACK_NUM); 
}
	
void SoftDeviceManager::begin(){
	
	uint32_t err_code;
	
	// Low frequency clock source to be used by the SoftDevice
	nrf_clock_lf_cfg_t clock_lf_cfg = {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM};
	// Initialize the SoftDevice handler module.	
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);
	
	ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(1/*number of central*/,
                                                    1/*number of peripheral*/,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);
	
	ble_enable_params.gatts_enable_params.service_changed = 1;
	
	 //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(1/*number of central*/,1/*number of peripheral*/);

    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    if(err_code != 0) registerError("SoftDeviceManager::begin()", err_code, "softdevice_enable() returned an error");
    
    // Register the BLE callback handler
    softdevice_ble_evt_handler_set(BLEManager.processBleEvents);
}

uint8_t SoftDeviceManager::isEnabled(){
	uint8_t enable;
	uint32_t err_code;
	
	err_code = sd_softdevice_is_enabled(&enable);
    if(err_code != 0) registerError("SoftDeviceManager::isEnabled()", err_code, "sd_softdevice_is_enabled() returned an error");

	return enable;
}

void SoftDeviceManager::setSystemEventHandler(void (*systemEventHandler)(uint32_t)){
	
	uint32_t err_code;
	
	// Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(systemEventHandler);
    APP_ERROR_CHECK(err_code);
}

void SoftDeviceManager::flashErasePage(uint32_t page){}
void SoftDeviceManager::flashReadArray(uint32_t flashAddress, uint8_t *buf, uint32_t bufLength){}
void SoftDeviceManager::flashWriteArray(uint32_t flashAddress, uint8_t *buf, uint32_t bufLength){}

const char * SoftDeviceManager::getErrorDescription(uint32_t errorCode){
	/*uint8_t i=0;
	for(;i<err_index;i++)
		if(errorCode==errorsList[i].errCode)
			return errorsList[i].msg;*/
	return NULL;
}
void SoftDeviceManager::registerError(char *file, uint32_t errCode, char *msg){
    errorMessage newError = {errCode, file, msg};
	errorMessageFifo.put(&newError);
    pollErrors();
}

void SoftDeviceManager::pollErrors(void){
    if(errorCallback != 0){
        errorMessage error;
        while(errorMessageFifo.get(&error)){
            errorCallback(error.file, error.errCode, error.msg);
        }
    }    
}

void SoftDeviceManager::setErrorHandler(void (*errorHandlerCallback)(char *file, uint32_t errCode, char *msg)){
	errorCallback=errorHandlerCallback;
}

SoftDeviceManager SDManager;