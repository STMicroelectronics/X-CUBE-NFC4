
/**
  ******************************************************************************
  * File Name          :  app_x-cube-nfc4.c
  * Description        : This file provides code for the configuration
  *                      of the STMicroelectronics.X-CUBE-NFC4.2.0.5 instances.
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
extern UART_HandleTypeDef hcom_uart[COMn];
uint8_t awritedata[16];
uint8_t areaddata[16];
uint8_t cnt = 0;
uint16_t mblength;
ST25DV_MB_CTRL_DYN_STATUS mbctrldynstatus;
ST25DV_EN_STATUS MB_mode;
ST25DV_PASSWD passwd;
ST25DV_I2CSSO_STATUS i2csso;
UART_HandleTypeDef huart;
/* Private functions ---------------------------------------------------------*/
void MX_NFC4_MAILBOX_Init(void);
void MX_NFC4_MAILBOX_Process(void);

void MX_NFC_Init(void)
{
  /* USER CODE BEGIN SV */

  /* USER CODE END SV */

  /* USER CODE BEGIN NFC4_Library_Init_PreTreatment */

  /* USER CODE END NFC4_Library_Init_PreTreatment */

  /* Initialize the peripherals and the NFC4 components */

  MX_NFC4_MAILBOX_Init();

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

  MX_NFC4_MAILBOX_Process();

}

/**
  * @brief  Initialize the MAILBOX  feature Example
  * @retval None
  */
void MX_NFC4_MAILBOX_Init(void)
{
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
  printf( "\n\r----------------------------------------" );

  /* Init ST25DV driver */
  while( NFC04A1_NFCTAG_Init(NFC04A1_NFCTAG_INSTANCE) != NFCTAG_OK );

  /* Init done */
  NFC04A1_LED_Off( GREEN_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_Off( BLUE_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_Off( YELLOW_LED );
  HAL_Delay( 300 );

  printf( "\n\r\n\r This program will show you basic example on how to use the mailbox on I2C side" );

  /* If not activated, activate Mailbox, as long as MB is ON EEPROM is not available */
  NFC04A1_NFCTAG_ReadMBMode(NFC04A1_NFCTAG_INSTANCE, &MB_mode);
  if( MB_mode == ST25DV_DISABLE )
  {
    /* You need to present password before changing static configuration */
    NFC04A1_NFCTAG_ReadI2CSecuritySession_Dyn(NFC04A1_NFCTAG_INSTANCE, &i2csso );
    if( i2csso == ST25DV_SESSION_CLOSED )
    {
      /* if I2C session is closed, present password to open session */
      passwd.MsbPasswd = 0; /* Default value for password */
      passwd.LsbPasswd = 0; /* change it if password has been modified */
      NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd );
    }
   NFC04A1_NFCTAG_WriteMBMode(NFC04A1_NFCTAG_INSTANCE,ST25DV_ENABLE);

    /* Close session as dynamic register doesn't need open session for modification */
    passwd.MsbPasswd = 123;
    passwd.LsbPasswd = 123;
	NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd );
  }

  /* Enable Mailbox in dynamique register */
  NFC04A1_NFCTAG_SetMBEN_Dyn(NFC04A1_NFCTAG_INSTANCE);

  printf( "\n\r\n\rMailbox is activated" );

  /* prepare data to write */
  for( cnt = 0; cnt < 16; cnt++ )
  {
    awritedata[cnt] = cnt;
  }

  /* Check if Mailbox is available */
  NFC04A1_NFCTAG_ReadMBCtrl_Dyn( NFC04A1_NFCTAG_INSTANCE,&mbctrldynstatus );

  /* If MB is available, write data */
  if( (mbctrldynstatus.HostPutMsg == 0) && (mbctrldynstatus.RfPutMsg == 0) )
  {
    printf( "\n\r\n\rMailbox available, write data to it" );
    NFC04A1_NFCTAG_WriteMailboxData( NFC04A1_NFCTAG_INSTANCE, awritedata, 16 );
  }

  /* Read length of message */
  NFC04A1_NFCTAG_ReadMBLength_Dyn(NFC04A1_NFCTAG_INSTANCE, (uint8_t *)&mblength );
  printf( "\n\r\n\rLength of data write in MBCTRL register : %d", mblength );
  mblength++;
  printf( "\n\rNeed to add 1 to the length as it start at 0 : %d", mblength );
  /* Read mailbox status */
  NFC04A1_NFCTAG_ReadMBCtrl_Dyn( NFC04A1_NFCTAG_INSTANCE,&mbctrldynstatus );
  printf( "\n\r\n\rCtrl MB status register value:" );
  printf( "\n\rHost(i2c) Missed Message  = %d", mbctrldynstatus.HostMissMsg );
  printf( "\n\rRF(reader) Missed Message = %d", mbctrldynstatus.RFMissMsg );
  printf( "\n\rHost(i2c) Put Message     = %d", mbctrldynstatus.HostPutMsg );
  printf( "\n\rRF(reader) Put Message    = %d", mbctrldynstatus.RfPutMsg );
  printf( "\n\rMailbox Enable            = %d", mbctrldynstatus.MbEnable );

  /* Read all data in Mailbox */
  NFC04A1_NFCTAG_ReadMailboxData( NFC04A1_NFCTAG_INSTANCE,areaddata, 0, mblength );
  printf( "\n\r\n\rData read in Mailbox:\n\r" );
  /* prepare data to write */
  for( cnt = 0; cnt < 16; cnt++ )
  {
    printf( "0x%02X ", areaddata[cnt] );
  }

  NFC04A1_NFCTAG_ResetMBEN_Dyn(NFC04A1_NFCTAG_INSTANCE);
  NFC04A1_NFCTAG_SetMBEN_Dyn(NFC04A1_NFCTAG_INSTANCE);

  /* Read all data in Mailbox */
  NFC04A1_NFCTAG_ReadMailboxData(NFC04A1_NFCTAG_INSTANCE, areaddata, 0, mblength );
  printf( "\n\r\n\rClear mailbox by de/reactivate it or by reading it with a reader, read data:\n\r" );
  /* prepare data to write */
  for( cnt = 0; cnt < 16; cnt++ )
  {
    printf( "0x%02X ", areaddata[cnt] );
  }

  printf( "\n\r\n\r----------------------------------------" );
  printf( "\n\r*****        end of example        *****" );
  printf( "\n\r----------------------------------------\n\r\n\r" );

}

/**
* @brief  Process of the MAILBOX application
* @retval None
*/
void MX_NFC4_MAILBOX_Process(void)
{

}

#ifdef __cplusplus
}
#endif

