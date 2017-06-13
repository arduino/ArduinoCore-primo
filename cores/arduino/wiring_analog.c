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
#include "wiring_constants.h"
#include "nrf_saadc.h"
#include "nrf_pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

static int readResolution = 10;
static int writeResolution = 8;
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

void analogReference( uint8_t ulMode )
{
  // for nrf52832 only two reference are provided   
  switch(ulMode)
  {
    case DEFAULT:
      default:
	  reference=SAADC_CH_CONFIG_REFSEL_VDD1_4;
	  gain=SAADC_CH_CONFIG_GAIN_Gain1_4;
	  break;
    
	case INTERNAL:
	  reference=SAADC_CH_CONFIG_REFSEL_Internal;
	  gain=SAADC_CH_CONFIG_GAIN_Gain1_5;
	  break;

	case INTERNAL3V6:
	  reference=SAADC_CH_CONFIG_REFSEL_Internal;
	  gain=SAADC_CH_CONFIG_GAIN_Gain1_6;
	  break;
	  
    case EXTERNAL:
	  //nrf52 does not provide external reference.
	  //Here default one is used.
	  reference=SAADC_CH_CONFIG_REFSEL_VDD1_4;
	  gain=SAADC_CH_CONFIG_GAIN_Gain1_4;
	  break;
  }

}
uint32_t analogRead( uint32_t ulPin ){
	static int16_t valueRead[1];  

#ifdef ARDUINO_NRF52_PRIMO
	if((ulPin >= 0) && (ulPin <= 5))
		ulPin = ulPin + 14;
	// no analogRead for digital pin
	else if(ulPin<=13)
		return 0;
#elif defined ARDUINO_NRF52_PRIMO_CORE
	if(ulPin > 7)
		return 0;
#endif
	
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
 
    NRF_SAADC->EVENTS_END = 0;
    NRF_SAADC->TASKS_SAMPLE = 1;
    while(NRF_SAADC->EVENTS_END == 0);
    	
	//disable ADC
	NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Disabled << SAADC_ENABLE_ENABLE_Pos);

	if(valueRead[0]<0)
		return 0;
	
	return valueRead[0];	
}	


void analogWrite(uint32_t ulPin, uint32_t ulValue) {
	
	uint16_t val;
	if(writeResolution==8){
#if defined ARDUINO_NRF52_PRIMO_CORE
		// if a LED is chosen reverse the value because of the LEDs' reverse logic
		if(ulPin == 10 || ulPin == 11 || ulPin == 12 || ulPin == 13)
			ulValue = 255 - ulValue;
#endif //ARDUINO_NRF52_PRIMO_CORE
		val=(uint16_t) (ulValue*10000/255);
	}
	
	else if(writeResolution==10){
#if defined ARDUINO_NRF52_PRIMO_CORE
		// if a LED is chosen reverse the value because of the LEDs' reverse logic
		if(ulPin == 10 || ulPin == 11 || ulPin == 12 || ulPin == 13)
			ulValue = 1023 - ulValue;
#endif //ARDUINO_NRF52_PRIMO_CORE
		val=(uint16_t) (ulValue*10000/1024);
	}
	
	else{ //assuming 12 bit resolution
#if defined ARDUINO_NRF52_PRIMO_CORE
		// if a LED is chosen reverse the value because of the LEDs' reverse logic
		if(ulPin == 10 || ulPin == 11 || ulPin == 12 || ulPin == 13)
			ulValue = 4095 - ulValue;
#endif //ARDUINO_NRF52_PRIMO_CORE
		val=(uint16_t) (ulValue*10000/4096);
	}
	// This array cannot be allocated on stack (hence "static") and it must
    // be in RAM (hence no "const", though its content is not changed).
	static uint16_t /*const*/ seq_values[]={0};
	//In each value, the most significant bit (15) determines the polarity of the output
	//0x8000 is MSB = 1
	seq_values[0]=val | 0x8000;
	nrf_pwm_sequence_t const seq={
		.values.p_common = seq_values,
        .length          = NRF_PWM_VALUES_LENGTH(seq_values),
        .repeats         = 0,
        .end_delay       = 0
    };
	
	//assign pin to pwm channel - look at WVariant.h for details about ulPWMChannel attribute
	int8_t pwm_type=g_APinDescription[ulPin].ulPWMChannel;
	if(pwm_type == NOT_ON_PWM)
		return;
	
	uint32_t pin[NRF_PWM_CHANNEL_COUNT]={NRF_PWM_PIN_NOT_CONNECTED/*g_APinDescription[ulPin].ulPin*/, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED};
	pin[pwm_type & 0x0F]=g_APinDescription[ulPin].ulPin;
	NRF_PWM_Type * PWMInstance = NRF_PWM0;
	switch(pwm_type &0xF0){
		case 16://0x10
			PWMInstance = NRF_PWM1;
			break;
		case 32://0x20
			PWMInstance = NRF_PWM2;
			break;
	}
	nrf_pwm_pins_set(PWMInstance, pin);
	
	//enable pwm channel
	nrf_pwm_enable(PWMInstance);
	
	//configure pwm channel	- Prescaler, mode and counter top
	nrf_pwm_configure(PWMInstance, NRF_PWM_CLK_16MHz, NRF_PWM_MODE_UP, 10000);
	
	//set decoder
	nrf_pwm_decoder_set(PWMInstance, NRF_PWM_LOAD_COMMON, NRF_PWM_STEP_AUTO);
		
	//set the sequence for the given channel
	nrf_pwm_sequence_set(PWMInstance, 0, &seq);
	
	//perform sequence playback just one time
	nrf_pwm_loop_set(PWMInstance, 0UL);
	
	//start pwm generation
	nrf_pwm_task_trigger(PWMInstance, NRF_PWM_TASK_SEQSTART0);
 
}



#ifdef __cplusplus
}
#endif

