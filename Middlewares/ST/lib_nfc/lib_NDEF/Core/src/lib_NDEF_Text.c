/**
  ******************************************************************************
  * @file    lib_NDEF_Text.c
  * @author  MMY Application Team
  * @version 1.3.2
  * @date    28-Feb-2022
  * @brief   This file help to manage Text NDEF file.
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

/* Includes ------------------------------------------------------------------*/
#include "lib_NDEF_Text.h"
#include "stdlib.h"

/** @addtogroup NFC_libraries
  * @{
  * @brief  <b>This is the library used to manage the content of the TAG (data)
  *          But also the specific feature of the tag, for instance
  *          password, gpo... </b>
  */


/** @addtogroup libNFC_FORUM
  * @{
  * @brief  This part of the library manage data which follow NFC forum organisation.
  */

/**
  * @brief  This buffer contains the data send/received by TAG
  */
extern uint8_t NDEF_Buffer [];

/**
  * @}
  */

/** @defgroup libEmail_Public_Functions
  * @{
  * @brief  This file is used to manage Email (stored or loaded in tag)
  */ 

/**
  * @brief  This function write the text in the TAG.
  * @param  text : text to write.
  * @retval NDEF_OK : NDEF file data written in the tag.
  * @retval NDEF_ERROR : not able to store NDEF in tag.
  * @retval NDEF_ERROR_MEMORY_INTERNAL : Cannot write to tag.
  * @retval NDEF_ERROR_NOT_FORMATED : CCFile data not supported or not present.
  * @retval NDEF_ERROR_MEMORY_TAG : Size not compatible with memory.
  * @retval NDEF_ERROR_LOCKED : Tag locked, cannot be write.
  */
uint16_t NDEF_WriteText( char *text )
{
  uint16_t status = NDEF_ERROR;
  uint32_t textSize, Offset = 0;

 
  /* TEXT : 1+en+message */
  textSize = 3 + strlen(text);

  /* TEXT header */
  NDEF_Buffer[Offset] = 0xD1;
  if( textSize < 256 ) NDEF_Buffer[Offset] |= 0x10;                   // Set the SR bit
  Offset++;

  NDEF_Buffer[Offset++] = TEXT_TYPE_STRING_LENGTH;
  if( textSize > 255 )
  {
    NDEF_Buffer[Offset++] = (textSize & 0xFF000000) >> 24;
    NDEF_Buffer[Offset++] = (textSize & 0x00FF0000) >> 16;
    NDEF_Buffer[Offset++] = (textSize & 0x0000FF00) >> 8;
    NDEF_Buffer[Offset++] = textSize & 0x000000FF;
  }
  else
  {
    NDEF_Buffer[Offset++] = (uint8_t)textSize;
  }
  memcpy( &NDEF_Buffer[Offset], TEXT_TYPE_STRING, TEXT_TYPE_STRING_LENGTH );
  Offset += TEXT_TYPE_STRING_LENGTH;

  /* TEXT payload */
  NDEF_Buffer[Offset++] = ISO_ENGLISH_CODE_STRING_LENGTH;
  memcpy( &NDEF_Buffer[Offset], ISO_ENGLISH_CODE_STRING, ISO_ENGLISH_CODE_STRING_LENGTH );
  Offset += ISO_ENGLISH_CODE_STRING_LENGTH;

  memcpy( &NDEF_Buffer[Offset], text, strlen(text) );
  Offset += strlen(text);

  status = NfcTag_WriteNDEF( Offset , NDEF_Buffer );

  return status;
}


/**
  * @brief  This function the Text information in a record if any.
  * @param  pRecordStruct Pointer on a NDEF record structure.
  * @param  pText A pointer on a text information structure, used to return the text & its metadata.
  * @retval NDEF_OK                    Text information has been retrieved.
  * @retval NDEF_ERROR                 The record is not a Text record.
  * @retval NDEF_ERROR_MEMORY_INTERNAL Language code or Text length is too big for the buffers.
  */
uint16_t NDEF_ReadText( sRecordInfo_t *pRecordStruct, NDEF_Text_info_t *pText )
{
  uint16_t status = NDEF_ERROR;

  if( pRecordStruct->NDEF_Type == TEXT_TYPE )
  {
    /* Get the text metadata (status byte (encoding & language code length) + language code) */
    NDEF_Text_metadata_t *text_record_info = (NDEF_Text_metadata_t *)pRecordStruct->PayloadBufferAdd;
        
    uint32_t text_length = pRecordStruct->PayloadLength           /* record length */
                           - text_record_info->language_length    /* minus language code length */
                           - sizeof(uint8_t);                     /* minus the status byte length */

#ifdef NDEF_DYN_ALLOC
    if(text_record_info->language_length >= NDEF_TEXT_LANGUAGE_CODE_MAX_LENGTH)
#else    
    if((text_record_info->language_length >= NDEF_TEXT_LANGUAGE_CODE_MAX_LENGTH) || 
        (text_length >= NDEF_TEXT_MAX_LENGTH))
#endif
    {
      /* One of the text info structure buffer is too small */
      return NDEF_ERROR_MEMORY_INTERNAL;
    }
    
    /* Retrieve the encoding */
    pText->encoding =  (NDEF_Text_encoding_t)text_record_info->encoding;

    /* Save the language code string (adding null char at the end) */
    memcpy(&pText->language_code,text_record_info->language,text_record_info->language_length);
    pText->language_code[text_record_info->language_length] = '\0';
    
    /* Copy the text string itself (adding null char at the end) */
#ifdef NDEF_DYN_ALLOC
    pText->text = malloc(text_length + 1);
    if(pText->text != NULL)
#endif
    {  
      memcpy(pText->text, text_record_info->language + text_record_info->language_length,text_length);
      pText->text[text_length] = '\0';
    }  
    status = NDEF_OK;
  }
  else 
  {
    /* Not a text record, exit in error */
    status = NDEF_ERROR;
  }

  return status;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
