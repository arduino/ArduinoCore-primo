/**
 ******************************************************************************
 * @file    HTS221Sensor.cpp
 * @author  AST
 * @version V1.0.0
 * @date    5 August 2016
 * @brief   Implementation of an HTS221 Humidity and Temperature sensor.
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

#include "Arduino.h"
#include "DevI2C_Arduino/DevI2C.h"
#include "HTS221Sensor.h"
#include "HTS221_Driver.h"


/* Class Implementation ------------------------------------------------------*/

/** Constructor
 * @param i2c object of an helper class which handles the I2C peripheral
 * @param address the address of the component's instance
 */
HTS221Sensor::HTS221Sensor(DevI2C &i2c) : dev_i2c(i2c)
{
  address = HTS221_I2C_ADDRESS; 

  /* Power down the device */
  if ( HTS221_DeActivate( (void *)this ) == HTS221_ERROR )
  {
    return;
  }

  /* Enable BDU */
  if ( HTS221_Set_BduMode( (void *)this, HTS221_ENABLE ) == HTS221_ERROR )
  {
    return;
  }
  
  if(SetODR(1.0f) == HTS221_STATUS_ERROR)
  {
    return;
  }
};


/** Constructor
 * @param i2c object of an helper class which handles the I2C peripheral
 * @param address the address of the component's instance
 */
HTS221Sensor::HTS221Sensor(DevI2C &i2c, uint8_t address) : dev_i2c(i2c), address(address)
{
    /* Power down the device */
  if ( HTS221_DeActivate( (void *)this ) == HTS221_ERROR )
  {
    return;
  }

  /* Enable BDU */
  if ( HTS221_Set_BduMode( (void *)this, HTS221_ENABLE ) == HTS221_ERROR )
  {
    return;
  }
  
  if(SetODR(1.0f) == HTS221_STATUS_ERROR)
  {
    return;
  }
};

/**
 * @brief  Enable HTS221
 * @retval HTS221_STATUS_OK in case of success, an error code otherwise
 */
HTS221StatusTypeDef HTS221Sensor::Enable(void)
{
  /* Power up the device */
  if ( HTS221_Activate( (void *)this ) == HTS221_ERROR )
  {
    return HTS221_STATUS_ERROR;
  }

  return HTS221_STATUS_OK;
}

/**
 * @brief  Disable HTS221
 * @retval HTS221_STATUS_OK in case of success, an error code otherwise
 */
HTS221StatusTypeDef HTS221Sensor::Disable(void)
{
  /* Power up the device */
  if ( HTS221_DeActivate( (void *)this ) == HTS221_ERROR )
  {
    return HTS221_STATUS_ERROR;
  }

  return HTS221_STATUS_OK;
}

/**
 * @brief  Read ID address of HTS221
 * @param  ht_id the pointer where the ID of the device is stored
 * @retval HTS221_STATUS_OK in case of success, an error code otherwise
 */
HTS221StatusTypeDef HTS221Sensor::ReadID(uint8_t *ht_id)
{
  /* Read WHO AM I register */
  if ( HTS221_Get_DeviceID( (void *)this, ht_id ) == HTS221_ERROR )
  {
    return HTS221_STATUS_ERROR;
  }

  return HTS221_STATUS_OK;
}

/**
 * @brief  Reboot memory content of HTS221
 * @param  None
 * @retval HTS221_STATUS_OK in case of success, an error code otherwise
 */
HTS221StatusTypeDef HTS221Sensor::Reset(void)
{
    uint8_t tmpreg;

    /* Read CTRL_REG2 register */
    if (ReadReg(HTS221_CTRL_REG2, &tmpreg) != HTS221_STATUS_OK)
    {
      return HTS221_STATUS_ERROR;
    }

    /* Enable or Disable the reboot memory */
    tmpreg |= (0x01 << HTS221_BOOT_BIT);

    /* Write value to MEMS CTRL_REG2 regsister */
	if (WriteReg(HTS221_CTRL_REG2, tmpreg) != HTS221_STATUS_OK)
    {
      return HTS221_STATUS_ERROR;
    }
    
    return HTS221_STATUS_OK;
}

/**
 * @brief  Read HTS221 output register, and calculate the humidity
 * @param  pfData the pointer to data output
 * @retval HTS221_STATUS_OK in case of success, an error code otherwise
 */
HTS221StatusTypeDef HTS221Sensor::GetHumidity(float* pfData)
{
  uint16_t uint16data = 0;

  /* Read data from HTS221. */
  if ( HTS221_Get_Humidity( (void *)this, &uint16data ) == HTS221_ERROR )
  {
    return HTS221_STATUS_ERROR;
  }

  *pfData = ( float )uint16data / 10.0f;

  return HTS221_STATUS_OK;
}

/**
 * @brief  Read HTS221 output register, and calculate the temperature
 * @param  pfData the pointer to data output
 * @retval HTS221_STATUS_OK in case of success, an error code otherwise
 */
HTS221StatusTypeDef HTS221Sensor::GetTemperature(float* pfData)
{
  int16_t int16data = 0;

  /* Read data from HTS221. */
  if ( HTS221_Get_Temperature( (void *)this, &int16data ) == HTS221_ERROR )
  {
    return HTS221_STATUS_ERROR;
  }

  *pfData = ( float )int16data / 10.0f;

  return HTS221_STATUS_OK;
}

/**
 * @brief  Read HTS221 output register, and calculate the humidity
 * @param  odr the pointer to the output data rate
 * @retval HTS221_STATUS_OK in case of success, an error code otherwise
 */
HTS221StatusTypeDef HTS221Sensor::GetODR(float* odr)
{
  HTS221_Odr_et odr_low_level;

  if ( HTS221_Get_Odr( (void *)this, &odr_low_level ) == HTS221_ERROR )
  {
    return HTS221_STATUS_ERROR;
  }

  switch( odr_low_level )
  {
    case HTS221_ODR_ONE_SHOT:
      *odr =  0.0f;
      break;
    case HTS221_ODR_1HZ     :
      *odr =  1.0f;
      break;
    case HTS221_ODR_7HZ     :
      *odr =  7.0f;
      break;
    case HTS221_ODR_12_5HZ  :
      *odr = 12.5f;
      break;
    default                 :
      *odr = -1.0f;
      return HTS221_STATUS_ERROR;
  }

  return HTS221_STATUS_OK;
}

/**
 * @brief  Set ODR
 * @param  odr the output data rate to be set
 * @retval HTS221_STATUS_OK in case of success, an error code otherwise
 */
HTS221StatusTypeDef HTS221Sensor::SetODR(float odr)
{
  HTS221_Odr_et new_odr;

  new_odr = ( odr <= 1.0f ) ? HTS221_ODR_1HZ
          : ( odr <= 7.0f ) ? HTS221_ODR_7HZ
          :                   HTS221_ODR_12_5HZ;

  if ( HTS221_Set_Odr( (void *)this, new_odr ) == HTS221_ERROR )
  {
    return HTS221_STATUS_ERROR;
  }

  return HTS221_STATUS_OK;
}


/**
 * @brief Read the data from register
 * @param reg register address
 * @param data register data
 * @retval HTS221_STATUS_OK in case of success
 * @retval HTS221_STATUS_ERROR in case of failure
 */
HTS221StatusTypeDef HTS221Sensor::ReadReg( uint8_t reg, uint8_t *data )
{

  if ( HTS221_ReadReg( (void *)this, reg, 1, data ) == HTS221_ERROR )
  {
    return HTS221_STATUS_ERROR;
  }

  return HTS221_STATUS_OK;
}

/**
 * @brief Write the data to register
 * @param reg register address
 * @param data register data
 * @retval HTS221_STATUS_OK in case of success
 * @retval HTS221_STATUS_ERROR in case of failure
 */
HTS221StatusTypeDef HTS221Sensor::WriteReg( uint8_t reg, uint8_t data )
{

  if ( HTS221_WriteReg( (void *)this, reg, 1, &data ) == HTS221_ERROR )
  {
    return HTS221_STATUS_ERROR;
  }

  return HTS221_STATUS_OK;
}

uint8_t HTS221_IO_Write( void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite )
{
  return ((HTS221Sensor *)handle)->IO_Write(pBuffer, WriteAddr, nBytesToWrite);
}

uint8_t HTS221_IO_Read( void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead )
{
  return ((HTS221Sensor *)handle)->IO_Read(pBuffer, ReadAddr, nBytesToRead);
}
