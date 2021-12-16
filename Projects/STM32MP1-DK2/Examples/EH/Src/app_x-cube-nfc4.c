/**
  ******************************************************************************
  * File Name          :  stmicroelectronics_x-cube-nfc4_2_0_3.c
  * Description        : This file provides code for the configuration
  *                      of the STMicroelectronics.X-CUBE-NFC4.2.0.3 instances.
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

/* Includes ------------------------------------------------------------------*/

#include "main.h"

/* Includes ------------------------------------------------------------------*/
#include "app_x-cube-nfc4.h"
#include "nfc04a1_nfctag.h"
#include "stm32mp15xx_disco.h"

/** @defgroup ST25_Nucleo
  * @{
  */

/** @defgroup Main
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Application defined push button, to detect long push */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
uint32_t Button_TimeCount;
/* Private functions ---------------------------------------------------------*/
void MX_NFC4_EH_Init(void);
void MX_NFC4_EH_Process(void);

void MX_NFC_Init(void)
{
  /* USER CODE BEGIN SV */ 

  /* USER CODE END SV */

  /* USER CODE BEGIN NFC4_Library_Init_PreTreatment */
  
  /* USER CODE END NFC4_Library_Init_PreTreatment */

  /* Initialize the peripherals and the NFC4 components */

  MX_NFC4_EH_Init();
  
  

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

  MX_NFC4_EH_Process();
  
  

  /* USER CODE END NFC4_Library_Process */
}

  /**
  * @brief  Initialize the EnergyHarvesting Example
  * @retval None
  */

void MX_NFC4_EH_Init(void)
{
	
  ST25DV_EN_STATUS value_eh_dyn;
  ST25DV_EH_MODE_STATUS value_eh_mode;
  
  /* Configuration of X-NUCLEO-NFC04A1                                          */
  /******************************************************************************/
  /* Init of the Leds on X-NUCLEO-NFC04A1 board */
  NFC04A1_LED_Init(GREEN_LED );
  NFC04A1_LED_Init(BLUE_LED );
  NFC04A1_LED_Init(YELLOW_LED );
  NFC04A1_LED_On( GREEN_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_On( BLUE_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_On( YELLOW_LED );
  HAL_Delay( 300 );
  
  /* Init ST25DV driver */
  while( NFC04A1_NFCTAG_Init(NFC04A1_NFCTAG_INSTANCE) != NFCTAG_OK );
  
   /* Init User button to allow user to change EH configuration */
  BSP_PB_Init( BUTTON_USER, BUTTON_MODE_GPIO );

  /* Init done */
  NFC04A1_LED_Off( GREEN_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_Off( BLUE_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_Off( YELLOW_LED );
  HAL_Delay( 300 );
  
  /* Check status of Energy Harvesting static configuration */
  NFC04A1_NFCTAG_ReadEHMode(NFC04A1_NFCTAG_INSTANCE, &value_eh_mode );
  if( value_eh_mode == ST25DV_EH_ACTIVE_AFTER_BOOT )
  {
    /* Energy Harvesting is set after each boot, LED 1 is on */
    NFC04A1_LED_On( GREEN_LED );
  }
  else
  {
    /* Energy Harvesting is reset after each boot, LED 1 is off */
    NFC04A1_LED_Off( GREEN_LED );
  }
  
  /* Check status of Energy Harvesting static configuration */
  NFC04A1_NFCTAG_GetEHENMode_Dyn(NFC04A1_NFCTAG_INSTANCE, &value_eh_dyn );
  if( value_eh_dyn == ST25DV_DISABLE )
  {
    /* Energy Harvesting is disabled, LED 2 is on */
    NFC04A1_LED_Off( BLUE_LED );
  }
  else
  {
    /* Energy Harvesting is activated, LED 2 is on */
    NFC04A1_LED_On( BLUE_LED );
  }
  
}

/**
  * @brief  Process of the NFC4 EH application
  * @retval None
  */

void MX_NFC4_EH_Process(void)
{
  ST25DV_PASSWD passwd;
  ST25DV_EN_STATUS value_eh_dyn;
  ST25DV_EH_MODE_STATUS value_eh_mode;
  ST25DV_I2CSSO_STATUS i2csso;
  uint32_t tick_start;
  uint32_t tick_end;

  /*Detect button press*/

  if(BSP_PB_GetState( BUTTON_USER ) == GPIO_PIN_RESET)
  {
    tick_start = HAL_GetTick();
    /* Debouncing */
    HAL_Delay(50);

    do
    {
      tick_end = HAL_GetTick();
    } while ((BSP_PB_GetState( BUTTON_USER ) == GPIO_PIN_RESET) && (tick_end - tick_start) <= 1000);

	if((tick_end - tick_start) > 1000)
    {
      /* Long Button press */
      /* Wait until the button is released */
      while ((BSP_PB_GetState( BUTTON_USER ) == GPIO_PIN_RESET));

      /* Debouncing */
      HAL_Delay(50);

      /* When user button is released after 1s, toggle Energy Harvesting static configuration */

      /* You need to present password to change static configuration */
      NFC04A1_NFCTAG_ReadI2CSecuritySession_Dyn(NFC04A1_NFCTAG_INSTANCE, &i2csso );
      if( i2csso == ST25DV_SESSION_CLOSED )
      {
        /* if I2C session is closed, present password to open session */
        passwd.MsbPasswd = 0;
        passwd.LsbPasswd = 0;
        NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd );
      }
    
      /* Toggle static configuration */
      NFC04A1_NFCTAG_ReadEHMode(NFC04A1_NFCTAG_INSTANCE, &value_eh_mode );
      if( value_eh_mode == ST25DV_EH_ACTIVE_AFTER_BOOT )
      {
        /* Let the user activate energy harvesting */
        value_eh_mode = ST25DV_EH_ON_DEMAND;
        NFC04A1_NFCTAG_WriteEHMode(NFC04A1_NFCTAG_INSTANCE, value_eh_mode );
        NFC04A1_LED_Off( GREEN_LED );
      }
      else
      {
        /* Energy harvesting activated after Power On Reset */
        value_eh_mode = ST25DV_EH_ACTIVE_AFTER_BOOT;
        NFC04A1_NFCTAG_WriteEHMode(NFC04A1_NFCTAG_INSTANCE, value_eh_mode );
        NFC04A1_LED_On( GREEN_LED );
      }
    }
    else
    {
      /* Short Button press */

      /* Button already released */
	  
      /* Debouncing */
      HAL_Delay(50);

      /* When user button is pressed, toggle Energy Harvesting dynamic configuration */
      NFC04A1_NFCTAG_GetEHENMode_Dyn(NFC04A1_NFCTAG_INSTANCE, &value_eh_dyn );
      if( value_eh_dyn == ST25DV_DISABLE )
      {
        /* Enable energy harvesting */
        NFC04A1_NFCTAG_SetEHENMode_Dyn(NFC04A1_NFCTAG_INSTANCE);
        NFC04A1_LED_On( BLUE_LED );
      }
      else
      {
        /* Disable energy harvesting */
        NFC04A1_NFCTAG_ResetEHENMode_Dyn(NFC04A1_NFCTAG_INSTANCE);
        NFC04A1_LED_Off( BLUE_LED );
      }
    }
  }
}

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
