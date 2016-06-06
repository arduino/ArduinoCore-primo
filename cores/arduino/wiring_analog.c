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

#include "wiring_analog.h"
#include "wiring_digital.h"
#include "nrf_saadc.h"
#include "nrf_pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

static int readResolution = 10;
static int writeResolution = 10;
static int reference=SAADC_CH_CONFIG_REFSEL_VDD1_4;
static int gain=SAADC_CH_CONFIG_GAIN_Gain1_4;


void analogReadResolution(int res) {
	readResolution = res;
	
	if(res==8) nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_8BIT); 
	else if(res==10) nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_10BIT);   
	else if(res==12) nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_12BIT);
	else if(res==14) nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_14BIT);
}

void analogWriteResolution(int res) {
	writeResolution = res;
}

static inline uint32_t mapResolution(uint32_t value, uint32_t from, uint32_t to) {
	if (from == to)
		return value;
	if (from > to)
		return value >> (from-to);
	else
		return value << (to-from);
}

void analogReference( eAnalogReference ulMode )
{
  // for nrf52832 only two reference are provided   
  switch(ulMode)
  {
    case AR_DEFAULT:
      default:
	  reference=SAADC_CH_CONFIG_REFSEL_VDD1_4;
	  gain=SAADC_CH_CONFIG_GAIN_Gain1_4;
	  break;
    
	case AR_INTERNAL:
	  reference=SAADC_CH_CONFIG_REFSEL_Internal;
	  gain=SAADC_CH_CONFIG_GAIN_Gain1_5;
	  break;

    case AR_EXTERNAL:
	  reference=SAADC_CH_CONFIG_REFSEL_VDD1_4;
	  gain=SAADC_CH_CONFIG_GAIN_Gain1_4;
	  break;
  }
  
}


uint32_t analogRead( uint32_t ulPin )
{
	static int16_t valueRead[1];  
	
	//enable ADC
    NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos;       
	 
	//configure ADC 
    NRF_SAADC->CH[0].CONFIG = SAADC_CH_CONFIG_BURST_Enabled << SAADC_CH_CONFIG_BURST_Pos | 
                              gain << SAADC_CH_CONFIG_GAIN_Pos |
                              SAADC_CH_CONFIG_MODE_SE << SAADC_CH_CONFIG_MODE_Pos | 
                              reference << SAADC_CH_CONFIG_REFSEL_Pos |
                              SAADC_CH_CONFIG_RESN_Pulldown << SAADC_CH_CONFIG_RESN_Pos |
                              SAADC_CH_CONFIG_RESP_Bypass << SAADC_CH_CONFIG_RESP_Pos |
                              SAADC_CH_CONFIG_TACQ_10us << SAADC_CH_CONFIG_TACQ_Pos;
    NRF_SAADC->CH[0].PSELN =  SAADC_CH_PSELN_PSELN_NC;
    NRF_SAADC->CH[0].PSELP =  g_APinDescription[ulPin].ulADCChannelNumber;
    
	// calibration
    // NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
    // NRF_SAADC->TASKS_CALIBRATEOFFSET = 1;
    // while(NRF_SAADC->EVENTS_CALIBRATEDONE == 0);   
	
	//set oversample
    NRF_SAADC->OVERSAMPLE = 4;
      
	//set result buffer
    NRF_SAADC->RESULT.MAXCNT = 1;
    NRF_SAADC->RESULT.PTR = (uint32_t)valueRead;
	
	//start ADC conversion
	NRF_SAADC->EVENTS_STARTED = 0;
    NRF_SAADC->TASKS_START = 1;
    while(NRF_SAADC->EVENTS_STARTED == 0);
 
    NRF_SAADC->EVENTS_DONE = 0;
    NRF_SAADC->TASKS_SAMPLE = 1;
    while(NRF_SAADC->EVENTS_DONE == 0);
    	
	//disable ADC
	NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Disabled << SAADC_ENABLE_ENABLE_Pos);

	if(valueRead[0]<0)
		return 0;
	
	return valueRead[0];	
}	




//in NRF52 pwm works on all pins
void analogWrite(uint32_t ulPin, uint32_t ulValue) {

	//assuming user uses values from 0 to 255
	uint16_t val=(uint16_t) (ulValue*10000/255);
	
	// This array cannot be allocated on stack (hence "static") and it must
    // be in RAM (hence no "const", though its content is not changed).
	static uint16_t /*const*/ seq_values[]={0};
	seq_values[0]=val;
	nrf_pwm_sequence_t const seq={
		.values.p_common = seq_values,
        .length          = NRF_PWM_VALUES_LENGTH(seq_values),
        .repeats         = 0,
        .end_delay       = 0
    };
	
	//assign pin to pwm channel
	uint32_t pin[NRF_PWM_CHANNEL_COUNT]={g_APinDescription[ulPin].ulPin | 0x80 , NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED};
	nrf_pwm_pins_set(NRF_PWM0, pin);
	
	//enable pwm channel
	nrf_pwm_enable(NRF_PWM0);
	
	//configure pwm channel	- Prescaler, mode and counter top
	nrf_pwm_configure(NRF_PWM0, NRF_PWM_CLK_500kHz, NRF_PWM_MODE_UP, 10000);
	
	//set decoder
	nrf_pwm_decoder_set(NRF_PWM0, NRF_PWM_LOAD_COMMON, NRF_PWM_STEP_AUTO);
		
	//set the sequence for the given channel
	nrf_pwm_sequence_set(NRF_PWM0, 0, &seq);
	
	//perform sequence playback just one time
	nrf_pwm_loop_set(NRF_PWM0, 0UL);
	
	//start pwm generation
	nrf_pwm_task_trigger(NRF_PWM0, NRF_PWM_TASK_SEQSTART0);
 
}



#ifdef __cplusplus
}
#endif
