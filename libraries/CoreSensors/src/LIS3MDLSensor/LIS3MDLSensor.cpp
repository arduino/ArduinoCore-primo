/**
 ******************************************************************************
 * @file    LIS3MDLSensor.cpp
 * @author  AST
 * @version V1.0.0
 * @date    5 August 2016
 * @brief   Implementation an LIS3MDL magnetometer sensor.
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
#include "LIS3MDLSensor.h"
#include "LIS3MDL_MAG_driver.h"


/* Class Implementation ------------------------------------------------------*/

/** Constructor
 * @param i2c object of an helper class which handles the I2C peripheral
 * @param address the address of the component's instance
 */
LIS3MDLSensor::LIS3MDLSensor(DevI2C &i2c) : dev_i2c(i2c)
{
  address = LIS3MDL_MAG_I2C_ADDRESS_HIGH;

  /* Operating mode selection - power down */
  if ( LIS3MDL_MAG_W_SystemOperatingMode( (void *)this, LIS3MDL_MAG_MD_POWER_DOWN ) == MEMS_ERROR )
  {
    return;
  }

  /* Enable BDU */
  if ( LIS3MDL_MAG_W_BlockDataUpdate( (void *)this, LIS3MDL_MAG_BDU_ENABLE ) == MEMS_ERROR )
  {
    return;
  }

  if ( SetODR( 80.0f ) == LIS3MDL_STATUS_ERROR )
  {
    return;
  }

  if ( SetFS( 4.0f ) == LIS3MDL_STATUS_ERROR )
  {
    return;
  }

  /* X and Y axes Operating mode selection */
  if ( LIS3MDL_MAG_W_OperatingModeXY( (void *)this, LIS3MDL_MAG_OM_HIGH ) == MEMS_ERROR )
  {
    return;
  }

  /* Temperature sensor disable - temp. sensor not used */
  if ( LIS3MDL_MAG_W_TemperatureSensor( (void *)this, LIS3MDL_MAG_TEMP_EN_DISABLE ) == MEMS_ERROR )
  {
    return;
  }
};

/** Constructor
 * @param i2c object of an helper class which handles the I2C peripheral
 * @param address the address of the component's instance
 */
LIS3MDLSensor::LIS3MDLSensor(DevI2C &i2c, uint8_t address) : dev_i2c(i2c), address(address)
{
  /* Operating mode selection - power down */
  if ( LIS3MDL_MAG_W_SystemOperatingMode( (void *)this, LIS3MDL_MAG_MD_POWER_DOWN ) == MEMS_ERROR )
  {
    return;
  }

  /* Enable BDU */
  if ( LIS3MDL_MAG_W_BlockDataUpdate( (void *)this, LIS3MDL_MAG_BDU_ENABLE ) == MEMS_ERROR )
  {
    return;
  }

  if ( SetODR( 80.0f ) == LIS3MDL_STATUS_ERROR )
  {
    return;
  }

  if ( SetFS( 4.0f ) == LIS3MDL_STATUS_ERROR )
  {
    return;
  }

  /* X and Y axes Operating mode selection */
  if ( LIS3MDL_MAG_W_OperatingModeXY( (void *)this, LIS3MDL_MAG_OM_HIGH ) == MEMS_ERROR )
  {
    return;
  }

  /* Temperature sensor disable - temp. sensor not used */
  if ( LIS3MDL_MAG_W_TemperatureSensor( (void *)this, LIS3MDL_MAG_TEMP_EN_DISABLE ) == MEMS_ERROR )
  {
    return;
  }
};

/**
 * @brief  Enable LIS3MDL magnetometer
 * @retval LIS3MDL_STATUS_OK in case of success, an error code otherwise
 */
LIS3MDLStatusTypeDef LIS3MDLSensor::Enable(void)
{
  /* Operating mode selection - continuous */
  if ( LIS3MDL_MAG_W_SystemOperatingMode( (void *)this, LIS3MDL_MAG_MD_CONTINUOUS ) == MEMS_ERROR )
  {
    return LIS3MDL_STATUS_ERROR;
  }

  return LIS3MDL_STATUS_OK;
}

/**
 * @brief  Disable LIS3MDL magnetometer
 * @retval LIS3MDL_STATUS_OK in case of success, an error code otherwise
 */
LIS3MDLStatusTypeDef LIS3MDLSensor::Disable(void)
{
  /* Operating mode selection - power down */
  if ( LIS3MDL_MAG_W_SystemOperatingMode( (void *)this, LIS3MDL_MAG_MD_POWER_DOWN ) == MEMS_ERROR )
  {
    return LIS3MDL_STATUS_ERROR;
  }

  return LIS3MDL_STATUS_OK;
}

/**
 * @brief  Read ID of LIS3MDL Magnetometer
 * @param  p_id the pointer where the ID of the device is stored
 * @retval LIS3MDL_STATUS_OK in case of success, an error code otherwise
 */
LIS3MDLStatusTypeDef LIS3MDLSensor::ReadID(uint8_t *p_id)
{
  if(!p_id)
  { 
    return LIS3MDL_STATUS_ERROR; 
  }
 
  /* Read WHO AM I register */
  if ( LIS3MDL_MAG_R_WHO_AM_I_( (void *)this, p_id ) == MEMS_ERROR )
  {
    return LIS3MDL_STATUS_ERROR;
  }

  return LIS3MDL_STATUS_OK;
}

/**
 * @brief  Read data from LIS3MDL Magnetometer
 * @param  pData the pointer where the magnetometer data are stored
 * @retval LIS3MDL_STATUS_OK in case of success, an error code otherwise
 */
LIS3MDLStatusTypeDef LIS3MDLSensor::GetAxes(int32_t *pData)
{
  int16_t pDataRaw[3];
  float sensitivity = 0;

  /* Read raw data from LIS3MDL output register. */
  if ( GetAxesRaw( pDataRaw ) == LIS3MDL_STATUS_ERROR )
  {
    return LIS3MDL_STATUS_ERROR;
  }

  /* Get LIS3MDL actual sensitivity. */
  if ( GetSensitivity( &sensitivity ) == LIS3MDL_STATUS_ERROR )
  {
    return LIS3MDL_STATUS_ERROR;
  }

  /* Calculate the data. */
  pData[0] = ( int32_t )( pDataRaw[0] * sensitivity );
  pData[1] = ( int32_t )( pDataRaw[1] * sensitivity );
  pData[2] = ( int32_t )( pDataRaw[2] * sensitivity );

  return LIS3MDL_STATUS_OK;
}

/**
 * @brief  Read Magnetometer Sensitivity
 * @param  pfData the pointer where the magnetometer sensitivity is stored
 * @retval LIS3MDL_STATUS_OK in case of success, an error code otherwise
 */
LIS3MDLStatusTypeDef LIS3MDLSensor::GetSensitivity(float *pfData)
{
  LIS3MDL_MAG_FS_t fullScale;

  /* Read actual full scale selection from sensor. */
  if ( LIS3MDL_MAG_R_FullScale( (void *)this, &fullScale ) == MEMS_ERROR )
  {
    return LIS3MDL_STATUS_ERROR;
  }

  /* Store the sensitivity based on actual full scale. */
  switch( fullScale )
  {
    case LIS3MDL_MAG_FS_4Ga:
      *pfData = ( float )LIS3MDL_MAG_SENSITIVITY_FOR_FS_4G;
      break;
    case LIS3MDL_MAG_FS_8Ga:
      *pfData = ( float )LIS3MDL_MAG_SENSITIVITY_FOR_FS_8G;
      break;
    case LIS3MDL_MAG_FS_12Ga:
      *pfData = ( float )LIS3MDL_MAG_SENSITIVITY_FOR_FS_12G;
      break;
    case LIS3MDL_MAG_FS_16Ga:
      *pfData = ( float )LIS3MDL_MAG_SENSITIVITY_FOR_FS_16G;
      break;
    default:
      *pfData = -1.0f;
      break;
  }

  return LIS3MDL_STATUS_OK;
}

/**
 * @brief  Read raw data from LIS3MDL Magnetometer
 * @param  pData the pointer where the magnetomer raw data are stored
 * @retval LIS3MDL_STATUS_OK in case of success, an error code otherwise
 */
LIS3MDLStatusTypeDef LIS3MDLSensor::GetAxesRaw(int16_t *pData)
{
  uint8_t regValue[6] = {0, 0, 0, 0, 0, 0};

  /* Read output registers from LIS3MDL_MAG_OUTX_L to LIS3MDL_MAG_OUTZ_H. */
  if ( LIS3MDL_MAG_Get_Magnetic( (void *)this, ( uint8_t* )regValue ) == MEMS_ERROR )
  {
    return LIS3MDL_STATUS_ERROR;
  }

  /* Format the data. */
  pData[0] = ( ( ( ( int16_t )regValue[1] ) << 8 ) + ( int16_t )regValue[0] );
  pData[1] = ( ( ( ( int16_t )regValue[3] ) << 8 ) + ( int16_t )regValue[2] );
  pData[2] = ( ( ( ( int16_t )regValue[5] ) << 8 ) + ( int16_t )regValue[4] );

  return LIS3MDL_STATUS_OK;
}

/**
 * @brief  Read LIS3MDL Magnetometer output data rate
 * @param  odr the pointer to the output data rate
 * @retval LIS3MDL_STATUS_OK in case of success, an error code otherwise
 */
LIS3MDLStatusTypeDef LIS3MDLSensor::GetODR(float* odr)
{
  LIS3MDL_MAG_DO_t odr_low_level;

  if ( LIS3MDL_MAG_R_OutputDataRate( (void *)this, &odr_low_level ) == MEMS_ERROR )
  {
    return LIS3MDL_STATUS_ERROR;
  }

  switch( odr_low_level )
  {
    case LIS3MDL_MAG_DO_0_625Hz:
      *odr =  0.625f;
      break;
    case LIS3MDL_MAG_DO_1_25Hz:
      *odr =  1.250f;
      break;
    case LIS3MDL_MAG_DO_2_5Hz:
      *odr =  2.500f;
      break;
    case LIS3MDL_MAG_DO_5Hz:
      *odr =  5.000f;
      break;
    case LIS3MDL_MAG_DO_10Hz:
      *odr = 10.000f;
      break;
    case LIS3MDL_MAG_DO_20Hz:
      *odr = 20.000f;
      break;
    case LIS3MDL_MAG_DO_40Hz:
      *odr = 40.000f;
      break;
    case LIS3MDL_MAG_DO_80Hz:
      *odr = 80.000f;
      break;
    default:
      *odr = -1.000f;
      return LIS3MDL_STATUS_ERROR;
  }

  return LIS3MDL_STATUS_OK;
}

/**
 * @brief  Set ODR
 * @param  odr the output data rate to be set
 * @retval LIS3MDL_STATUS_OK in case of success, an error code otherwise
 */
LIS3MDLStatusTypeDef LIS3MDLSensor::SetODR(float odr)
{
  LIS3MDL_MAG_DO_t new_odr;

  new_odr = ( odr <=  0.625f ) ? LIS3MDL_MAG_DO_0_625Hz
          : ( odr <=  1.250f ) ? LIS3MDL_MAG_DO_1_25Hz
          : ( odr <=  2.500f ) ? LIS3MDL_MAG_DO_2_5Hz
          : ( odr <=  5.000f ) ? LIS3MDL_MAG_DO_5Hz
          : ( odr <= 10.000f ) ? LIS3MDL_MAG_DO_10Hz
          : ( odr <= 20.000f ) ? LIS3MDL_MAG_DO_20Hz
          : ( odr <= 40.000f ) ? LIS3MDL_MAG_DO_40Hz
          :                      LIS3MDL_MAG_DO_80Hz;

  if ( LIS3MDL_MAG_W_OutputDataRate( (void *)this, new_odr ) == MEMS_ERROR )
  {
    return LIS3MDL_STATUS_ERROR;
  }

  return LIS3MDL_STATUS_OK;
}


/**
 * @brief  Read LIS3MDL Magnetometer full scale
 * @param  fullScale the pointer to the output data rate
 * @retval LIS3MDL_STATUS_OK in case of success, an error code otherwise
 */
LIS3MDLStatusTypeDef LIS3MDLSensor::GetFS(float* fullScale)
{
  LIS3MDL_MAG_FS_t fs_low_level;

  if ( LIS3MDL_MAG_R_FullScale( (void *)this, &fs_low_level ) == MEMS_ERROR )
  {
    return LIS3MDL_STATUS_ERROR;
  }

  switch( fs_low_level )
  {
    case LIS3MDL_MAG_FS_4Ga:
      *fullScale =  4.0f;
      break;
    case LIS3MDL_MAG_FS_8Ga:
      *fullScale =  8.0f;
      break;
    case LIS3MDL_MAG_FS_12Ga:
      *fullScale = 12.0f;
      break;
    case LIS3MDL_MAG_FS_16Ga:
      *fullScale = 16.0f;
      break;
    default:
      *fullScale = -1.0f;
      return LIS3MDL_STATUS_ERROR;
  }

  return LIS3MDL_STATUS_OK;
}

/**
 * @brief  Set full scale
 * @param  fullScale the full scale to be set
 * @retval LIS3MDL_STATUS_OK in case of success, an error code otherwise
 */
LIS3MDLStatusTypeDef LIS3MDLSensor::SetFS(float fullScale)
{
  LIS3MDL_MAG_FS_t new_fs;

  new_fs = ( fullScale <=  4.0f ) ? LIS3MDL_MAG_FS_4Ga
         : ( fullScale <=  8.0f ) ? LIS3MDL_MAG_FS_8Ga
         : ( fullScale <= 12.0f ) ? LIS3MDL_MAG_FS_12Ga
         :                          LIS3MDL_MAG_FS_16Ga;

  if ( LIS3MDL_MAG_W_FullScale( (void *)this, new_fs ) == MEMS_ERROR )
  {
    return LIS3MDL_STATUS_ERROR;
  }

  return LIS3MDL_STATUS_OK;
}


/**
 * @brief Read magnetometer data from register
 * @param reg register address
 * @param data register data
 * @retval LIS3MDL_STATUS_OK in case of success
 * @retval LIS3MDL_STATUS_ERROR in case of failure
 */
LIS3MDLStatusTypeDef LIS3MDLSensor::ReadReg( uint8_t reg, uint8_t *data )
{
  if ( LIS3MDL_MAG_ReadReg( (void *)this, reg, data, 1 ) == MEMS_ERROR )
  {
    return LIS3MDL_STATUS_ERROR;
  }

  return LIS3MDL_STATUS_OK;
}


/**
 * @brief Write magnetometer data to register
 * @param reg register address
 * @param data register data
 * @retval LIS3MDL_STATUS_OK in case of success
 * @retval LIS3MDL_STATUS_ERROR in case of failure
 */
LIS3MDLStatusTypeDef LIS3MDLSensor::WriteReg( uint8_t reg, uint8_t data )
{
  if ( LIS3MDL_MAG_WriteReg( (void *)this, reg, &data, 1 ) == MEMS_ERROR )
  {
    return LIS3MDL_STATUS_ERROR;
  }

  return LIS3MDL_STATUS_OK;
}

uint8_t LIS3MDL_IO_Write( void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite )
{
  return ((LIS3MDLSensor *)handle)->IO_Write(pBuffer, WriteAddr, nBytesToWrite);
}

uint8_t LIS3MDL_IO_Read( void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead )
{
  return ((LIS3MDLSensor *)handle)->IO_Read(pBuffer, ReadAddr, nBytesToRead);
}
