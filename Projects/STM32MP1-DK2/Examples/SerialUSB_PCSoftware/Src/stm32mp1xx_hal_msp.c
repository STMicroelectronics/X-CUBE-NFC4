/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    OpenAMP/OpenAMP_TTY_raw/Src/stm32mp1xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hi2c->Instance==I2C5)
  {
  /* USER CODE BEGIN I2C5_MspInit 0 */
    if(IS_ENGINEERING_BOOT_MODE())
    {
      RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;

      /*##-1- Configure the I2C clock source #*/
      RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C35;
      RCC_PeriphCLKInitStruct.I2c35ClockSelection = RCC_I2C35CLKSOURCE_HSI;
      HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);
    }
  /* USER CODE END I2C5_MspInit 0 */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /**I2C5 GPIO Configuration
    PA11     ------> I2C5_SCL
    PA12     ------> I2C5_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C5;
    PERIPH_LOCK(GPIOA);
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    PERIPH_UNLOCK(GPIOA);
    /* Peripheral clock enable */
    __HAL_RCC_I2C5_CLK_ENABLE();
  /* USER CODE BEGIN I2C5_MspInit 1 */

  /* USER CODE END I2C5_MspInit 1 */
  }

}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{

  if(hi2c->Instance==I2C5)
  {
  /* USER CODE BEGIN I2C5_MspDeInit 0 */

  /* USER CODE END I2C5_MspDeInit 0 */

  /* Peripheral clock disable */
  __HAL_RCC_I2C5_CLK_DISABLE();


    /**I2C5 GPIO Configuration
    PA11     ------> I2C5_SCL
    PA12     ------> I2C5_SDA
    */
    PERIPH_LOCK(GPIOA);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);
    PERIPH_UNLOCK(GPIOA);

    /* I2C5 interrupt DeInit */
    HAL_NVIC_DisableIRQ(I2C5_EV_IRQn);
    HAL_NVIC_DisableIRQ(I2C5_ER_IRQn);
  /* USER CODE BEGIN I2C5_MspDeInit 1 */

  /* USER CODE END I2C5_MspDeInit 1 */
  }

}

/**
* @brief IPCC MSP Initialization
* This function configures the hardware resources used in this example
* @param hipcc: IPCC handle pointer
* @retval None
*/
void HAL_IPCC_MspInit(IPCC_HandleTypeDef* hipcc)
{

  if(hipcc->Instance==IPCC)
  {
  /* USER CODE BEGIN IPCC_MspInit 0 */

  /* USER CODE END IPCC_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_IPCC_CLK_ENABLE();
  /* IPCC interrupt Init */
    HAL_NVIC_SetPriority(IPCC_RX1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(IPCC_RX1_IRQn);
  /* USER CODE BEGIN IPCC_MspInit 1 */

  /* USER CODE END IPCC_MspInit 1 */
}

}

/**
* @brief IPCC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hipcc: IPCC handle pointer
* @retval None
*/

void HAL_IPCC_MspDeInit(IPCC_HandleTypeDef* hipcc)
{

  if(hipcc->Instance==IPCC)
  {
  /* USER CODE BEGIN IPCC_MspDeInit 0 */

  /* USER CODE END IPCC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_IPCC_CLK_DISABLE();

    /* IPCC interrupt DeInit */
    HAL_NVIC_DisableIRQ(IPCC_RX1_IRQn);
  /* USER CODE BEGIN IPCC_MspDeInit 1 */

  /* USER CODE END IPCC_MspDeInit 1 */
  }

}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
