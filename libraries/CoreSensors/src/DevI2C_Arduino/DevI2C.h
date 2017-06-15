/**
 ******************************************************************************
 * @file    DevI2C.h
 * @author  AST
 * @version V1.0.0
 * @date    5 August 2016
 * @brief   Helper Class for the I2C hardware interface on Arduino.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */


/* Prevent recursive inclusion -----------------------------------------------*/

#ifndef __DEVI2C_H__
#define __DEVI2C_H__


/* Includes ------------------------------------------------------------------*/

#include <Wire.h>


/*
 * I2C
 *
 * Arduino Due is provided with two I2C interfaces:
 * - I2C0: on pin 20/21;
 * - I2C1: on pin 9/10.
 * The Wire library provides the following objects:
 * - Wire: to use I2C0;
 * - Wire1: to use I2C1.
 * ST Microelectronics' X-NUCLEO-IKS01A1 expansion board uses I2C on pin 9/10.
 */


/* Definitions ---------------------------------------------------------------*/

/*
 * The Arduino two-wire interface uses a 7-bit number for the address, and sets automatically the last bit
 * depending on the action: 1 for reading and 0 for writing.
 */
#define TO_WIRE_ADDRESS(__ADDRESS__)   ((uint8_t)(((__ADDRESS__) >> 1) & 0x7F))


/* Types ---------------------------------------------------------------------*/

typedef enum
{
  I2C_OK,
  I2C_TIMEOUT,
  I2C_ERROR
} I2C_Code;


/* Class ---------------------------------------------------------------------*/

class DevI2C
{
  public:
    /**
     * @brief  Init the I2C interface
     * @param  Wire the I2C object
     * @param  clock the clock frequency
     * @param  timeout the timeout in milliseconds
     * @retval None
     */
    DevI2C(TwoWire *wire, int clock = 100000, uint32_t timeout = 0);

    /**
     * @brief  Set the I2C interface's clock
     * @param  clock the clock frequency
     * @retval I2C_OK in case of success, an error code otherwise
     */
    I2C_Code i2c_set_clock(int clock);

    /**
     * @brief  Reads a buffer from a sensor via I2C with 8 bits register address
     * @param  pBuffer the pointer to data to be read
     * @param  DeviceAddr the slave address to be programmed
     * @param  RegisterAddr the internal address register to be read
     * @param  NumByteToRead the number of bytes to be read
     * @retval I2C_OK in case of success, an error code otherwise
     */
    I2C_Code i2c_read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr, uint16_t NumByteToRead);
	
	/**
     * @brief  Reads a buffer from a sensor via I2C with 16 bits register address
     * @param  pBuffer the pointer to data to be read
     * @param  DeviceAddr the slave address to be programmed
     * @param  RegisterAddr the internal address register to be read
     * @param  NumByteToRead the number of bytes to be read
     * @retval I2C_OK in case of success, an error code otherwise
     */
    I2C_Code i2c_read(uint8_t* pBuffer, uint8_t DeviceAddr, uint16_t RegisterAddr, uint16_t NumByteToRead);

    /**
     * @brief  Write a buffer to a sensor via I2C with 8 bits register address
     * @param  pBuffer the pointer to data to be written
     * @param  DeviceAddr the slave address to be programmed
     * @param  RegisterAddr the internal address register to be written
     * @param  NumByteToWrite the number of bytes to be written
     * @retval I2C_OK in case of success, an error code otherwise
     */
    I2C_Code i2c_write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr, uint16_t NumByteToWrite);
	
	/**
     * @brief  Write a buffer to a sensor via I2C with 16 bits register address
     * @param  pBuffer the pointer to data to be written
     * @param  DeviceAddr the slave address to be programmed
     * @param  RegisterAddr the internal address register to be written
     * @param  NumByteToWrite the number of bytes to be written
     * @retval I2C_OK in case of success, an error code otherwise
     */
    I2C_Code i2c_write(uint8_t* pBuffer, uint8_t DeviceAddr, uint16_t RegisterAddr, uint16_t NumByteToWrite);

  protected:
    TwoWire *wire;
    uint32_t timeout;
};

#endif