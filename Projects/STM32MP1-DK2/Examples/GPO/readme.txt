/**
  @page GPO interrupt activation sample application for dynamic NFC/RFID tag IC Expansion Board and STM32 Nucleo Boards
  
  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @author  SRA Application Team
  * @brief   This application contains an example to show how to enable and use the GPO
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

This example application shows how to enable and use the GPO
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
After initialization, an interrupt is programmed to detect field changes in proximity of the
ST25DV. The green LED is switched ON when the field is detected and switched OFF
when the field disappears


X-NUCLEO-NFC04A1 dynamic NFC/RFID tag IC expansion board is based on the ST25DV04K
NFC Type V/RFID tag IC with a dual interface 4 Kbits EEPROM that also features an I²C interface. It
can be powered by the pin of Arduino connector or directly by the received carrier electromagnetic field

The ST25DV04K features a low power down (LPD) pin, a general purpose output (GPO)
and an energy harvesting feature that lets you output the energy coming from the RF field
on the Vout analog pin and acts on ST1 jumper to enable it



@par Keywords

NFC, NDEF, EEPROM, I2C, ST25DV04K, GPO
  
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
 - Open SW4STM32 (this firmware has been successfully tested with Version 2.9.0).
   
 - Rebuild all files and load your image into target memory.
 - Run the example.
 - Alternatively, you can download the pre-built binaries in "Binary" 
   folder included in the distributed package.


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
