/**
  ******************************************************************************
  * @file    nfc04a1.h 
  * @author  MMY-SRA Team
  * @brief   Header file to declare function for interfacing X-NUCLEO-NFC04A1 and ST25DV 
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __X_NUCLEO_NFC04A1_H
#define __X_NUCLEO_NFC04A1_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "nfc04a1_conf.h"
 
#include "st25dv.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup X_NUCLEO_NFC04A1
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup X_NUCLEO_NFC04A1_Exported_Types
  * @{
  */
/**
 * @brief  NFC04A1 Led enumerator definition
 */
typedef enum 
{
  GREEN_LED = 0,
  BLUE_LED,
  YELLOW_LED
}NFC04A1_Led_E;

/**
 * @brief  NFC04A1 Ack Nack enumerator definition
 */
typedef enum 
{
  I2CANSW_ACK = 0,
  I2CANSW_NACK
}NFC04A1_I2CANSW_E;

/**
 * @brief  NFC04A1 Led structure definition
 */
typedef struct
{
  uint16_t          NFC04A1_LED_PIN;
  GPIO_TypeDef *    NFC04A1_LED_PIN_PORT;
}NFC04A1_Led_TypeDef;

/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/
#define NFC04A1_INIT_CLK_GREEN_LED()        NFC04A1_INIT_CLK_LED1_RFD( );
#define NFC04A1_INIT_CLK_BLUE_LED()         NFC04A1_INIT_CLK_LED2_RFD( );
#define NFC04A1_INIT_CLK_YELLOW_LED()       NFC04A1_INIT_CLK_LED3_RFD( );


/* External variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/** @defgroup ST25DV_NUCLEO_Exported_Functions
  * @{
  */
int32_t NFC04A1_LED_Init(  NFC04A1_Led_E led );
int32_t NFC04A1_LED_DeInit( NFC04A1_Led_E led );
int32_t NFC04A1_LED_On( const NFC04A1_Led_E led );
int32_t NFC04A1_LED_Off( const NFC04A1_Led_E led );
int32_t NFC04A1_LED_Toggle( const NFC04A1_Led_E led );
int32_t NFC04A1_GPO_Init( void );
int32_t NFC04A1_GPO_DeInit( void );
int32_t NFC04A1_GPO_ReadPin( void );
int32_t NFC04A1_LPD_Init( void );
int32_t NFC04A1_LPD_DeInit( void );
int32_t NFC04A1_LPD_ReadPin( void );
int32_t NFC04A1_LPD_On( void );
int32_t NFC04A1_LPD_Off( void );
int32_t NFC04A1_LPD_Toggle( void );
void NFC04A1_GPO_IRQHandler(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 

#ifdef __cplusplus
  }
#endif
#endif /* __X_NUCLEO_NFC04A1_H */

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/
