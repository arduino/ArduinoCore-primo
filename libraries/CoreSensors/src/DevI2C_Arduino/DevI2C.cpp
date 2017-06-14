/**
 ******************************************************************************
 * @file    DevI2C.cpp
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


/* Includes ------------------------------------------------------------------*/
#include <Arduino.h>
#include <Wire.h>
#include "DevI2C.h"


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


/* Implementation ------------------------------------------------------------*/

/**
 * @brief  Init the I2C interface
 * @param  Wire the I2C object
 * @param  clock the clock frequency
 * @param  timeout the timeout in milliseconds
 * @retval None
 */
DevI2C::DevI2C(TwoWire *wire, int clock, uint32_t timeout) : wire(wire), timeout(timeout)
{
  wire->begin();
  wire->setClock(clock);
}

/**
 * @brief  Set the I2C interface's clock
 * @param  clock the clock frequency
 * @retval I2C_OK in case of success, an error code otherwise
 */
I2C_Code DevI2C::i2c_set_clock(int clock)
{
  wire->setClock(clock);
  
  return I2C_OK;
}

/**
 * @brief  Reads a buffer from a sensor via I2C with 8 bits register address 
 * @param  pBuffer the pointer to data to be read
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the internal address register to be read
 * @param  NumByteToRead the number of bytes to be read
 * @retval I2C_OK in case of success, an error code otherwise
 */
I2C_Code DevI2C::i2c_read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr, uint16_t NumByteToRead)
{
  wire->beginTransmission(TO_WIRE_ADDRESS(DeviceAddr));
  wire->write(RegisterAddr);
  wire->endTransmission(false);
  
  wire->requestFrom(TO_WIRE_ADDRESS(DeviceAddr), (byte) NumByteToRead);

  int i=0;
  while (wire->available())
  {
    pBuffer[i] = wire->read();
    i++;
  }
  
  return I2C_OK;
}

/**
 * @brief  Reads a buffer from a sensor via I2C with 16 bits register address
 * @param  pBuffer the pointer to data to be read
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the internal address register to be read
 * @param  NumByteToRead the number of bytes to be read
 * @retval I2C_OK in case of success, an error code otherwise
 */
I2C_Code DevI2C::i2c_read(uint8_t* pBuffer, uint8_t DeviceAddr, uint16_t RegisterAddr, uint16_t NumByteToRead)
{
  int ret,i;
  uint8_t tmp[32]; 
  uint16_t myRegisterAddr = RegisterAddr;
  uint16_t myRegisterAddrBE;
  uint16_t ReadDeviceAddr=DeviceAddr*2;
  
  /* then prepare 16 bits register address in BE format. Then, send data and STOP condition */
  tmp[0] = *(((uint8_t*)&myRegisterAddr)+1);  
  tmp[1] = (uint8_t)RegisterAddr;
  
  /* Send 8 bits device address and 16 bits register address in BE format, with no STOP condition */
  wire->beginTransmission(TO_WIRE_ADDRESS(ReadDeviceAddr));

  for (i = 0 ; i < sizeof(RegisterAddr); i++)
    wire->write(tmp[i]);
  
  wire->endTransmission(false);

  /* Read data, with STOP condition  */
  ReadDeviceAddr|=0x01;
  wire->requestFrom(TO_WIRE_ADDRESS(ReadDeviceAddr), (byte) NumByteToRead);

  i=0;
  while (wire->available())
  {
    tmp[i] = wire->read();
    i++;
  }
  
  if(NumByteToRead>1)  /* swap data endianess */
  {
    for(i=0;i<NumByteToRead;i++)
    {      
      pBuffer[i] = tmp[NumByteToRead-1-i];
    }
  }
  else
  {      
    memcpy(pBuffer, tmp, NumByteToRead);
  }
  
  return I2C_OK;
}

/**
 * @brief  Write a buffer to a sensor via I2C with 8 bits register address
 * @param  pBuffer the pointer to data to be written
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the internal address register to be written
 * @param  NumByteToWrite the number of bytes to be written
 * @retval I2C_OK in case of success, an error code otherwise
 */
I2C_Code DevI2C::i2c_write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr, uint16_t NumByteToWrite)
{
  wire->beginTransmission(TO_WIRE_ADDRESS(DeviceAddr));
  
  wire->write(RegisterAddr);
  for (int i = 0 ; i < NumByteToWrite ; i++)
    wire->write(pBuffer[i]);
  
  wire->endTransmission(true);
  
  return I2C_OK;
}

/**
 * @brief  Write a buffer to a sensor via I2C with 16 bits register address
 * @param  pBuffer the pointer to data to be written
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the internal address register to be written
 * @param  NumByteToWrite the number of bytes to be written
 * @retval I2C_OK in case of success, an error code otherwise
 */
I2C_Code DevI2C::i2c_write(uint8_t* pBuffer, uint8_t DeviceAddr, uint16_t RegisterAddr, uint16_t NumByteToWrite)
{
  int i;
  uint8_t tmp[32];
  uint16_t myRegisterAddr = RegisterAddr;
  uint16_t WriteDeviceAddr=DeviceAddr*2;

  if(NumByteToWrite > (32 - sizeof(RegisterAddr))) return I2C_ERROR;
        
  /* then prepare 16 bits register address in BE format. Then, send data and STOP condition */
  tmp[0] = *(((uint8_t*)&myRegisterAddr)+1);  
  tmp[1] = (uint8_t)RegisterAddr;
    
  if(NumByteToWrite>1)  /* swap data endianess */
  {
    for(i=0;i<NumByteToWrite;i++)
    {
      tmp[NumByteToWrite+sizeof(RegisterAddr)-1-i]=pBuffer[i];
    }
  }
  else
  {    
    memcpy(tmp+sizeof(RegisterAddr), pBuffer, NumByteToWrite);
  }
  
  wire->beginTransmission(TO_WIRE_ADDRESS(WriteDeviceAddr));

  for (int i = 0 ; i < (NumByteToWrite + sizeof(RegisterAddr)); i++)
    wire->write(tmp[i]);
  
  wire->endTransmission(true);
  
  return I2C_OK;
}
