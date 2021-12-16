
/**
  ******************************************************************************
  * File Name          :  app_x-cube-nfc4.c
  * Description        : This file provides code for the configuration
  *                      of the STMicroelectronics.X-CUBE-NFC4.2.0.4 instances.
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
#include "app_x-cube-nfc4.h"
#include "main.h"

/* Includes ------------------------------------------------------------------*/
#include "nfc04a1_nfctag.h"
#include "stm32f4xx_nucleo.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/** @defgroup ST25_Nucleo
  * @{
  */

/** @defgroup Main
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TOSTRING(s) #s
#define STRINGIZE(s) TOSTRING(s)

#define MCU_FW_VERSION_MAJOR              1
#define MCU_FW_VERSION_MINOR              0
#define MCU_FW_VERSION_REVISION           0
#define MCU_FW_VERSION                    STRINGIZE(MCU_FW_VERSION_MAJOR) "." \
                                          STRINGIZE(MCU_FW_VERSION_MINOR) "." \
                                          STRINGIZE(MCU_FW_VERSION_REVISION)

#define SERIAL_COM_HEADERSIZE         5
#define SERIAL_COM_FOOTERSIZE         1
#define SERIAL_COM_DESTADDR           0
#define SERIAL_COM_SRCADDR            1
#define SERIAL_COM_CMD                2
#define SERIAL_COM_LENGTH             3
#define SERIAL_COM_PAYLOAD            5

#define TMSG_EOF                0xF0
#define TMSG_BS                 0xF1
#define TMSG_BS_EOF             0xF2

#ifdef USE_USB_OTG_HS
#define TMSG_MAXLEN             512
#else
#define TMSG_MAXLEN             256
#endif

#define UART_RXBUFFERSIZE (2*TMSG_MAXLEN)

#define PC_SW_ADDRESS                      0x32
#define STM32_FW_ADDRESS                   0xB4

#define PCSW_BAUDRATE                      115200

#define SERIALCOM_FW_VERSION               MCU_FW_VERSION

#define ST25DV_SERIALCOM_FW_VERSION                   SERIALCOM_FW_VERSION

/*            ---- DOESN'T NEED STM32_FW_ADDRESS ---             */

/**********  GENERIC  CMD  (0x00 - 0x0F)  **********/
#define CMD_PING                                      0x01
#define CMD_CHECK_FIRMWAREVERSION                     0x02
#define CMD_GET_STM32_FW_ADDRESS                      0x03

/*            ---- NEED STM32_FW_ADDRESS ---                      */

/**********  ST25DV SYSTEM CMD  (0x10 - 0x2F)  **********/
#define CMD_ST25DV_INIT_DRIVER                        0x10
#define CMD_ST25DV_DEINIT_DRIVER                      0x11
#define CMD_ST25DV_IS_INIT                            0x12
#define CMD_ST25DV_I2C_SPEED                          0x13

/**********  ST25DV USER CMD  (0x30 - 0x5F)  **********/
#define CMD_ST25DV_READ                               0x30
#define CMD_ST25DV_WRITE                              0x31
#define CMD_ST25DV_READ_CONFIG                        0x32
#define CMD_ST25DV_WRITE_CONFIG                       0x33
#define CMD_ST25DV_PRESENT_PASSWORD                   0x34
#define CMD_ST25DV_WRITE_PASSWORD                     0x35
#define CMD_ST25DV_READ_GPO_PIN                       0x36
#define CMD_ST25DV_SET_RESET_LPD                      0x37

/******** ERROR MANAGEMENT  CMD  (0x60 - 0x6F)  ********/
#define CMD_Reply_PCAddrError                         0x60
#define CMD_Reply_STM32AddrError                      0x61
#define CMD_Reply_LengthError                         0x62
#define CMD_Reply_DeviceBusy                          0x63
#define CMD_Reply_WriteTimeout                        0x64
#define CMD_Reply_WriteNack                           0x65
#define CMD_Reply_CmdError                            0x66

/******** PROTOCOL COMMUNICATION  CMD  (0x80 - 0x8F)  ********/
#define CMD_Reply_OK                                  0x80

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
//UART_HandleTypeDef* HandleUart;

extern UART_HandleTypeDef hcom_uart[COMn];
volatile uint8_t UART_RxBuffer[UART_RXBUFFERSIZE];
volatile uint8_t UART_TxBuffer[TMSG_MAXLEN * 2];
/**
 * @brief  Serial message structure definition
 */
typedef struct
{
  uint32_t Len;
  uint8_t Data[2*TMSG_MAXLEN];
} TMsg;

/**
 * @brief  Error return enum definition
 */
typedef enum
{
  SERIALCOM_OK      = 0,
  SERIALCOM_ERROR   = 1
} SERIALCOM_StatusTypeDef;

/* Private function prototypes -------------------------------------------------------------*/
void MX_NFC4_SERIALUSB_Init(void);
void MX_NFC4_SERIALUSB_Process(void);
void SerialCom_SendMsg( TMsg * const Msg );
SERIALCOM_StatusTypeDef SerialCom_ReceivedMSG( TMsg * const Msg );
int ByteStuffCopyByte( uint8_t * const Dest, const uint8_t Source );
int ReverseByteStuffCopyByte2( const uint8_t Source0, const uint8_t Source1, uint8_t * const Dest );
int ByteStuffCopy( uint8_t * const Dest, const TMsg * const Source );
int ReverseByteStuffCopyByte( const uint8_t * const Source, uint8_t * const Dest );
int ReverseByteStuffCopy( TMsg * const Dest, const uint8_t *Source );
void CHK_ComputeAndAdd( TMsg * const Msg );
int CHK_CheckAndRemove( TMsg * const Msg );
uint32_t Deserialize( const uint8_t * const Source, uint32_t Len );
int32_t Deserialize_s32( const uint8_t * const Source, uint32_t Len );
int32_t Deserialize_s32_MSBfirst( const uint8_t * const Source, const uint32_t Len );
void Serialize( uint8_t * const Dest, uint32_t Source, const uint32_t Len );
void Serialize_s32( uint8_t * const Dest, int32_t Source, const uint32_t Len );
void Serialize_s32_MSBfirst( uint8_t * const Dest, int32_t Source, const uint32_t Len );

uint32_t SerialCom_GetDMACounter( void );
uint8_t SerialCom_GetReceivedBufferByte( const uint16_t startmsg, const uint16_t offset );
void SerialComUART_SendMsg( TMsg * const Msg );
void Build_Reply_Header( TMsg * const Msg );
void Build_Reply_Header_Length_Error( TMsg * const Msg );
void Build_Reply_Header_Sequence_Error( TMsg * const Msg );
void Build_Reply_Header_PC_Dev_Addr_Error( TMsg * const Msg );
void Build_Reply_Header_STM32_Dev_Addr_Error( TMsg * const Msg );
void Build_Reply_Header_Cmd_Reply_DeviceBusy( TMsg * const Msg );
void Build_Reply_Header_Cmd_Reply_WriteTimeout( TMsg * const Msg );
void Build_Reply_Header_Cmd_Reply_WriteNack( TMsg * const Msg );
void Build_Reply_Header_Cmd_Error( TMsg * const Msg );

SERIALCOM_StatusTypeDef HandleMSG( TMsg * const Msg );

void MX_NFC_Init(void)
{
  /* USER CODE BEGIN SV */

  /* USER CODE END SV */

  /* USER CODE BEGIN NFC4_Library_Init_PreTreatment */

  /* USER CODE END NFC4_Library_Init_PreTreatment */

  /* Initialize the peripherals and the NFC4 components */

  MX_NFC4_SERIALUSB_Init();

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

  MX_NFC4_SERIALUSB_Process();

}

/**
  * @brief  Initialize the SERIALUSB_PCSoftware feature Example
  * @retval None
  */
void MX_NFC4_SERIALUSB_Init(void)
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

  while( NFC04A1_NFCTAG_Init(NFC04A1_NFCTAG_INSTANCE) != NFCTAG_OK );

  /* Init UART for display message on console */
  BSP_COM_Init(COM1);
  HAL_UART_Receive_DMA( &hcom_uart[COM1], (uint8_t *)UART_RxBuffer, UART_RXBUFFERSIZE );

  /* Init done */
  NFC04A1_LED_Off( GREEN_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_Off( BLUE_LED );
  HAL_Delay( 300 );
  NFC04A1_LED_Off( YELLOW_LED );

}

/**
  * @brief  Process the SERIALUSB_PCSoftware feature Example
  * @retval None
  */
void MX_NFC4_SERIALUSB_Process(void)
{
  TMsg Msg;
  if( SerialCom_ReceivedMSG( &Msg ) == SERIALCOM_OK )
    {
      NFC04A1_LED_On( BLUE_LED );
      HandleMSG( &Msg );
    }
    HAL_Delay(50);
    NFC04A1_LED_Off( BLUE_LED );
}

 /**
  * @brief  Get message from UART if end of message detected
  * @param  Msg: pointer to store the message received
  * @retval SERIALCOM_StatusTypeDef
  */
SERIALCOM_StatusTypeDef SerialCom_ReceivedMSG( TMsg * const Msg )
{
  static uint16_t DmaMsgIndex = 0;
  uint16_t cnt = 0;
  uint16_t dmacounter = 0;
  uint16_t sizecount = 0;
  uint16_t payloadsize = 0;

  /* Get the dma counter to detect new messages */
  dmacounter = UART_RXBUFFERSIZE - SerialCom_GetDMACounter( );

  if( dmacounter != DmaMsgIndex )
  {
    /* Extract 2 bytes payload size from frame */
    payloadsize = SerialCom_GetReceivedBufferByte( DmaMsgIndex, SERIAL_COM_LENGTH );
    payloadsize = payloadsize << 8;
    payloadsize |= SerialCom_GetReceivedBufferByte( DmaMsgIndex, SERIAL_COM_LENGTH + 1 );

    /* Detect if we are at end of buffer (DMA_CIRCULAR) */
    if( dmacounter > DmaMsgIndex )
    {
      /* Compute number of data to transfer to Msg */
      sizecount = ((dmacounter - 1) - DmaMsgIndex);
    }
    else
    {
      /* Compute number of data to transfer to Msg */
      sizecount = ((UART_RXBUFFERSIZE - DmaMsgIndex) + (dmacounter - 1));
    }

    /* New message in buffer available */
    if( (SERIAL_COM_HEADERSIZE + payloadsize + SERIAL_COM_FOOTERSIZE) == sizecount )
    {
      /* Message is complete terminate by TMSG_EOF */
      Msg->Len = 0;

      /* Start copying data to Msg structure */
      for( cnt = 0; cnt < sizecount; cnt++ )
      {
        /* Copy message to Msg buffer */
        Msg->Data[cnt] = SerialCom_GetReceivedBufferByte( DmaMsgIndex, cnt );
        Msg->Len++;
      }
      /* Update index on DMA buffer */
      DmaMsgIndex = dmacounter;

      /* Check Checksum and remove it from message */
      if( CHK_CheckAndRemove( Msg ) == 1 )
      {
        return SERIALCOM_OK;
      }
    }
  }
  return SERIALCOM_ERROR;
}

/**
  * @brief  Send a message via UART
  * @param  Msg: pointer to the message to send
  * @retval None
  */
void SerialCom_SendMsg( TMsg * const Msg )
{
  /* Add Checksum CHKSUM byte */
  CHK_ComputeAndAdd( Msg );

  SerialComUART_SendMsg( Msg );
}

/**
 * @brief  Byte stuffing process for one byte
 * @param  Dest destination
 * @param  Source source
 * @retval Total number of bytes processed
*/
int ByteStuffCopyByte( uint8_t * const Dest, const uint8_t Source )
{
  switch( Source )
  {
    case TMSG_EOF:
      Dest[0] = TMSG_BS;
      Dest[1] = TMSG_BS_EOF;
      return 2;
    case TMSG_BS:
      Dest[0] = TMSG_BS;
      Dest[1] = TMSG_BS;
      return 2;
    default:
      Dest[0] = Source;
      return 1;
  }
}

/**
 * @brief  Byte stuffing process for a Msg
 * @param  Dest destination
 * @param  Source source
 * @retval Total number of bytes processed
 */
int ByteStuffCopy( uint8_t * const Dest, const TMsg * const Source )
{
  int i, Count;

  Count = 0;
  for( i = 0; i < Source->Len; i++ )
  {
    Count += ByteStuffCopyByte( &Dest[Count], Source->Data[i] );
  }
  Dest[Count] = TMSG_EOF;
  Count++;
  return Count;
}

/**
 * @brief  Reverse Byte stuffing process for one byte
 * @param  Source source
 * @param  Dest destination
 * @retval Number of input bytes processed (1 or 2) or 0 for invalid sequence
 */
int ReverseByteStuffCopyByte( const uint8_t * const Source, uint8_t * const Dest )
{
  if( Source[0] == TMSG_BS )
  {
    if( Source[1] == TMSG_BS )
    {
      *Dest = TMSG_BS;
      return 2;
    }
    if( Source[1] == TMSG_BS_EOF )
    {
      *Dest = TMSG_EOF;
      return 2;
    }
    return 0; // invalid sequence
  }
  else
  {
    *Dest = Source[0];
    return 1;
  }
}

/**
 * @brief  Reverse Byte stuffing process for two input data
 * @param  Source0 input data
 * @param  Source1 input data
 * @param  Dest the destination data
 * @retval Number of input bytes processed (1 or 2) or 0 for invalid sequence
 */
int ReverseByteStuffCopyByte2( const uint8_t Source0, const uint8_t Source1, uint8_t * const Dest )
{
  if( Source0 == TMSG_BS )
  {
    if( Source1 == TMSG_BS )
    {
      *Dest = TMSG_BS;
      return 2;
    }
    if( Source1 == TMSG_BS_EOF )
    {
      *Dest = TMSG_EOF;
      return 2;
    }
    return 0; // invalid sequence
  }
  else
  {
    *Dest = Source0;
    return 1;
  }
}

/**
 * @brief  Reverse Byte stuffing process for a Msg
 * @param  Dest destination
 * @param  Source source
 * @retval 1 if the operation succeeds, 0 if an error occurs
 */
int ReverseByteStuffCopy( TMsg * const Dest, const uint8_t * Source )
{
  int Count = 0, State = 0;

  while( (*Source) != TMSG_EOF )
  {
    if( State == 0 )
    {
      if( (*Source) == TMSG_BS )
      {
        State = 1;
      }
      else
      {
        Dest->Data[Count] = *Source;
        Count++;
      }
    }
    else
    {
      if( (*Source) == TMSG_BS )
      {
        Dest->Data[Count] = TMSG_BS;
        Count++;
      }
      else
      {
        if( (*Source) == TMSG_BS_EOF )
        {
          Dest->Data[Count] = TMSG_EOF;
          Count++;
        }
        else
        {
          return 0; // invalid sequence
        }
      }
      State = 0;
    }
    Source++;
  }
  if( State != 0 )
  {
    return 0;
  }
  Dest->Len = Count;
  return 1;
}

/**
 * @brief  Compute and add checksum
 * @param  Msg pointer to the message
 * @retval None
 */
void CHK_ComputeAndAdd( TMsg * const Msg )
{
  uint8_t CHK = 0;
  int i;

  for( i = 0; i < Msg->Len; i++ )
  {
    CHK -= Msg->Data[i];
  }
  Msg->Data[i] = CHK;
  Msg->Len++;
}

/**
 * @brief  Compute and remove checksum
 * @param  Msg pointer to the message
 * @retval A number different from 0 if the operation succeeds, 0 if an error occurs
 */
int CHK_CheckAndRemove( TMsg * const Msg )
{
  uint8_t CHK = 0;
  int i;

  for( i = 0; i < Msg->Len; i++ )
  {
    CHK += Msg->Data[i];
  }
  Msg->Len--;
  return (CHK == 0);
}

/**
 * @brief  Build an array from the uint32_t (LSB first)
 * @param  Dest destination
 * @param  Source source
 * @param  Len number of bytes
 * @retval None
 */
void Serialize( uint8_t * const Dest, uint32_t Source, const uint32_t Len )
{
  int i;
  for( i = 0; i < Len; i++ )
  {
    Dest[i] = Source & 0xFF;
    Source >>= 8;
  }
}

/**
 * @brief  Unbuild a Number from an array (LSB first)
 * @param  Source source
 * @param  Len number of bytes
 * @retval Rebuild unsigned int variable
 */
uint32_t Deserialize( const uint8_t * const Source, uint32_t Len )
{
  uint32_t app;

  app = Source[--Len];

  while( Len > 0 )
  {
    app <<= 8;
    app += Source[--Len];
  }
  return app;
}

/**
 * @brief  Build an array from the uint32_t (LSB first)
 * @param  Dest destination
 * @param  Source source
 * @param  Len number of bytes
 * @retval None
 */
void Serialize_s32( uint8_t * const Dest, int32_t Source, const uint32_t Len )
{
  int i;
  for( i = 0; i < Len; i++ )
  {
    Dest[i] = Source & 0xFF;
    Source >>= 8;
  }
}

/**
 * @brief  Build an array from the uint32_t (MSB first)
 * @param  Dest destination
 * @param  Source source
 * @param  Len number of bytes
 * @retval None
 */
void Serialize_s32_MSBfirst( uint8_t * const Dest, int32_t Source, const uint32_t Len )
{
  int i;
  for( i = 0; i < Len; i++ )
  {
    Dest[Len-1-i] = Source & 0xFF;
    Source >>= 8;
  }
}

/**
 * @brief  Unbuild a Number from an array (LSB first)
 * @param  Source source
 * @param  Len number of bytes
 * @retval Rebuild signed int variable
 */
int32_t Deserialize_s32( const uint8_t * const Source, uint32_t Len )
{
  int32_t app;

  app = Source[--Len];

  while( Len > 0 )
  {
    app <<= 8;
    app += Source[--Len];
  }
  return app;
}

/**
 * @brief  Unbuild a Number from an array (LSB first)
 * @param  Source source
 * @param  Len number of bytes
 * @retval Rebuild signed int variable
 */
int32_t Deserialize_s32_MSBfirst( const uint8_t * const Source, const uint32_t Len )
{
  int i;
  int32_t app = 0;

	for( i = 0; i < Len; i++ )
  {
    app <<= 8;
	app += Source[i];
  }
  return app;
}

/**
 * @brief  Returns the number of remaining data units in the current DMAy Streamx transfer
 * @param  handle_dma DMA handle
 * @retval The number of remaining data units in the current DMA Stream transfer
 */
uint32_t SerialCom_GetDMACounter( void )
{
  return (__HAL_DMA_GET_COUNTER(hcom_uart[COM1].hdmarx));
}

uint8_t SerialCom_GetReceivedBufferByte( const uint16_t startmsg, const uint16_t offset )
{
  uint8_t ret_value = 0;

  if( (startmsg + offset) >= UART_RXBUFFERSIZE )
  {
    ret_value = UART_RxBuffer[offset - (UART_RXBUFFERSIZE - startmsg)];
  }
  else
  {
    ret_value = UART_RxBuffer[startmsg + offset];
  }

  return ret_value;
}

/**
  * @brief  Sends a message throught UART
  * @param  Msg the pointer to the message to be sent
  * @retval None
  */
void SerialComUART_SendMsg( TMsg * const Msg )
{
  memcpy( (void *)UART_TxBuffer, Msg->Data, Msg->Len );

  UART_TxBuffer[Msg->Len] = TMSG_EOF;
  Msg->Len++;

  HAL_UART_Transmit_DMA( &hcom_uart[COM1], (uint8_t *)UART_TxBuffer, Msg->Len );
}

/**
  * @brief  Build the reply header
  * @param  Msg: pointer to store the message to send
  * @retval None
  */
void Build_Reply_Header( TMsg * const Msg )
{
  Msg->Data[SERIAL_COM_DESTADDR] = STM32_FW_ADDRESS;
  Msg->Data[SERIAL_COM_SRCADDR] = PC_SW_ADDRESS;
  Msg->Data[SERIAL_COM_CMD] = CMD_Reply_OK;
  Msg->Data[SERIAL_COM_LENGTH] = 0;
  Msg->Data[SERIAL_COM_LENGTH + 1] = 0;
  Msg->Len = 5;
}

/**
  * @brief  Build the length error reply header
  * @param  Msg: pointer to store the message to send
  * @retval None
  */
void Build_Reply_Header_Length_Error( TMsg * const Msg )
{
  Msg->Data[SERIAL_COM_DESTADDR] = STM32_FW_ADDRESS;
  Msg->Data[SERIAL_COM_SRCADDR] = PC_SW_ADDRESS;
  Msg->Data[SERIAL_COM_CMD] = CMD_Reply_LengthError;
  Msg->Data[SERIAL_COM_LENGTH] = 0;
  Msg->Data[SERIAL_COM_LENGTH + 1] = 0;
  Msg->Len = 5;
}

/**
  * @brief  Build the sequence error reply header
  * @param  Msg: pointer to store the message to send
  * @retval None
  */
void Build_Reply_Header_Sequence_Error( TMsg * const Msg )
{
  Msg->Data[SERIAL_COM_DESTADDR] = STM32_FW_ADDRESS;
  Msg->Data[SERIAL_COM_SRCADDR] = PC_SW_ADDRESS;
  Msg->Data[SERIAL_COM_CMD] = CMD_Reply_LengthError;
  Msg->Data[SERIAL_COM_LENGTH] = 0;
  Msg->Data[SERIAL_COM_LENGTH + 1] = 0;
  Msg->Len = 5;
}

/**
  * @brief  Build the PC device address error reply header
  * @param  Msg: pointer to store the message to send
  * @retval None
  */
void Build_Reply_Header_PC_Dev_Addr_Error( TMsg * const Msg )
{
  Msg->Data[SERIAL_COM_DESTADDR] = STM32_FW_ADDRESS;
  Msg->Data[SERIAL_COM_SRCADDR] = PC_SW_ADDRESS;
  Msg->Data[SERIAL_COM_CMD] = CMD_Reply_PCAddrError;
  Msg->Data[SERIAL_COM_LENGTH] = 0;
  Msg->Data[SERIAL_COM_LENGTH + 1] = 0;
  Msg->Len = 5;
}

/**
  * @brief  Build the STM32 device address error reply header
  * @param  Msg: pointer to store the message to send
  * @retval None
  */
void Build_Reply_Header_STM32_Dev_Addr_Error( TMsg * const Msg )
{
  Msg->Data[SERIAL_COM_DESTADDR] = STM32_FW_ADDRESS;
  Msg->Data[SERIAL_COM_SRCADDR] = PC_SW_ADDRESS;
  Msg->Data[SERIAL_COM_CMD] = CMD_Reply_STM32AddrError;
  Msg->Data[SERIAL_COM_LENGTH] = 0;
  Msg->Data[SERIAL_COM_LENGTH + 1] = 0;
  Msg->Len = 5;
}

/**
  * @brief  Build the command device busy reply header
  * @param  Msg: pointer to store the message to send
  * @retval None
  */
void Build_Reply_Header_Cmd_Reply_DeviceBusy( TMsg * const Msg )
{
  Msg->Data[SERIAL_COM_DESTADDR] = STM32_FW_ADDRESS;
  Msg->Data[SERIAL_COM_SRCADDR] = PC_SW_ADDRESS;
  Msg->Data[SERIAL_COM_CMD] = CMD_Reply_DeviceBusy;
  Msg->Data[SERIAL_COM_LENGTH] = 0;
  Msg->Data[SERIAL_COM_LENGTH + 1] = 0;
  Msg->Len = 5;
}

/**
  * @brief  Build the command write timeout reply header
  * @param  Msg: pointer to store the message to send
  * @retval None
  */
void Build_Reply_Header_Cmd_Reply_WriteTimeout( TMsg * const Msg )
{
  Msg->Data[SERIAL_COM_DESTADDR] = STM32_FW_ADDRESS;
  Msg->Data[SERIAL_COM_SRCADDR] = PC_SW_ADDRESS;
  Msg->Data[SERIAL_COM_CMD] = CMD_Reply_WriteTimeout;
  Msg->Data[SERIAL_COM_LENGTH] = 0;
  Msg->Data[SERIAL_COM_LENGTH + 1] = 0;
  Msg->Len = 5;
}

/**
  * @brief  Build the command write nack reply header
  * @param  Msg: pointer to store the message to send
  * @retval None
  */
void Build_Reply_Header_Cmd_Reply_WriteNack( TMsg * const Msg )
{
  Msg->Data[SERIAL_COM_DESTADDR] = STM32_FW_ADDRESS;
  Msg->Data[SERIAL_COM_SRCADDR] = PC_SW_ADDRESS;
  Msg->Data[SERIAL_COM_CMD] = CMD_Reply_WriteNack;
  Msg->Data[SERIAL_COM_LENGTH] = 0;
  Msg->Data[SERIAL_COM_LENGTH + 1] = 0;
  Msg->Len = 5;
}

/**
  * @brief  Build the command error reply header
  * @param  Msg: pointer to store the message to send
  * @retval None
  */
void Build_Reply_Header_Cmd_Error( TMsg * const Msg )
{
  Msg->Data[SERIAL_COM_DESTADDR] = STM32_FW_ADDRESS;
  Msg->Data[SERIAL_COM_SRCADDR] = PC_SW_ADDRESS;
  Msg->Data[SERIAL_COM_CMD] = CMD_Reply_CmdError;
  Msg->Data[SERIAL_COM_LENGTH] = 0;
  Msg->Data[SERIAL_COM_LENGTH + 1] = 0;
  Msg->Len = 5;
}

/**
  * @brief  Check error on received message
  * @param  Msg: pointer to the message received
  * @retval SERIALCOM_StatusTypeDef
  */
SERIALCOM_StatusTypeDef Check_Error_Msg( TMsg * const Msg )
{
  SERIALCOM_StatusTypeDef ret = SERIALCOM_OK;

  if( Msg->Data[SERIAL_COM_DESTADDR] != PC_SW_ADDRESS )
  { /* Message not from PC SW */
    Build_Reply_Header_PC_Dev_Addr_Error( Msg );
    SerialCom_SendMsg( Msg );
    ret = SERIALCOM_ERROR;
  }
  else if( (Msg->Data[SERIAL_COM_SRCADDR] != STM32_FW_ADDRESS) && (Msg->Data[SERIAL_COM_CMD] != CMD_PING) && (Msg->Data[SERIAL_COM_CMD] != CMD_CHECK_FIRMWAREVERSION)
            && (Msg->Data[SERIAL_COM_CMD] != CMD_GET_STM32_FW_ADDRESS) )
  { /* Wrong addressee */
    Build_Reply_Header_PC_Dev_Addr_Error( Msg );
    SerialCom_SendMsg( Msg );
    ret = SERIALCOM_ERROR;
  }
  else
  {
    switch( Msg->Data[SERIAL_COM_CMD] )
    { /* Check length of Cmd */
      case CMD_PING:
      case CMD_CHECK_FIRMWAREVERSION:
      case CMD_GET_STM32_FW_ADDRESS:
      case CMD_ST25DV_INIT_DRIVER:
      case CMD_ST25DV_IS_INIT:
      case CMD_ST25DV_READ_GPO_PIN:

        if( Msg->Len != SERIAL_COM_PAYLOAD )
        {
          Build_Reply_Header_Length_Error( Msg );
          SerialCom_SendMsg( Msg );
          ret = SERIALCOM_ERROR;
        }
        break;

      case CMD_ST25DV_I2C_SPEED:
      case CMD_ST25DV_SET_RESET_LPD:

        if( Msg->Len != (SERIAL_COM_PAYLOAD + 1) )
        {
          Build_Reply_Header_Length_Error( Msg );
          SerialCom_SendMsg( Msg );
          ret = SERIALCOM_ERROR;
        }
        break;

      case CMD_ST25DV_READ:
      case CMD_ST25DV_READ_CONFIG:

        if( Msg->Len != (SERIAL_COM_PAYLOAD + 4) )
        {
          Build_Reply_Header_Length_Error( Msg );
          SerialCom_SendMsg( Msg );
          ret = SERIALCOM_ERROR;
        }
        break;

      case CMD_ST25DV_WRITE:
      case CMD_ST25DV_WRITE_CONFIG:

        if( Msg->Len <= (SERIAL_COM_PAYLOAD + 4) )
        {
          Build_Reply_Header_Length_Error( Msg );
          SerialCom_SendMsg( Msg );
          ret = SERIALCOM_ERROR;
        }
        break;

      case CMD_ST25DV_PRESENT_PASSWORD:

        if( Msg->Len < (SERIAL_COM_PAYLOAD + 8) )
        {
          Build_Reply_Header_Length_Error( Msg );
          SerialCom_SendMsg( Msg );
          ret = SERIALCOM_ERROR;
        }
        break;

      case CMD_ST25DV_WRITE_PASSWORD:

        if( Msg->Len < (SERIAL_COM_PAYLOAD + 8) )
        {
          Build_Reply_Header_Length_Error( Msg );
          SerialCom_SendMsg( Msg );
          ret = SERIALCOM_ERROR;
        }
        break;

      default:

        Build_Reply_Header_Cmd_Error( Msg );
        SerialCom_SendMsg( Msg );
        ret = SERIALCOM_ERROR;
    }
  }
  return ret;
}

/**
  * @brief  Set payload size in Msg
  * @param  Msg: pointer to the message received
  * @param  payloadsize: size of message payload
  * @retval None
  */
void SetPayloadLength( TMsg * const Msg, const uint16_t payloadsize )
{
  Msg->Data[SERIAL_COM_LENGTH] = (payloadsize >> 8) & 0xFF;
  Msg->Data[SERIAL_COM_LENGTH + 1] = payloadsize & 0xFF;
}

/**
  * @brief  Handle a message
  * @param  Msg: pointer to the message received
  * @retval SERIALCOM_StatusTypeDef
  */
SERIALCOM_StatusTypeDef HandleMSG( TMsg * const Msg )
/*  DestAddr | SouceAddr | CMD | PAYLOAD LENGTH | PAYLOAD  |  CRC  |  EOF
        1          1        1           2            N         1       1    */
{
  SERIALCOM_StatusTypeDef ret = SERIALCOM_OK;
  ST25DV_I2CSSO_STATUS i2csso;
  char *fwvers = ST25DV_SERIALCOM_FW_VERSION;
  uint16_t cnt;
  uint16_t memadd;
  uint16_t nbbytes;
  int32_t status;
  ST25DV_PASSWD mypssd;

  if( Check_Error_Msg( Msg ) == SERIALCOM_ERROR )
  {
    ret = SERIALCOM_ERROR;
  }
  else
  {
    /* if no error handle command */
    switch( Msg->Data[SERIAL_COM_CMD] )
    {

      /*---------------------------------*/
      case CMD_PING:

        Build_Reply_Header( Msg );
        SerialCom_SendMsg( Msg );
        break;

      /*---------------------------------*/
      case CMD_CHECK_FIRMWAREVERSION:

        Build_Reply_Header( Msg );
        cnt = 0;
        while( cnt < strlen(fwvers) )
        {
          Msg->Data[Msg->Len + cnt] = fwvers[cnt];
          cnt++;
        }
        SetPayloadLength( Msg, cnt );
        Msg->Len += cnt;
        SerialCom_SendMsg( Msg );
        break;

      /*---------------------------------*/
      case CMD_GET_STM32_FW_ADDRESS:

        Build_Reply_Header( Msg );
        Msg->Data[Msg->Len] = STM32_FW_ADDRESS;
        Msg->Len++;
        SetPayloadLength( Msg, 1 );
        SerialCom_SendMsg( Msg );
        break;

      /*---------------------------------*/
      case CMD_ST25DV_INIT_DRIVER:

        /* Initialize ST25DV driver return cmd error if failed */
        status = NFC04A1_NFCTAG_Init(NFC04A1_NFCTAG_INSTANCE);
        if( status != NFCTAG_OK )
        {
          Build_Reply_Header_Cmd_Error( Msg );
        }
        else
        {
          Build_Reply_Header( Msg );
        }
        SerialCom_SendMsg( Msg );
        break;

      /*---------------------------------*/
      case CMD_ST25DV_DEINIT_DRIVER:

        /* DeInitialize ST25DV driver  */
        NFC04A1_NFCTAG_DeInit(NFC04A1_NFCTAG_INSTANCE);
        Build_Reply_Header( Msg );
        SerialCom_SendMsg( Msg );
        break;

      /*---------------------------------*/
      case CMD_ST25DV_IS_INIT:

        /* Return 0 if the driver is not initialized, 1 else */
        Build_Reply_Header( Msg );
        Msg->Data[Msg->Len] = NFC04A1_NFCTAG_isInitialized(NFC04A1_NFCTAG_INSTANCE);
        Msg->Len++;
        SetPayloadLength( Msg, 1 );
        SerialCom_SendMsg( Msg );
        break;

      /*---------------------------------*/
      case CMD_ST25DV_I2C_SPEED:

        break;

      /*---------------------------------*/
      case CMD_ST25DV_PRESENT_PASSWORD:

        /* Open Session to allow RegisterS write */
        mypssd.MsbPasswd = ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD]) << 24;
        mypssd.MsbPasswd |= ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 1]) << 16;
        mypssd.MsbPasswd |= ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 2]) << 8;
        mypssd.MsbPasswd |= ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 3]);

        mypssd.LsbPasswd = ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 4]) << 24;
        mypssd.LsbPasswd |= ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 5]) << 16;
        mypssd.LsbPasswd |= ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 6]) << 8;
        mypssd.LsbPasswd |= ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 7]);

        NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, mypssd );
        NFC04A1_NFCTAG_ReadI2CSecuritySession_Dyn(NFC04A1_NFCTAG_INSTANCE, &i2csso );
        Build_Reply_Header( Msg );
        Msg->Data[Msg->Len] = i2csso;
        Msg->Len++;
        SetPayloadLength( Msg, 1 );
        SerialCom_SendMsg( Msg );
        break;

      /*---------------------------------*/
      case CMD_ST25DV_WRITE_PASSWORD:

        /* Change password */
        mypssd.MsbPasswd = ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD]) << 24;
        mypssd.MsbPasswd |= ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 1]) << 16;
        mypssd.MsbPasswd |= ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 2]) << 8;
        mypssd.MsbPasswd |= ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 3]);

        mypssd.LsbPasswd = ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 4]) << 24;
        mypssd.LsbPasswd |= ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 5]) << 16;
        mypssd.LsbPasswd |= ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 6]) << 8;
        mypssd.LsbPasswd |= ((uint32_t)Msg->Data[SERIAL_COM_PAYLOAD + 7]);

        status = NFC04A1_NFCTAG_WriteI2CPassword(NFC04A1_NFCTAG_INSTANCE, mypssd );
        if( status == NFCTAG_OK )
        {
          Build_Reply_Header( Msg );
        }
        else
        {
          Build_Reply_Header_Cmd_Error( Msg );
        }
        SerialCom_SendMsg( Msg );
        break;

      /*---------------------------------*/
      case CMD_ST25DV_READ:

        /* Read EEPROM */
        memadd = Deserialize_s32_MSBfirst( &Msg->Data[SERIAL_COM_PAYLOAD], 2 );
        nbbytes = Deserialize_s32_MSBfirst( &Msg->Data[SERIAL_COM_PAYLOAD + 2], 2 );

        Build_Reply_Header( Msg );
        if( NFC04A1_NFCTAG_ReadData(NFC04A1_NFCTAG_INSTANCE, &Msg->Data[SERIAL_COM_PAYLOAD], memadd, nbbytes ) == NFCTAG_OK )
        {
          Msg->Len += nbbytes;
        }
        else
        {
          Build_Reply_Header_Cmd_Reply_DeviceBusy( Msg );
          Msg->Len += nbbytes;
        }
        SetPayloadLength( Msg, nbbytes );
        SerialCom_SendMsg( Msg );
        break;

      /*---------------------------------*/
      case CMD_ST25DV_WRITE:

        /* Write EEPROM */
        memadd = Deserialize_s32_MSBfirst( &Msg->Data[SERIAL_COM_PAYLOAD], 2 );
        nbbytes = Deserialize_s32_MSBfirst( &Msg->Data[SERIAL_COM_PAYLOAD + 2], 2 );
        status = NFC04A1_NFCTAG_WriteData(NFC04A1_NFCTAG_INSTANCE, &Msg->Data[SERIAL_COM_PAYLOAD + 4], memadd, nbbytes );
        if( status == NFCTAG_OK )
        {
          Build_Reply_Header( Msg );
        }
        else if( status == NFCTAG_NACK )
        {
          Build_Reply_Header_Cmd_Reply_WriteNack( Msg );
        }
        else if( status == NFCTAG_TIMEOUT )
        {
          Build_Reply_Header_Cmd_Reply_WriteTimeout( Msg );
        }
        else
        {
          Build_Reply_Header_Cmd_Error( Msg );
        }

        SerialCom_SendMsg( Msg );
        break;

        /*---------------------------------*/
      case CMD_ST25DV_READ_CONFIG:

        /* Read system area */
        memadd = Deserialize_s32_MSBfirst( &Msg->Data[SERIAL_COM_PAYLOAD], 2 );
        nbbytes = Deserialize_s32_MSBfirst( &Msg->Data[SERIAL_COM_PAYLOAD + 2], 2 );

        Build_Reply_Header( Msg );
        if( NFC04A1_NFCTAG_ReadRegister(NFC04A1_NFCTAG_INSTANCE, &Msg->Data[SERIAL_COM_PAYLOAD], memadd, nbbytes ) == NFCTAG_OK )
        {
          Msg->Len += nbbytes;
        }
        else
        {
          Build_Reply_Header_Cmd_Reply_DeviceBusy( Msg );
          Msg->Len += nbbytes;
        }
        SetPayloadLength( Msg, nbbytes );
        SerialCom_SendMsg( Msg );
        break;

      /*---------------------------------*/
      case CMD_ST25DV_WRITE_CONFIG:

        /* Write system area, need to open session before */
        memadd = Deserialize_s32_MSBfirst( &Msg->Data[SERIAL_COM_PAYLOAD], 2 );
        nbbytes = Deserialize_s32_MSBfirst( &Msg->Data[SERIAL_COM_PAYLOAD + 2], 2 );

        status = NFC04A1_NFCTAG_WriteRegister(NFC04A1_NFCTAG_INSTANCE, &Msg->Data[SERIAL_COM_PAYLOAD + 4], memadd, nbbytes );
        if( status == NFCTAG_OK )
        {
          Build_Reply_Header( Msg );
        }
        else if( status == NFCTAG_NACK )
        {
          Build_Reply_Header_Cmd_Error( Msg );
        }
        else if( status == NFCTAG_TIMEOUT )
        {
          Build_Reply_Header_Cmd_Reply_WriteTimeout( Msg );
        }
        else
        {
          Build_Reply_Header_Cmd_Error( Msg );
        }

        SerialCom_SendMsg( Msg );
        break;

      /*---------------------------------*/
      case CMD_ST25DV_READ_GPO_PIN:

        Build_Reply_Header( Msg );
        Msg->Data[SERIAL_COM_PAYLOAD] = NFC04A1_GPO_ReadPin( );
        Msg->Len++;
        SetPayloadLength( Msg, 1 );
        SerialCom_SendMsg( Msg );
        break;

      /*---------------------------------*/
      case CMD_ST25DV_SET_RESET_LPD:
        if((GPIO_PinState)Msg->Data[SERIAL_COM_PAYLOAD] == GPIO_PIN_SET)
        {
          NFC04A1_LPD_On();
        } else {
          NFC04A1_LPD_Off();
        }
        Build_Reply_Header( Msg );
        SerialCom_SendMsg( Msg );
        break;

      /*---------------------------------*/
      default:

        Build_Reply_Header_Cmd_Error( Msg );
        SerialCom_SendMsg( Msg );
        ret = SERIALCOM_ERROR;
    }
  }
  return ret;
}

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
