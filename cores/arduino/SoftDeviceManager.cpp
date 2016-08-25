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

SoftDeviceManager::SoftDeviceManager(){}
	
void SoftDeviceManager::begin(){
	
	uint32_t err_code;
	
	// Low frequency clock source to be used by the SoftDevice
	nrf_clock_lf_cfg_t clock_lf_cfg= {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
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
    APP_ERROR_CHECK(err_code);

}

uint8_t SoftDeviceManager::isEnabled(){
	uint8_t enable;
	uint32_t err_code;
	
	err_code = sd_softdevice_is_enabled(&enable);
	APP_ERROR_CHECK(err_code);

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

const char * SoftDeviceManager::getErrorDescription(uint32_t errorCode){}
void SoftDeviceManager::registerError(uint8_t *file, uint32_t errCode, uint8_t *msg){}
void SoftDeviceManager::setErrorHandler(void (*errorHandlerCallback)(uint8_t *file, uint32_t errCode, uint8_t *msg)){}


SoftDeviceManager SDManager;