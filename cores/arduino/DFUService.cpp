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

#ifdef __cplusplus
extern "C"{
#endif

#include "ble_dfu.h"
#include "dfu_app_handler.h"
#include "ble_advertising.h"
#include "pstorage.h"
#include "nrf_delay.h"

#ifdef __cplusplus
}
#endif

static ble_dfu_t                         m_dfus;
static uint16_t                          m_conn_handle = BLE_CONN_HANDLE_INVALID;   /**< Handle of the current connection. */
static dm_application_instance_t         m_app_handle;

bool dfuService = true;

extern void processBleEvents(ble_evt_t * p_ble_evt) __attribute__((weak));
extern bool isPeripheralRunning()                   __attribute__((weak));
extern bool isCentralRunning()                      __attribute__((weak));

void removeDfuService(bool remove){
	if(remove == true)
		dfuService = false;
}

static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
            {
        case BLE_GAP_EVT_CONNECTED:
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
//-----start advertising again here ? ------------			
		ble_gap_adv_params_t advertisingParameters;
		memset(&advertisingParameters, 0x00, sizeof(advertisingParameters));

		advertisingParameters.type        = BLE_GAP_ADV_TYPE_ADV_IND;
		advertisingParameters.p_peer_addr = NULL;
		advertisingParameters.fp          = BLE_GAP_ADV_FP_ANY;
		advertisingParameters.p_whitelist = NULL;
		advertisingParameters.interval    = (100 * 16) / 10; // advertising interval (in units of 0.625 ms)
		advertisingParameters.timeout     = 0;

		sd_ble_gap_adv_start(&advertisingParameters);
//------let it manage to BLE library ? ------------
            break;

        default:
            // No implementation needed.
            break;
    }
}
#include "Arduino.h"
static void reset_prepare(void)
{
    if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        // Disconnect from peer.
        sd_ble_gap_disconnect(m_conn_handle, 0x13/*HCI_REMOTE_USER_TERMINATED_CONNECTION*/);
	}
    else
    {
        // If not connected, the device will be advertising. Hence stop the advertising.
        sd_ble_gap_adv_stop();
    }
	
	// Add a delay to make sure that disconnect request is sent
	nrf_delay_ms(500);
}

void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
	if(dfuService){
		dm_ble_evt_handler(p_ble_evt);
		ble_dfu_on_ble_evt(&m_dfus, p_ble_evt);
		on_ble_evt(p_ble_evt);
	}
	// forward events to Arduino BLE library if used
	if(processBleEvents)
		processBleEvents(p_ble_evt);
}

static void app_context_load(dm_handle_t const * p_handle)
{
    uint32_t                 err_code;
    static uint32_t          context_data;
    dm_application_context_t context;

    context.len    = sizeof(context_data);
    context.p_data = (uint8_t *)&context_data;

    err_code = dm_application_context_get(p_handle, &context);
    if (err_code == NRF_SUCCESS)
    {
        // Send Service Changed Indication if ATT table has changed.
        if ((context_data & (DFU_APP_ATT_TABLE_CHANGED << DFU_APP_ATT_TABLE_POS)) != 0)
        {
            err_code = sd_ble_gatts_service_changed(m_conn_handle, 0x00C/*APP_SERVICE_HANDLE_START*/, 0xFFFF/*BLE_HANDLE_MAX*/);
            if ((err_code != NRF_SUCCESS) &&
                (err_code != BLE_ERROR_INVALID_CONN_HANDLE) &&
                (err_code != NRF_ERROR_INVALID_STATE) &&
                (err_code != BLE_ERROR_NO_TX_PACKETS) &&
                (err_code != NRF_ERROR_BUSY) &&
                (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING))
            {
				//error
            }
        }

        err_code = dm_application_context_delete(p_handle);
    }
    else if (err_code == DM_NO_APP_CONTEXT)
    {
        // No context available. Ignore.
    }
    else
    {
		//error
    }
}


static uint32_t device_manager_evt_handler(dm_handle_t const * p_handle,
                                           dm_event_t const  * p_event,
                                           ret_code_t        event_result)
{
    if (p_event->event_id == DM_EVT_LINK_SECURED)
    {
        app_context_load(p_handle);
    }

    return NRF_SUCCESS;
}

void add_dfu_service(){
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;
		
//*** device manager init - start
  
    dm_init_param_t        init_param = {.clear_persistent_data = false};
    dm_application_param_t register_param;

    // Initialize persistent storage module.
    pstorage_init();
	dm_init(&init_param);
	
    memset(&register_param.sec_param, 0, sizeof(ble_gap_sec_params_t));

    register_param.sec_param.bond         = 1; //SEC_PARAM_BOND
    register_param.sec_param.mitm         = 0; //SEC_PARAM_MITM
	// register_param.sec_param.mitm         = true;
    register_param.sec_param.lesc         = 0; //SEC_PARAM_LESC
// register_param.sec_param.lesc         = true;
    register_param.sec_param.keypress     = 0; //SEC_PARAM_KEYPRESS
    register_param.sec_param.io_caps      = BLE_GAP_IO_CAPS_NONE;//BLE_GAP_IO_CAPS_DISPLAY_ONLY;//BLE_GAP_IO_CAPS_KEYBOARD_ONLY;
	// register_param.sec_param.io_caps      =BLE_GAP_IO_CAPS_DISPLAY_YESNO;
    register_param.sec_param.oob          = 0; //SEC_PARAM_OOB
    register_param.sec_param.min_key_size = 7; //SEC_PARAM_MIN_KEY_SIZE
    register_param.sec_param.max_key_size = 16; //SEC_PARAM_MAX_KEY_SIZE
    register_param.evt_handler            = device_manager_evt_handler;
    register_param.service_type           = DM_PROTOCOL_CNTXT_GATT_SRVR_ID;

    dm_register(&m_app_handle, &register_param);
 
//*** device manager init - end
	// If isPeripheralRunning is defined user is using Arduino BLE library
	if(!isPeripheralRunning || !isPeripheralRunning()){
		// Advertising and connection parameters have to be set only if
		// user is not using Arduino BLE library or peripheral role is not running
		// to avoid overriding user's settings
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

		const char * DEVICE_NAME = "ArduinoPRIMO_DFU";
		sd_ble_gap_device_name_set(&sec_mode,
											 (const uint8_t *)DEVICE_NAME,
											 strlen(DEVICE_NAME));

		memset(&gap_conn_params, 0, sizeof(gap_conn_params));

		gap_conn_params.min_conn_interval = 320; //MIN_CONN_INTERVAL
		gap_conn_params.max_conn_interval = 520; //MAX_CONN_INTERVAL
		gap_conn_params.slave_latency     = 0;   //SLAVE_LATENCY
		gap_conn_params.conn_sup_timeout  = 400; //CONN_SUP_TIMEOUT

		sd_ble_gap_ppcp_set(&gap_conn_params);

		// advertising
		unsigned char adv_data[31] ={0};
		uint8_t len=strlen(DEVICE_NAME);
		adv_data[0]=len+1; //len
		adv_data[1]=0x09;  //type - complete local name
		memcpy((void*)&adv_data[2], DEVICE_NAME, len);
		len=len+2;
		sd_ble_gap_adv_data_set(adv_data, len, NULL, 0);		

		  // ble_advdata_t advdata;
		// static ble_uuid_t m_adv_uuids[] = {
									   // {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}}; /**< Universally unique service identifiers. */

		// Build advertising data struct to pass into @ref ble_advertising_init.
		// memset(&advdata, 0, sizeof(advdata));

		// advdata.name_type               = BLE_ADVDATA_FULL_NAME;
		// advdata.include_appearance      = true;
		// advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
		// advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
		// advdata.uuids_complete.p_uuids  = m_adv_uuids;

		// ble_adv_modes_config_t options = {0};
		// options.ble_adv_fast_enabled  = true; //BLE_ADV_FAST_ENABLED;
		// options.ble_adv_fast_interval = 40; //APP_ADV_INTERVAL;
		// options.ble_adv_fast_timeout  = BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED;
		// if(isCentralRunning && isCentralRunning())	//don't pass options in ble_advertising_init if central is running
			// ble_advertising_init(&advdata, NULL, NULL, NULL, NULL);
		// else
			// ble_advertising_init(&advdata, NULL, &options, NULL, NULL);
	}
	
	/** @snippet [DFU BLE Service initialization] */
    ble_dfu_init_t   dfus_init;

    // Initialize the Device Firmware Update Service.
    memset(&dfus_init, 0, sizeof(dfus_init));

    dfus_init.evt_handler   = dfu_app_on_dfu_evt;
    dfus_init.error_handler = NULL;
    dfus_init.evt_handler   = dfu_app_on_dfu_evt;
    dfus_init.revision      = 1;//DFU_REVISION;

    ble_dfu_init(&m_dfus, &dfus_init);
	    
    dfu_app_reset_prepare_set(reset_prepare);
	dfu_app_dm_appl_instance_set(m_app_handle);
		 
	if(!isPeripheralRunning || !isPeripheralRunning()){
		ble_gap_adv_params_t advertisingParameters;
		memset(&advertisingParameters, 0x00, sizeof(advertisingParameters));

		advertisingParameters.type        = BLE_GAP_ADV_TYPE_ADV_IND;
		advertisingParameters.p_peer_addr = NULL;
		advertisingParameters.fp          = BLE_GAP_ADV_FP_ANY;
		advertisingParameters.p_whitelist = NULL;
		advertisingParameters.interval    = (100 * 16) / 10; // advertising interval (in units of 0.625 ms)
		advertisingParameters.timeout     = 0;

		sd_ble_gap_adv_start(&advertisingParameters);
	}
}

bool dfuIsEnabled(){
	return dfuService;
}