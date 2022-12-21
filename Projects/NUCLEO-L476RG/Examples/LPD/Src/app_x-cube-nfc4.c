
/**
  ******************************************************************************
  * File Name          :  app_x-cube-nfc4.c
  * Description        : This file provides code for the configuration
  *                      of the STMicroelectronics.X-CUBE-NFC4.3.0.0 instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "app_x-cube-nfc4.h"
#include "main.h"

/* Includes ------------------------------------------------------------------*/
#include "nfc04a1_nfctag.h"
#include "stm32l4xx_nucleo.h"
#include <stdio.h>

/** @defgroup ST25_Nucleo
  * @{
  */

/** @defgroup Main
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
uint8_t nfctagid = 0;
int32_t status;
static volatile uint8_t UserChange = 0;

 /* Private functions ---------------------------------------------------------*/
void MX_NFC4_LPD_Init(void);
void MX_NFC4_LPD_Process(void);
void BSP_PB_Callback(Button_TypeDef Button);

void MX_NFC_Init(void)
{
  /* USER CODE BEGIN SV */

  /* USER CODE END SV */

  /* USER CODE BEGIN NFC4_Library_Init_PreTreatment */

  /* USER CODE END NFC4_Library_Init_PreTreatment */

  /* Initialize the peripherals and the NFC4 components */

  MX_NFC4_LPD_Init();

  /* USER CODE BEGIN SV */

  /* USER CODE END SV */

  /* USER CODE BEGIN NFC4_Library_Init_PostTreatment */

  /* USER CODE END NFC4_Library_Init_PostTreatment */
}
/*
 * LM background task
 */
void MX_NFC_Process(void)
{
  /* USER CODE BEGIN NFC4_Library_Process */

  /* USER CODE END NFC4_Library_Process */

  MX_NFC4_LPD_Process();

}

/**
  * @brief  Initialize the LPD feature Example
  * @retval None
  */
void MX_NFC4_LPD_Init(void)
{

  /* Configuration of X-NUCLEO-NFC04A1                                          */
  /******************************************************************************/
  /* Init of the Leds on X-NUCLEO-NFC04A1 board */

  NFC04A1_LED_Init(GREEN_LED);
  NFC04A1_LED_Init(BLUE_LED);
  NFC04A1_LED_Init(YELLOW_LED);

  NFC04A1_LED_On( GREEN_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_On( BLUE_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_On( YELLOW_LED );
  HAL_Delay( 300 );

  /* Init UART for display message on console */
  BSP_COM_Init(COM1);

  printf( "----------------------------------------" );
  printf( "\n\r*****Welcome to x-cube-nfc4 example*****" );
  printf( "\n\r----------------------------------------\n\r" );

  /* Init the GPIO Pin of LPD */
  NFC04A1_LPD_Init();

 /* Init User button to allow user to change LPD configuration */
  BSP_PB_Init( BUTTON_KEY, BUTTON_MODE_EXTI );

 /* Init ST25DV driver */
  while( NFC04A1_NFCTAG_Init(NFC04A1_NFCTAG_INSTANCE) != NFCTAG_OK );

  NFC04A1_LED_Off( GREEN_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_Off( BLUE_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_Off( YELLOW_LED );
  HAL_Delay( 300 );

  if( NFC04A1_LPD_ReadPin( ) == GPIO_PIN_SET )
  {
    /* When LPD is activated, Green led is ON */
    NFC04A1_LED_On( GREEN_LED );
    printf("LPD is activated\n\r");
  }
  else
  {
    /* When LPD is deactivated, Green led is OFF */
    NFC04A1_LED_Off( GREEN_LED );
    printf("LPD is deactivated\n\r");
  }
}

/**
  * @brief  Process of the EnergyHarvesting application
  * @retval None
  */
void MX_NFC4_LPD_Process(void)
{
  if( UserChange == 1 )
  {
    UserChange = 0;
    /*Change the LPD GPIO state*/
    HAL_GPIO_TogglePin( NFC04A1_LPD_PIN_PORT, NFC04A1_LPD_PIN );
    NFC04A1_LED_Toggle( GREEN_LED );
    if( NFC04A1_LPD_ReadPin( ) == GPIO_PIN_SET )
    {
      printf("LPD is activated\n\r");
    }
    else
    {
      printf("LPD is deactivated\n\r");
    }
    HAL_Delay( 50 );
    status = NFC04A1_NFCTAG_ReadID(NFC04A1_NFCTAG_INSTANCE, &nfctagid );

    if(status == NFCTAG_OK && (nfctagid != 0))
    {
        nfctagid = 0;
        NFC04A1_LED_On( YELLOW_LED );
    }
    else
    {
    NFC04A1_LED_Off( YELLOW_LED );
    }
  }
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void BSP_PB_Callback(Button_TypeDef Button)
{
  if( Button == BUTTON_KEY )
  {
    UserChange = 1;
  }
}

#ifdef __cplusplus
}
#endif

