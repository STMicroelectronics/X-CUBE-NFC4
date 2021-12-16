# X-CUBE-NFC4 Firmware Package

![latest tag](https://img.shields.io/github/v/tag/STMicroelectronics/x-cube-nfc4.svg?color=brightgreen)

The X-CUBE-NFC4 software expansion for STM32Cube provides a complete middleware for STM32 to build applications using dynamic NFC/RFID tag IC (ST25DV device). The software is based on STM32Cube technology and expands STM32Cube based packages. It is built on top of STM32Cube software technology to ease portability across different STM32 microcontrollers.

**X-CUBE-NFC4 software features**:

- Complete middleware to build applications using dynamic NFC/RFID tag IC (ST25DV04K).
- Easy portability across different MCU families thanks to STM32Cube.
- Sample application to communicate with PC software.
- Sample implementation available on the X-NUCLEO-NFC04A1 expansion board, plugged into a NUCLEO-F401RE, NUCLEO-L053R8, or NUCLEO-L476RG board.
- Free, user-friendly license terms.
- Package compatible with STM32CubeMX, can be downloaded from and installed directly into STM32CubeMX

The figure below shows the overall architecture.

[![X-CUBE-NFC4 Block Diagram](_htmresc/X-CUBE-NFC4_components.jpg)]()

- At the bottom layer there are the HW components: the STM32 MCU and the ST25DV04K.  
- The drivers abstract low level details of the hardware and allow the middleware software to provide NFC Tag features in a hardware independent fashion.
- The applications provide examples of how to use the code.

**Related information and documentation**:

- [UM2666](https://www.st.com/content/ccc/resource/technical/document/user_manual/group0/59/22/f4/26/87/f6/47/bd/DM00403798/files/DM00403798.pdf/jcr:content/translations/en.DM00403798.pdf): Getting started with the X-CUBE-NFC4 dynamic NFC/RFID tag IC software expansion for STM32Cube
- [STM32Cube](http://www.st.com/stm32cube)
- [STM32 Nucleo boards](http://www.st.com/stm32nucleo)
- [How to use the X-CUBE-NFC4 pack in the STM32CubeMX tool](https://www.youtube.com/watch?v=-vagkdYS8KU&t=79s)
