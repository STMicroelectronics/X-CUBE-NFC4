
## <b>Mailbox Example Description</b>

This application contains an example to show how  to write a message into the mailbox and how to read mailbox status register

Example Description:
  
This example application shows how  to write a message into the mailbox and how to read mailbox
status register.

Text is displayed on a UART console (via ST-LINK) if a PC is connected

The ST25DV04K can act as a mailbox in fast transfer mode in both directions between the
RF terminal (such as an e-reader or a smart phone) and the microcontroller through the
I²C. In this case, messages are stored in the RAM instead of EEPROM. This mode allows
exchanging up to 256 byte payload data via RF and I²C interfaces

X-NUCLEO-NFC04A1 dynamic NFC/RFID tag IC expansion board is based on the ST25DV04K
NFC Type V/RFID tag IC with a dual interface 4 Kbits EEPROM that also features an I²C interface. It
can be powered by the pin of Arduino connector or directly by the received carrier electromagnetic field

Known limitations:

- When starting the project from Example Selector in STM32CubeMX and regenerating it
  from ioc file, you may face a build issue. To solve it, if you started the project for the
  Nucleo-F401RE board, remove from the IDE project the file stm32f4xx_nucleo.c in the Application/User
  virtual folder and delete, from Src and Inc folders, the files: stm32f4xx_nucleo.c, stm32f4xx_nucleo.h
  and stm32f4xx_nucleo_errno.h.
  The same operations apply if you starts the project for another STM32 Nucleo board (e.g. for
  Nucleo-L053R8, the files to be removed are stm32l0xx_nucleo.c, stm32l0xx_nucleo.c, stm32l0xx_nucleo.h
  and stm32l0xx_nucleo_errno.h).

### <b>Keywords</b>

NFC, NDEF, EEPROM, I2C, ST25DV04K

### <b>Directory contents</b>

 - app_x-cube-nfc4.c      Mailbox initialization and applicative code.
 
 - main.c                 Main program body.

 - stm32**xx_hal_msp.c    This file provides code for the MSP Initialization
                          and de-Initialization.
						
 - stm32**xx_nucleo_bus.c Source file for the BSP BUS IO driver.
 
 - stm32**xx_it.c         Source code for interrupt Service Routines.
 
 - stm32**xx_nucleo.c     Source file for the BSP Common driver
 
 - system_stm32**xx.c     CMSIS Cortex-M4 Device Peripheral Access Layer
                          System Source File.

 
### <b>Hardware and Software environment</b>

  - This example runs on STM32 Nucleo boards with X-NUCLEO-NFC04A1 STM32 expansion board
  - This example has been tested with STMicroelectronics:
    - NUCLEO-F401RE RevC board  
    - NUCLEO-L053R8 RevC board
    - NUCLEO-L476RG RevC board
    and can be easily tailored to any other supported device and development 
    board.
     

ADDITIONAL_BOARD : X-NUCLEO-NFC04A1 https://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-connect-hw/x-nucleo-nfc04a1.html#
ADDITIONAL_COMP : ST25DV04K https://www.st.com/content/st_com/en/products/nfc/st25-nfc-rfid-tags-readers/st25-dynamic-nfc-tags/st25dv-i2c-series-dynamic-nfc-tags/st25dv04k.html#

### <b>How to use it?</b> 

-   In order to make the program work, you must do the following:
    - WARNING: before opening the project with any toolchain be sure your folder installation path is not too in-depth since the toolchain may report errors after building.
    - Open STM32CubeIDE (this firmware has been successfully tested with Version 1.10.1). Alternatively you can use the Keil uVision toolchain (this firmware has been successfully tested with V5.37.0) or the IAR toolchain   (this firmware has been successfully tested with Embedded Workbench V9.20.1).
    - Rebuild all files and load your image into target memory.
    - Run the example.
    - Alternatively, you can download the pre-built binaries in "Binary" folder included in the distributed package.

### <b>Author</b>

SRA Application Team

### <b>License</b>

Copyright (c) 2021 STMicroelectronics.
All rights reserved.

This software is licensed under terms that can be found in the LICENSE file
in the root directory of this software component.
If no LICENSE file comes with this software, it is provided AS-IS.
