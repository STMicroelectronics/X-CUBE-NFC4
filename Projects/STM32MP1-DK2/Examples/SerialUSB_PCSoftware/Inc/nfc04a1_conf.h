/**
  ******************************************************************************
  * @file    nfc04a1_conf.h
  * @author  SRA Application Team
  * @version V0.0.1
  * @date    26-Nov-2018
  * @brief   This file contains definitions for the NFC4 components bus interfaces
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NFC04A1_CONF_H__
#define __NFC04A1_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif
 
 
#include "stm32mp1xx_hal.h"
#include "stm32mp1xx_hal_exti.h"

#define BSP_ERROR_NONE                    0

int32_t ST25DV_IO_IsDeviceReady( uint16_t DevAddr, const uint32_t Trials );
uint8_t ST25DV_IO_IsNacked( void );
int32_t ST25DV_IO_Init( void );
int32_t ST25DV_IO_MemWrite( uint16_t DevAddr, uint16_t TarAddr, const uint8_t * pData, uint16_t Size );
int32_t ST25DV_IO_MemRead( uint16_t DevAddr, uint16_t TarAddr, uint8_t * pData, uint16_t Size );
int32_t ST25DV_IO_Read( uint16_t DevAddr, uint8_t * pData, const uint16_t Size );
uint8_t ST25DV_IO_IsNacked( void );

//#include "stm32l4xx_hal_exti.h"

#define NFC04A1_I2C_Init         ST25DV_IO_Init
#define NFC04A1_I2C_DeInit       ST25DV_IO_Init
#define NFC04A1_I2C_ReadReg16    ST25DV_IO_MemRead
#define NFC04A1_I2C_WriteReg16   ST25DV_IO_MemWrite
#define NFC04A1_I2C_Recv         ST25DV_IO_Read
#define NFC04A1_I2C_IsReady      ST25DV_IO_IsDeviceReady

#define NFC04A1_GetTick          HAL_GetTick

#define NFC04A1_LPD_PIN_PORT GPIOD
#define NFC04A1_LPD_PIN GPIO_PIN_1
#define NFC04A1_GPO_PIN_PORT GPIOE
#define NFC04A1_GPO_PIN GPIO_PIN_13
#define NFC04A1_NFCTAG_GPO_EXTI_LINE EXTI_LINE_13
#define NFC04A1_GPO_EXTI EXTI13_IRQn
#define NFC04A1_LED1_PIN_PORT GPIOD
#define NFC04A1_LED1_PIN GPIO_PIN_15
#define NFC04A1_LED2_PIN_PORT GPIOE
#define NFC04A1_LED2_PIN GPIO_PIN_10
#define NFC04A1_LED3_PIN_PORT GPIOE
#define NFC04A1_LED3_PIN GPIO_PIN_1

#define NFC04A1_NFCTAG_INSTANCE         (0)

#define NFC04A1_NFCTAG_GPO_PRIORITY     (0)

#define I2C_INSTANCE  hi2c5
extern I2C_HandleTypeDef hi2c5;

#ifdef __cplusplus
}
#endif

#endif /* __NFC04A1_CONF_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

