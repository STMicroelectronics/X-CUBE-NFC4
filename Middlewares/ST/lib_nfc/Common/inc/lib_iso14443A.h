/**
  ******************************************************************************
  * @file    lib_iso14443A.h
  * @author  MMY Application Team
  * @version 1.0.1
  * @date    28-Feb-2022
  * @brief   ISO14443A common constants.
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

/* Define to prevent recursive inclusion -------------------------------------------------------*/
#ifndef __ISO14443A_H
#define __ISO14443A_H

#include "stdint.h"
#include "stdbool.h"

/*  status and error code ---------------------------------------------------------------------- */
#define ISO14443A_SUCCESSCODE									RESULTOK
#define ISO14443A_ERRORCODE_DEFAULT						0x61
#define ISO14443A_ERRORCODE_CRC								0x62

/* Anticollison levels (commands)  ------------------------------------------------------------- */
#define SEL_CASCADE_LVL_1											0x93
#define SEL_CASCADE_LVL_2											0x95
#define SEL_CASCADE_LVL_3											0x97
#define COMMAND_REQA													0x26
#define COMMAND_WUPA													0x52
#define COMMAND_SELECT_LV1										0x93
#define COMMAND_SELECT_LV2										0x95
#define COMMAND_SELECT_LV3										0x97
#define COMMAND_HLTA													0x50

#define COMMAND_RATS													0xE0
#define COMMAND_PPS														0xD0
#define COMMAND_DESELECT											0xC2
#define COMMAND_DESELECTCID										0xCA

/* Iblock  ------------------------------------------------------------------------------------- */
#define COMMAND_IBLOCK02												0x02
#define COMMAND_IBLOCK03												0x03
#define COMMAND_SBLOCK													0xC2
#define COMMAND_NACKBLOCK_B2										0xB2
#define COMMAND_NACKBLOCK_B3										0xB3
#define COMMAND_ACKBLOCK												0xA2


/* numbr of the cascade level  ----------------------------------------------------------------- */
#define CASCADE_LVL_1															1
#define CASCADE_LVL_2															2
#define CASCADE_LVL_3															3
		
#define ISO14443A_NVM_10													0x10
#define ISO14443A_NVM_20													0x20
#define ISO14443A_NVM_30													0x30
#define ISO14443A_NVM_40													0x40
#define ISO14443A_NVM_50													0x50
#define ISO14443A_NVM_60													0x60
#define ISO14443A_NVM_70													0x70

/* UID Sizes ---------------------------------------------------------------------------------- */
#define ISO14443A_UIDSIZE_UNDEFINED								-1
#define ISO14443A_UID_PART												3
#define ISO14443A_UID_SINGLE_SIZE									4
#define	ISO14443A_UID_DOUBLE_SIZE									7
#define ISO14443A_UID_TRIPLE_SIZE									10


/* Mask used for ATQA ------------------------------------------------------------------------ */
#define ISO14443A_UID_MASK												0xC0
#define ISO14443A_AC_BIT_FRAME_MASK								0x1F
#define ISO14443A_CID_MASK												0x0F
#define ISO14443A_FSDI_MASK												0xF0

/* Size for ISO14443A variables ------------------------------------------------------------- */
#define ISO14443A_MAX_NAME_SIZE										50
#define ISO14443A_MAX_UID_SIZE			 							10
#define ISO14443A_ATQA_SIZE												2

/* SAK FLAG --------------------------------------------------------------------------------- */
#define SAK_FLAG_ATS_SUPPORTED										0x20
#define SAK_FLAG_UID_NOT_COMPLETE									0x04


/* ATQ FLAG */
#define ATQ_FLAG_UID_SINGLE_SIZE		0
#define	ATQ_FLAG_UID_DOUBLE_SIZE		1
#define ATQ_FLAG_UID_TRIPLE_SIZE		2

typedef struct{
	/* ATQA answer to request of type A*/
	uint8_t 	ATQA[ISO14443A_ATQA_SIZE];
	uint8_t 	CascadeLevel;
	/* UID : unique Identification*/
	uint8_t 	UIDsize;
	uint8_t 	UID[ISO14443A_MAX_UID_SIZE];
	/* SAK : Select acknowledge*/
	uint8_t 	SAK;
	bool 		ATSSupported;
	bool 		IsDetected;
	char			LogMsg[120];
	uint8_t 	CID,
						FSDI,
						DRI,
						DSI;
}ISO14443A_CARD;

#endif /* __ISO14443A_H */
