/**
  ******************************************************************************
  * @file    common_h.ftl
  * @author  SRA Application Team
  * @brief   common file for NDEF configuation
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMON_H
#define __COMMON_H

/* Includes ------------------------------------------------------------------*/

#include "nfc04a1_nfctag.h"

#include <string.h>

/* Exported types ------------------------------------------------------------*/
typedef uint8_t boolean;

/**
 * @brief  GPO status information structure definition
 */
typedef struct
{
  uint8_t WritenEEPROM;
  uint8_t RfBusy;
  uint8_t FieldOn;
  uint8_t FieldOff;
  uint8_t MsgInMailbox;
  uint8_t MailboxMsgRead;
  uint8_t RfInterrupt;
  uint8_t Rfuser;
} IT_GPO_STATUS;

/* Exported macro ------------------------------------------------------------*/
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#undef FAIL
#define FAIL 0

#undef PASS
#define PASS !FAIL

/* Exported constants --------------------------------------------------------*/
#define MAX_NDEF_MEM                 0x200
#define ST25DV_MAX_SIZE              NFCTAG_4K_SIZE
#define ST25DV_NDEF_MAX_SIZE         MIN(ST25DV_MAX_SIZE,MAX_NDEF_MEM)
#define NFC_DEVICE_MAX_NDEFMEMORY    ST25DV_NDEF_MAX_SIZE

#define RESULTOK                     0x00
#define ERRORCODE_GENERIC            1

#define TOSTRING(s) #s
#define STRINGIZE(s) TOSTRING(s)

#define MCU_FW_VERSION_MAJOR              1
#define MCU_FW_VERSION_MINOR              0
#define MCU_FW_VERSION_REVISION           0
#define MCU_FW_VERSION                    STRINGIZE(MCU_FW_VERSION_MAJOR) "." \
                                          STRINGIZE(MCU_FW_VERSION_MINOR) "." \
                                          STRINGIZE(MCU_FW_VERSION_REVISION)

/* Exported functions ------------------------------------------------------- */

#endif /* __COMMON_H */

