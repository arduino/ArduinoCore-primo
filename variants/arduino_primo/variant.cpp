/*
  Copyright (c) 2016 Arduino Srl.  All right reserved.

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

/*
 *
 * + Pin number +  ZERO Board pin  |  PIN   | Label/Name      |
 * +------------+------------------+--------+-----------------+
 * |            | Digital Low      |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 0          | 0 -> RX          |  P011  |                 |
 * | 1          | 1 <- TX          |  P012  |                 |
 * | 2          | ~2               |  P013  |                 |
 * | 3          | ~3               |  P014  |                 |
 * | 4          | ~4               |  P015  |                 |
 * | 5          | ~5               |  P016  |                 |
 * | 6          | ~6               |  P017  |                 |
 * | 7          | ~7               |  P018  |                 |
 * +------------+------------------+--------+-----------------+
 * |            | Digital High     |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 8          | ~8               |  P019  |                 | 
 * | 9          | ~9               |  P020  |                 |
 * | 10         | ~10              |  P022  |                 |
 * | 11         | ~11              |  P023  |                 |
 * | 12         | ~12              |  P024  |                 |
 * | 13         | ~13              |  P025  | LED             |
 * | 14         | GND              |        |                 |
 * | 15         | AREF             |  P002  |                 |
 * | 16         | SDA              |  P026  |                 |
 * | 17         | SCL              |  P027  |                 |
 * +------------+------------------+--------+-----------------+
 * |            | Analog Connector |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 18         | A0               |  P003  |                 |
 * | 19         | A1               |  P004  |                 |
 * | 20         | A2               |  P028  |                 |
 * | 21         | A3               |  P029  |                 |
 * | 22         | A4               |  P030  |                 |
 * | 23         | A5               |  P031  |                 |
 * +------------+------------------+--------+-----------------+
 * |            | ESP Connector    |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 24         |                  |  P005  | RX2             |
 * | 25         |                  |  P006  | TX2             |
 * +------------+------------------+--------+-----------------+
 * |            | USER2            |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 26         |                  |  P007  | USER2_BUTTON    |
 * | 27         |                  |  P008  | USER2_LED       |
 * +------------+------------------+--------+-----------------+
 * |            | NFC              |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 28         |                  |  P009  | NFC1            |
 * | 29         |                  |  P010  | NFC2            |
 * +------------+------------------+--------+-----------------+
 * |            |32.768KHz Crystal |        |                 |
 * +------------+------------------+--------+-----------------+
 * |            |                  |  P000  | XL1             | 
 * |            |                  |  P001  | XL2             | 
 * +------------+------------------+--------+-----------------+
 */

#include "variant.h"

/*
 * Pins descriptions
 */
const PinDescription g_APinDescription[]=
{
  // 0 .. 17 - Digital pins
  // ----------------------
    // 0/1 - UART (Serial1)
  { PORT0, 11, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // RX
  { PORT0, 12, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // TX

  // 2..12
  { PORT0,  13, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM, NOT_ON_TIMER},
  { PORT0,  14, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM, NOT_ON_TIMER},
  { PORT0,  15, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM, NOT_ON_TIMER},
  { PORT0,  16, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM, NOT_ON_TIMER},
  { PORT0,  17, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM, NOT_ON_TIMER},
  { PORT0,  18, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM, NOT_ON_TIMER},
  { PORT0,  19, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM, NOT_ON_TIMER},
  { PORT0,  20, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM, NOT_ON_TIMER},
  { PORT0,  22, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM, NOT_ON_TIMER},
  { PORT0,  23, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM, NOT_ON_TIMER},
  { PORT0,  24, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM, NOT_ON_TIMER},

  // 13 (LED)
  { PORT0,  25, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, PWM, NOT_ON_TIMER},

  // 14 (GND)
  { NOT_A_PORT, 0, PIO_NOT_A_PIN, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER},

  // 15 (AREF)
  { PORT0,  2, PIO_ANALOG, PIN_ATTR_ANALOG, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER },

  // 16..17 I2C pins (SDA/SCL and also EDBG:SDA/SCL)
  // ----------------------
  { PORT0,  26, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER }, // SDA
  { PORT0,  27, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER }, // SCL

  // 18..23 - Analog pins
  // --------------------
  { PORT0,  3, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_A0, NOT_ON_PWM, NOT_ON_TIMER },
  { PORT0,  4, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_A1, NOT_ON_PWM, NOT_ON_TIMER },
  { PORT0,  28, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_A2, NOT_ON_PWM, NOT_ON_TIMER },
  { PORT0,  29, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_A3, NOT_ON_PWM, NOT_ON_TIMER },
  { PORT0,  30, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_A4, NOT_ON_PWM, NOT_ON_TIMER },
  { PORT0,  31, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_A5, NOT_ON_PWM, NOT_ON_TIMER },

  //24..25 - ESP Connector
  // --------------------
  { PORT0, 5, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // RX2
  { PORT0, 6, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // TX2
  
  //26..27 - USER2
  // --------------------
  { PORT0, 7, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // button
  { PORT0, 8, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // led
  
  //28..29 - NFC
  // --------------------
  { PORT0, 9, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // NFC1
  { PORT0, 10, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER} // NFC2
  
} ;