/**
  ******************************************************************************
  * @file    nfc04a1_conf_template.h 
  * @author  MMY-SRA Team
  * @brief   Template file for X-NUCLEO-NFC04A1 configuration  
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
#ifndef _NFC04A1_CONF_H_
#define _NFC04A1_CONF_H_

#include "stm32l4xx_hal.h"
#include "nucleo_l476rg_bus.h"
#include "nucleo_l476rg_errno.h"

/** @defgroup X_NUCLEO_NFC04A1_Exported_Constants
  * @{
  */
/* I/O Definition of ST25DV_NUCLEO board */
/* HAL connector names */
#define NFC04A1_GPO_PIN                   GPIO_PIN_6
#define NFC04A1_GPO_PIN_PORT              GPIOA
#define NFC04A1_LPD_PIN                   GPIO_PIN_8
#define NFC04A1_LPD_PIN_PORT              GPIOA
#define NFC04A1_LED1_PIN                  GPIO_PIN_4
#define NFC04A1_LED1_PIN_PORT             GPIOB
#define NFC04A1_LED2_PIN                  GPIO_PIN_5
#define NFC04A1_LED2_PIN_PORT             GPIOB
#define NFC04A1_LED3_PIN                  GPIO_PIN_10
#define NFC04A1_LED3_PIN_PORT             GPIOA

#define NFC04A1_GPO_EXTI                  EXTI9_5_IRQn
#define NFC04A1_GPO_IRQHandler            EXTI9_5_IRQHandler

#define NFC04A1_INIT_CLK_GPO_RFD()        __HAL_RCC_GPIOA_CLK_ENABLE( );
#define NFC04A1_INIT_CLK_LPD_RFD()        __HAL_RCC_GPIOA_CLK_ENABLE( );

/* Bus IO */
#define BSP_NFCTAG_I2C_Init         BSP_I2C1_Init
#define BSP_NFCTAG_I2C_DeInit       BSP_I2C1_DeInit;
#define BSP_NFCTAG_I2C_IsReady      BSP_I2C1_IsReady;
#define BSP_NFCTAG_I2C_ReadReg16    BSP_I2C1_ReadReg16;
#define BSP_NFCTAG_I2C_Recv         BSP_I2C1_Recv;
#define BSP_NFCTAG_I2C_WriteReg16   BSP_I2C1_WriteReg16;
#define BSP_NFCTAG_GetTick          HAL_GetTick;

#define BSP_NFCTAG_BUS_HANDLE       (hbusi2c1)
#define BSP_NFCTAG_INSTANCE         (0)

#define BSP_NFCTAG_GPO_PRIORITY     (0)
#define BSP_NFCTAG_GPO_EXTI_LINE    (EXTI_LINE_6)
/**
  * @}
  */


#endif /* _NFC04A1_CONF_H_ */
