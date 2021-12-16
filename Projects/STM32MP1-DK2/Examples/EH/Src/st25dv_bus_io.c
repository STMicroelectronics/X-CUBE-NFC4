/**
  ******************************************************************************
  * File Name          : st25dv_bus_io.c
  * Description        : This file provides code for the configuration
  *                      of the STMicroelectronics.X-CUBE-NFC4.1.3.0 instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifdef __cplusplus
 extern "C" {
#endif

#include "nfc04a1_conf.h"

extern I2C_HandleTypeDef hi2c5;
int32_t ST25DV_IO_IsDeviceReady( uint16_t DevAddr, const uint32_t Trials );
uint8_t ST25DV_IO_IsNacked( void );

#define ST25_DISCOVERY_I2Cx_TIMEOUT (100)
#define ST25DV_I2C_WRITE_TIMEOUT (320)
int32_t ST25DV_IO_Init( void )
{
	return 0;
}

int32_t ST25DV_IO_MemWrite( uint16_t DevAddr, uint16_t TarAddr, const uint8_t * pData, uint16_t Size )
{
  int32_t pollstatus;
  int32_t ret;
  uint32_t tickstart;
  uint8_t *pbuffer = (uint8_t *)pData;

  ret = HAL_I2C_Mem_Write( &hi2c5, DevAddr, TarAddr, I2C_MEMADD_SIZE_16BIT, pbuffer, Size,
		  ST25_DISCOVERY_I2Cx_TIMEOUT );

  if( ret == 0 )
  {
	/* Poll until EEPROM is available */
	tickstart = HAL_GetTick( );
	/* Wait until ST25DV is ready or timeout occurs */
	do
	{
	  pollstatus = ST25DV_IO_IsDeviceReady( DevAddr, 1 );
	} while( ( (HAL_GetTick() - tickstart) < ST25DV_I2C_WRITE_TIMEOUT) && (pollstatus != 0) );

	if( pollstatus != 0 )
	{
	  ret = 4;
	}
  }
  else
  {
	/* Check if Write was NACK */
	if( ST25DV_IO_IsNacked() )
	{
	  ret = -102;
	}
  }
  return ret;
}

int32_t ST25DV_IO_MemRead( uint16_t DevAddr, uint16_t TarAddr, uint8_t * pData, uint16_t Size )
{

  uint8_t *pbuffer = (uint8_t *)pData;
  /* I2C Timeout: (transfer size in bytes) * (bits per bytes) * (extra delay) / (I2C speed) */
  uint32_t timeout = (Size * 8 * 1000 * 2) / 0x00701837;
  if( timeout < ST25_DISCOVERY_I2Cx_TIMEOUT )
  {
	timeout = ST25_DISCOVERY_I2Cx_TIMEOUT;
  }

  return HAL_I2C_Mem_Read( &hi2c5, DevAddr, TarAddr, I2C_MEMADD_SIZE_16BIT, pbuffer, Size, timeout );

}

int32_t ST25DV_IO_Read( uint16_t DevAddr, uint8_t * pData, const uint16_t Size )
{
  uint8_t *pbuffer = (uint8_t *)pData;

  return HAL_I2C_Master_Receive( &hi2c5, DevAddr, pbuffer, Size, ST25_DISCOVERY_I2Cx_TIMEOUT );
}

uint8_t ST25DV_IO_IsNacked( void )
{
  if( hi2c5.ErrorCode == HAL_I2C_ERROR_AF )
  {
	return 1;
  }
  return 0;
}

int32_t ST25DV_IO_IsDeviceReady( uint16_t DevAddr, const uint32_t Trials )
{
	return HAL_I2C_IsDeviceReady( &hi2c5, DevAddr, Trials, ST25_DISCOVERY_I2Cx_TIMEOUT );
}


#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
