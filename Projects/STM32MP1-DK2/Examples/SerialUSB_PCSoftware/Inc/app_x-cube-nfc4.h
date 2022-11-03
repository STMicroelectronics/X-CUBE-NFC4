
/**
  ******************************************************************************
  * File Name          :  stmicroelectronics_x-cube-nfc4_1_4_0.h
  * Description        : This file provides code for the configuration
  *                      of the STMicroelectronics.X-CUBE-NFC4.1.4.0 instances.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_X_CUBE_NFC4_H
#define APP_X_CUBE_NFC4_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "nfc04a1_conf.h"

/* Exported variables ----------------------------------------------------------*/
/**
  * @brief I2C_charac
  *  freq: I2C bus speed (Hz)
  *  freq_min: 80% of I2C bus speed (Hz)
  *  freq_max: 100% of I2C bus speed (Hz)
  *  fall_max: Max fall time of both SDA and SCL signals (ns)
  *  rise_max: Max rise time of both SDA and SCL signals (ns)
  *  hddat_min: Min data hold time (ns)
  *  vddat_max: Max data valid time (ns)
  *  sudat_min: Min data setup time (ns)
  *  lscl_min: Min low period of the SCL clock (ns)
  *  hscl_min: Min high period of the SCL clock (ns)
  *  trise: Rise time (ns)
  *  tfall: Fall time (ns)
  *  dnf: Digital filter coefficient (0-16)
  */
typedef struct
{
  uint32_t freq;
  uint32_t freq_min;
  uint32_t freq_max;
  uint32_t hddat_min;
  uint32_t vddat_max;
  uint32_t sudat_min;
  uint32_t lscl_min;
  uint32_t hscl_min;
  uint32_t trise;
  uint32_t tfall;
  uint32_t dnf;
}I2C_Charac_t;

/**
  * @brief I2C timings parameters
  *  presc: Prescaler value
  *  tscldel: Data setup time
  *  tsdadel: Data hold time
  *  sclh: SCL high period (master mode)
  *  scll: SCL low period (master mode)
  */
typedef struct
{
  uint32_t presc;
  uint32_t tscldel;
  uint32_t tsdadel;
  uint32_t sclh;
  uint32_t scll;
}I2C_Timings_t;
/* Includes ------------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/
void MX_NFC_Init(void);
void MX_NFC_Process(void);
uint32_t App_I2C_GetTiming(uint32_t clock_src_freq, uint32_t i2c_freq);
void App_Compute_PRESC_SCLDEL_SDADEL(uint32_t clock_src_freq, uint32_t I2C_Speed);
uint32_t App_Compute_SCLL_SCLH (uint32_t clock_src_freq, uint32_t I2C_speed);
void App_I2Cx_Init(I2C_HandleTypeDef* hi2c);
#ifdef __cplusplus
}
#endif
#endif /* __INIT_H */

