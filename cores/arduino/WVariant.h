#ifndef _VARIANTS_
#define _VARIANTS_

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

typedef enum _EAnalogChannel
{
	No_ADC_Channel=-1,
} EAnalogChannel ;

typedef enum _ETCChannel
{
	NOT_ON_TIMER=-1,
} ETCChannel ;

// Definitions for PWM channels
typedef enum _EPWMChannel
{
	NOT_ON_PWM=-1,
} EPWMChannel ;


typedef enum _EPortType
{
	NOT_A_PORT=-1,
	PORT0=0
} EPortType ;


typedef enum _EPioType
{
  PIO_NOT_A_PIN=-1,  /* Not under control of a peripheral. */
  PIO_ANALOG,        /* The pin is controlled by the associated signal of peripheral B. */
  PIO_DIGITAL,       /* The pin is controlled by PORT. */
  PIO_INPUT,         /* The pin is controlled by PORT and is an input. */
  PIO_INPUT_PULLUP,  /* The pin is controlled by PORT and is an input with internal pull-up resistor enabled. */
  PIO_OUTPUT,        /* The pin is controlled by PORT and is an output. */

  } EPioType ;

/**
 * Pin Attributes to be OR-ed
 */
#define PIN_ATTR_NONE          (0UL<<0)
#define PIN_ATTR_COMBO         (1UL<<0)
#define PIN_ATTR_ANALOG        (1UL<<1)
#define PIN_ATTR_DIGITAL       (1UL<<2)
#define PIN_ATTR_PWM           (1UL<<3)
#define PIN_ATTR_TIMER         (1UL<<4)
#define PIN_ATTR_EXTINT        (1UL<<5)

/* Types used for the table below */
typedef struct _PinDescription
{
  EPortType ulPort ;
  uint32_t ulPin ;
  EPioType ulPinType ;
  uint32_t ulPinAttribute ;
  EAnalogChannel ulADCChannelNumber ; /* ADC Channel number in the SAM device */
  EPWMChannel ulPWMChannel ;
  ETCChannel ulTCChannel ;
//  EExt_Interrupts ulExtInt ;
} PinDescription ;

/* Pins table to be instantiated into variant.cpp */
extern const PinDescription g_APinDescription[] ;

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // _VARIANTS_
