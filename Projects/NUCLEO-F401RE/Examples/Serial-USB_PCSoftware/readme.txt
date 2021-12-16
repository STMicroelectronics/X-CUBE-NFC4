/**
  @page Serial-USB_PCSoftware sample application for dynamic NFC/RFID tag IC Expansion Board and STM32 Nucleo Boards
  
  @verbatim
  ******************************************************************************
  * @file    readme.txt  
  * @author  MMY and SRA-SAIL
  * @brief   This application contains an example of PC software tool which can drive the ST25DV through the ST-LINK USB
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
  @endverbatim

  
@par Example Description 

This example application shows PC software tool which can drive the ST25DV through the ST-LINK USB.
------------------------------------
WARNING: When starting the project from Example Selector in STM32CubeMX and regenerating it
from ioc file, you may face a build issue. To solve it, if you started the project for the
Nucleo-L476RG board, remove from the IDE project the file stm32l4xx_nucleo.c in the Application/User
virtual folder and delete, from Src and Inc folders, the files: stm32l4xx_nucleo.c, stm32l4xx_nucleo.h
and stm32l4xx_nucleo_errno.h.
The same operations apply if you starts the project for another STM32 Nucleo board (e.g. for
Nucleo-L053R8, the files to be removed are stm32l0xx_nucleo.c, stm32l0xx_nucleo.c, stm32l0xx_nucleo.h
and stm32l0xx_nucleo_errno.h).
------------------------------------
After system initialization and clock configuration, three LEDs (green, blue and orange) are
sequencially turned ON. After NFC tag and UART inizialization, the three LEDs are
sequencially turned OFF . When the firmware receives a valid frame, the blue LED flashes
once


X-NUCLEO-NFC04A1 dynamic NFC/RFID tag IC expansion board is based on the ST25DV04K
NFC Type V/RFID tag IC with a dual interface 4 Kbits EEPROM that also features an I²C interface. It
can be powered by the pin of Arduino connector or directly by the received carrier electromagnetic field



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
 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.
 - Open STM32CubeIDE (this firmware has been successfully tested with Version 1.6.1).
   Alternatively you can use the Keil uVision toolchain (this firmware
   has been successfully tested with V5.32.0) or the IAR toolchain (this firmware has 
   been successfully tested with Embedded Workbench V8.50.9).
 - Rebuild all files and load your image into target memory.
 - Run the example.
 - Alternatively, you can download the pre-built binaries in "Binary" 
   folder included in the distributed package.


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
