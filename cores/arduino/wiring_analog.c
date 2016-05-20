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

 //

#include "wiring_analog.h"
#include "wiring_digital.h"
#include "nrf_saadc.h"
#include "nrf_pwm.h"

#ifdef __cplusplus
extern "C" {
#endif




static int readResolution = 10;
static int writeResolution = 10;
static int reference=NRF_SAADC_REFERENCE_VDD4;
static int gain=NRF_SAADC_GAIN1_4;

void analogReadResolution(int res) {
	readResolution = res;
	
	if(res == 8) nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_8BIT); 
	else if(res == 10) nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_10BIT);   
	else nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_12BIT); 
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
      
	  reference=NRF_SAADC_REFERENCE_VDD4;
	  gain=NRF_SAADC_GAIN1_4;
	  break;
    
	case AR_INTERNAL:
	  reference=NRF_SAADC_REFERENCE_INTERNAL;
	  gain=NRF_SAADC_GAIN1_5;
	  break;

    case AR_EXTERNAL:
	  reference=NRF_SAADC_REFERENCE_VDD4;
	  gain=NRF_SAADC_GAIN1_4;
	  break;
  }
  
}



uint32_t analogRead( uint32_t ulPin )
{
	static nrf_saadc_value_t valueRead;

	//configure ADC channel
	nrf_saadc_channel_config_t channel_config={NRF_SAADC_RESISTOR_DISABLED,
									   NRF_SAADC_RESISTOR_DISABLED,
									   gain,
									   reference,
									   NRF_SAADC_ACQTIME_3US,
									   NRF_SAADC_MODE_SINGLE_ENDED,
									   g_APinDescription[ulPin].ulADCChannelNumber,
									   g_APinDescription[ulPin].ulADCChannelNumber //pin negative ignored in single ended mode
									   };
	//enable channel
    nrf_saadc_enable();
	
	//init channel and start conversion
	nrf_saadc_channel_init(0, &channel_config);
	nrf_saadc_buffer_init(&valueRead, 1);
	nrf_saadc_event_clear(NRF_SAADC_EVENT_END);
    nrf_saadc_task_trigger(NRF_SAADC_TASK_START);
	
	nrf_saadc_task_trigger(NRF_SAADC_TASK_SAMPLE);

	while(!nrf_saadc_event_check(NRF_SAADC_EVENT_END))
		;//wait for a complete conversion
	
	if(valueRead<0)
		return 0;
	
	return valueRead;
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
