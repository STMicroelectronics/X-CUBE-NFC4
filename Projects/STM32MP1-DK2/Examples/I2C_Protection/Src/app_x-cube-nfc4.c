/**
  ******************************************************************************
  * File Name          :  stmicroelectronics_x-cube-nfc4_1_3_0.c
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

/* Includes ------------------------------------------------------------------*/

#include "main.h"

/* Includes ------------------------------------------------------------------*/
#include "app_x-cube-nfc4.h"
#include "nfc04a1_nfctag.h"
#include "stm32mp15xx_disco.h"
#include <stdio.h>
#include <string.h>	

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
char uartmsg[80];
uint8_t writedata = 0xBB;
uint8_t readdata = 0x0;
uint8_t cnt = 0;
uint32_t st25dvbmsize = 0;
uint32_t memindex = 0;
ST25DV_PASSWD passwd;
ST25DV_I2CSSO_STATUS i2csso;
ST25DV_MEM_SIZE st25dvmemsize;
uint32_t ret;

/* Private functions ---------------------------------------------------------*/
void MX_NFC4_I2CProtection_Init(void);
void MX_NFC4_I2CProtection_Process(void);


void MX_NFC_Init(void)
{
  /* USER CODE BEGIN SV */ 

  /* USER CODE END SV */

  /* USER CODE BEGIN NFC4_Library_Init_PreTreatment */
  
  /* USER CODE END NFC4_Library_Init_PreTreatment */

  /* Initialize the peripherals and the NFC4 components */

  MX_NFC4_I2CProtection_Init();

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

  MX_NFC4_I2CProtection_Process();
  
  
  
  /* USER CODE END NFC4_Library_Process */
}

/**
  * @brief  Initialize the I2CProtection  feature Example
  * @retval None
  */
void MX_NFC4_I2CProtection_Init(void)
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
  
 /* Init ST25DV driver */
  while( NFC04A1_NFCTAG_Init(NFC04A1_NFCTAG_INSTANCE) != NFCTAG_OK );
  
  /* Init done */
  NFC04A1_LED_Off( GREEN_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_Off( BLUE_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_Off( YELLOW_LED );
  HAL_Delay( 300 );

  
  printf( "----------------------------------------" );
  printf( "\n\r*****Welcome to x-cube-nfc4 example*****" );
  printf( "\n\r----------------------------------------" );
	
	/* Reset Mailbox enable to allow write to EEPROM */
  NFC04A1_NFCTAG_ResetMBEN_Dyn(NFC04A1_NFCTAG_INSTANCE);
	
	/* You need to present password to change static configuration */
  NFC04A1_NFCTAG_ReadI2CSecuritySession_Dyn(NFC04A1_NFCTAG_INSTANCE, &i2csso);
	
  if( i2csso == ST25DV_SESSION_CLOSED )
	{
     /* if I2C session is closed, present password to open session */
     passwd.MsbPasswd = 0;
     passwd.LsbPasswd = 0;
     NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd);
	}
	
   /* Get ST25DV EEPROM size */
   NFC04A1_NFCTAG_ReadMemSize(NFC04A1_NFCTAG_INSTANCE, &st25dvmemsize);
  
  /* st25dvmemsize is composed of Mem_Size (number of blocks) and BlockSize (size of each blocks in bytes) */
  st25dvbmsize = (st25dvmemsize.Mem_Size + 1) * (st25dvmemsize.BlockSize + 1);

  printf( "\n\r\n\rThis example will create 4 zones in the ST25DV,\n\rset different protection for each zone and test read/write on it" );
  /* Set mapping configuration on ST25DV, create 4 identical zone 
  Mapping example for a STDV64:
                 _______________
           @0000|               |
                |               |
                |     Zone 1    |
                |               |
           @2048|---------------|
                |               |
                |     Zone 2    |
                |               |
           @4096|---------------|
                |               |
                |     Zone 3    |
                |               |
           @6144|---------------|
                |               |
                |     Zone 4    |
                |               |
           @8192|_______________|
  */
	
   NFC04A1_NFCTAG_CreateUserZone(NFC04A1_NFCTAG_INSTANCE, (st25dvbmsize / 4), (st25dvbmsize / 4), (st25dvbmsize / 4), (st25dvbmsize / 4 ));
	
   printf( "\n\rZone 1 is not protected" );
   /*Unprotect zone 1*/
    NFC04A1_NFCTAG_WriteI2CProtectZonex(NFC04A1_NFCTAG_INSTANCE, ST25DV_PROT_ZONE1,  ST25DV_NO_PROT );
	
    printf( "\n\rZone 2 is write protected" );
   /*Protect zone 2 for i2c write without password */
	NFC04A1_NFCTAG_WriteI2CProtectZonex(NFC04A1_NFCTAG_INSTANCE, ST25DV_PROT_ZONE2,  ST25DV_WRITE_PROT );
	
	printf( "\n\rZone 3 is read protected" );
   /* Protect zone 3 for i2c read without password */
	NFC04A1_NFCTAG_WriteI2CProtectZonex(NFC04A1_NFCTAG_INSTANCE, ST25DV_PROT_ZONE3,  ST25DV_READ_PROT );
	
	printf( "\n\rZone 4 is read/write protected" );
   /* Protect zone 4 for i2c read and write without password */
   NFC04A1_NFCTAG_WriteI2CProtectZonex(NFC04A1_NFCTAG_INSTANCE, ST25DV_PROT_ZONE4,  ST25DV_READWRITE_PROT );
	
   printf( "\n\r\n\rPress a key to continue..." );
   scanf("%hhu", &readdata );
	
   printf( "\n\r\n\rSession is closed to see protections in operation" );

   /* Session is open, present wrong password to closed it */
   passwd.MsbPasswd = 123;
   passwd.LsbPasswd = 123;
   NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd);

   for( cnt = 1; cnt < 5; cnt++ )
   {
    /* Compute memory index for each zone */
    memindex = (((st25dvbmsize / 4) + 1) * (cnt - 1));
    
    sprintf( uartmsg, "\n\r\n\rStart writing Zone %d", cnt );
    printf( uartmsg );
    ret = NFC04A1_NFCTAG_WriteData(NFC04A1_NFCTAG_INSTANCE, &writedata, memindex, 1 );
    if( ret != NFCTAG_OK )
    {
      sprintf( uartmsg, "\n\rWrite Zone %d protected, need i2c password", cnt );
      printf( uartmsg );
    }
    else
    {
      sprintf( uartmsg, "\n\rWrite Zone %d done", cnt );
      printf( uartmsg );
    }
    
    sprintf( uartmsg, "\n\r\n\rStart reading Zone %d", cnt );
    printf( uartmsg );
    ret = NFC04A1_NFCTAG_ReadData(NFC04A1_NFCTAG_INSTANCE, &readdata, memindex, 1 );
    if( readdata == 0xFF )
    {
      sprintf( uartmsg, "\n\rRead Zone %d protected, need i2c password", cnt );
      printf( uartmsg );
    }
    else
    {
      sprintf( uartmsg, "\n\rRead Zone %d done", cnt );
      printf( uartmsg );
    }
  }
  
   printf( "\n\r\n\rPress a ke-y to continue..." );
  scanf("%hhu", &readdata );
  
  printf( "\n\r\n\rSession is open to see granted access" );
  /* Session is closed, present password to open it */
  passwd.MsbPasswd = 0;
  passwd.LsbPasswd = 0;
  NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd);
  
  for( cnt = 1; cnt < 5; cnt++ )
  {
    /* Compute memory index for each zone */
    memindex = (((st25dvbmsize / 4) + 1) * (cnt - 1));
    
    sprintf( uartmsg, "\n\r\n\rStart writing Zone %d", cnt );
    printf( uartmsg );
    ret = NFC04A1_NFCTAG_WriteData(NFC04A1_NFCTAG_INSTANCE, &writedata, memindex, 1 );
    if( ret != NFCTAG_OK )
    {
      sprintf( uartmsg, "\n\rWrite Zone %d protected, need i2c password", cnt );
      printf( uartmsg );
    }
    else
    {
      sprintf( uartmsg, "\n\rWrite Zone %d done", cnt );
      printf( uartmsg );
    }
    
    sprintf( uartmsg, "\n\r\n\rStart reading Zone %d", cnt );
    printf( uartmsg );
    ret = NFC04A1_NFCTAG_ReadData(NFC04A1_NFCTAG_INSTANCE, &readdata, memindex, 1 );
    if( readdata == 0xFF )
    {
      sprintf( uartmsg, "\n\rRead Zone %d protected, need i2c password", cnt );
      printf( uartmsg );
    }
    else
    {
      sprintf( uartmsg, "\n\rRead Zone %d done", cnt );
      printf( uartmsg );
    }
  }
  
  printf( "\n\r\n\r----------------------------------------" );
  printf( "\n\r*****        end of example        *****" );
  printf( "\n\r----------------------------------------\n\r" );
}

  /**
  * @brief  Process of the I2CProtection application
  * @retval None
  */
void MX_NFC4_I2CProtection_Process(void)
{
 
}



#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
