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
 * | Pin number |  PRIMO Board pin |  PIN   | Label/Name      |
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
 * | 9          | ~9               |  P020  | LED             |
 * | 10         | ~10              |  P022  |                 |
 * | 11         | ~11              |  P023  |                 |
 * | 12         | ~12              |  P024  |                 |
 * | 13         | ~13              |  P025  |                 |
 * +------------+------------------+--------+-----------------+
 * |            | Analog Connector |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 14         | A0               |  P003  |                 |
 * | 15         | A1               |  P004  |                 |
 * | 16         | A2               |  P028  |                 |
 * | 17         | A3               |  P029  |                 |
 * | 18         | A4               |  P030  |                 |
 * | 19         | A5               |  P031  |                 |
 * +------------+------------------+--------+-----------------+
 * | 20         | SDA              |  P026  |                 |
 * | 21         | SCL              |  P027  |                 |
 * | 22         | GND              |        |                 |
 * | 23         | AREF             |  P002  |                 |
 * +------------+------------------+--------+-----------------+
 * |            |SPI (Legacy ICSP) |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 24         | 1                |  P024  | MISO            |
 * | 25         | 2                |        | VDD_nRF         |
 * | 26         | 3                |  P025  | SCK             |
 * | 27         | 4                |  P023  | MOSI            |
 * | 28         | 5                |        | RESET           |
 * | 29         | 6                |        | GND             |
 * +------------+------------------+--------+-----------------+
 * |            | ESP Connector    |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 30         |                  |  P005  | RX2             |
 * | 31         |                  |  P006  | TX2             |
 * +------------+------------------+--------+-----------------+
 * |            | STM Connector    |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 32         |                  |  P011  | RX1             |
 * | 33         |                  |  P012  | TX1             |
 * +------------+------------------+--------+-----------------+
 * |            | USER1/BUZZER     |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 34         |                  |  P007  | USER1_BUTTON    |
 * | 35         |                  |  P008  | BUZZER          |
 * +------------+------------------+--------+-----------------+
 * |            | NFC              |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 36         |                  |  P009  | NFC1            |
 * | 37         |                  |  P010  | NFC2            |
 * +------------+------------------+--------+-----------------+
 * |            |32.768KHz Crystal |        |                 |
 * +------------+------------------+--------+-----------------+
 * |            |                  |  P000  | XL1             | 
 * |            |                  |  P001  | XL2             | 
 * +------------+------------------+--------+-----------------+
 * |            | STM GPIO         |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 38         |                  |  PC09  | USER2_LED       |
 * | 39         |                  |  PC06  | POWER_LED       |
 * | 40         |                  |  PA08  | BLE_LED         |
 * | 41         |                  |  PB00  | GPIO_ESP_PW     |
 * | 42         |                  |  PB01  | GPIO_ESP_EN     |
 * | 43         |                  |  PA07  | BATTERY_VOL     |
 * | 44         |                  |  PC04  | USER2_BUTTON    |
 * +------------+------------------+--------+-----------------+
 */

#include "variant.h"

/*
 * Pins descriptions
 */
const PinDescription g_APinDescription[]=
{
  // 0 .. 13 - Digital pins
  // ----------------------
    // 0/1 - UART (Serial1)
  { PORT0, 11, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // RX
  { PORT0, 12, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // TX

  // 2..8
  { PORT0,  13, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM0, NOT_ON_TIMER},
  { PORT0,  14, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM1, NOT_ON_TIMER},
  { PORT0,  15, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM2, NOT_ON_TIMER},
  { PORT0,  16, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM3, NOT_ON_TIMER},
  { PORT0,  17, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM4, NOT_ON_TIMER},
  { PORT0,  18, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM5, NOT_ON_TIMER},
  { PORT0,  19, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM6, NOT_ON_TIMER},

  // 9 (LED)
  { PORT0,  20, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM7, NOT_ON_TIMER},

  // 10..13
  { PORT0,  22, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM8, NOT_ON_TIMER},
  { PORT0,  23, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM9, NOT_ON_TIMER},
  { PORT0,  24, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM10, NOT_ON_TIMER},
  { PORT0,  25, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM11, NOT_ON_TIMER},

  // 14..19 - Analog pins
  // --------------------
  { PORT0,  3, PIO_ANALOG, (PIN_ATTR_ANALOG|PIN_ATTR_PWM), ADC_A0, NOT_ON_PWM, NOT_ON_TIMER },
  { PORT0,  4, PIO_ANALOG, (PIN_ATTR_ANALOG|PIN_ATTR_PWM), ADC_A1, NOT_ON_PWM, NOT_ON_TIMER },
  { PORT0,  28, PIO_ANALOG, (PIN_ATTR_ANALOG|PIN_ATTR_PWM), ADC_A2, NOT_ON_PWM, NOT_ON_TIMER },
  { PORT0,  29, PIO_ANALOG, (PIN_ATTR_ANALOG|PIN_ATTR_PWM), ADC_A3, NOT_ON_PWM, NOT_ON_TIMER },
  { PORT0,  30, PIO_ANALOG, (PIN_ATTR_ANALOG|PIN_ATTR_PWM), ADC_A4, NOT_ON_PWM, NOT_ON_TIMER },
  { PORT0,  31, PIO_ANALOG, (PIN_ATTR_ANALOG|PIN_ATTR_PWM), ADC_A5, NOT_ON_PWM, NOT_ON_TIMER },

  // 20..21 I2C pins (SDA/SCL)
  // ----------------------
  { PORT0,  26, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER }, // SDA
  { PORT0,  27, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER }, // SCL

  // 22 (GND)
  { NOT_A_PORT, 0, PIO_NOT_A_PIN, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER},

  // 23 (AREF)
  { PORT0,  2, PIO_ANALOG, PIN_ATTR_ANALOG, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER },

  
  // 24..29 - SPI pins (ICSP:MISO,SCK,MOSI)
  // ----------------------
  { PORT0, 24, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // MISO
  { NOT_A_PORT, 0, PIO_NOT_A_PIN, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // VDD_nRF
  { PORT0, 25, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // SCK
  { PORT0, 23, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // MOSI
  { NOT_A_PORT, 0, PIO_DIGITAL, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // RESET
  { NOT_A_PORT, 0, PIO_DIGITAL, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // GND

  
  //30..31 - ESP Connector
  // --------------------
  { PORT0, 5, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // RX2
  { PORT0, 6, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // TX2

  //32..33 - STM Connector
  // --------------------
  { PORT0, 11, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // RX1
  { PORT0, 12, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // TX1
  
  //34..35 - USER2
  // --------------------
  { PORT0, 7, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // button
  { PORT0, 8, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, PWM0, NOT_ON_TIMER}, // buzzer
  
  //36..37 - NFC
  // --------------------
  { PORT0, 9, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // NFC1
  { PORT0, 10, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // NFC2

  // 38..44
  { NOT_A_PORT, 0, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER},
  { NOT_A_PORT, 0, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER},
  { NOT_A_PORT, 0, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER},
  { NOT_A_PORT, 0, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER},
  { NOT_A_PORT, 0, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER},
  { NOT_A_PORT, 0, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER},
  { NOT_A_PORT, 0, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}
  
} ;

Uart Serial(32, 33);
