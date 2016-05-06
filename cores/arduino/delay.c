#include "delay.h"
#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Tick Counter united by ms */
static volatile uint32_t _ulTickCount=0 ;

uint32_t millis( void )
{
// todo: ensure no interrupts
  return _ulTickCount ;
}

// TODO: IMPLEMENT THIS FUNCTION
// Interrupt-compatible version of micros
// Theory: repeatedly take readings of SysTick counter, millis counter and SysTick interrupt pending flag.
// When it appears that millis counter and pending is stable and SysTick hasn't rolled over, use these
// values to calculate micros. If there is a pending SysTick, add one to the millis counter in the calculation.
//uint32_t micros( void )
//{}

void RTC0_IRQHandler(void){
  // Increment tick count each ms
  _ulTickCount++ ;
}

void delay( uint32_t ms )
{
	nrf_delay_ms(ms);
}

void delayMicroseconds(uint32_t usec)
{
	nrf_delay_us(usec);
}
      