/**
  @page Serial-USB_PCSoftware sample application for dynamic NFC/RFID tag IC Expansion Board and STM32 Nucleo Boards

  @verbatim
  ******************************************************************************
  * @file    readme.txt  
  * @author  MMY
  * @brief   PC software tool which can drive the ST25DV through the ST-LINK USB
  ******************************************************************************
  *
  * Copyright (c) 2021 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under <License Name> license
  * <SLA00xx>, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0077
  *
  ******************************************************************************
   @endverbatim

  
@par Example Description 

X-NUCLEO-NFC_software.exe can drive the ST25DV through the ST-LINK USB.

When the STM32 Nucleo board is connected, the communication user interface detects the
X-NUCLEO-NFC04A1 expansion board.

1. Open the user interface for the X-NUCLEO-NFC04A1 by clicking on the related button.
2. Access ST25DV registers and memory area by selecting the various tabs
	- EEPROM Tab : to access the ST25DV EEPROM content
	- FTM tab refers to the fast transfer mode
	- Dynamic registers tab refers to volatile registers
	- Configuration tab refers to static registers
	- Registers & RF infos tab refers to read-only registers
	- Password management tab refers to the I²C session feature
	- The remaining tabs are dedicated to NDEF type 5


@par Keywords

NFC, NDEF, EEPROM, I2C, ST25DV04K
  
@par Hardware and Software environment

  - This example runs on STM32 Nucleo boards with X-NUCLEO-NFC04A1 STM32 expansion board
  - This example has been tested with STMicroelectronics:
    - NUCLEO-F401RE RevC board  
    - NUCLEO-L053R8 RevC board
    - NUCLEO-L476RG RevC board

ADDITIONAL_BOARD : X-NUCLEO-NFC04A1 https://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-connect-hw/x-nucleo-nfc04a1.html#
ADDITIONAL_COMP : ST25DV04K https://www.st.com/content/st_com/en/products/nfc/st25-nfc-rfid-tags-readers/st25-dynamic-nfc-tags/st25dv-i2c-series-dynamic-nfc-tags/st25dv04k.html#

@par How to use it? 

In order to make the program work, you must do the following:
 - Open the user interface for the X-NUCLEO-NFC04A1 by clicking on the related button and then can interact with ST25DV device


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
