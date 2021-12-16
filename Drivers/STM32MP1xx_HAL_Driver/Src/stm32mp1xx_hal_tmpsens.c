/**
  ******************************************************************************
  * @file    stm32mp1xx_hal_tmpsens.c
  * @author  MCD Application Team
  * @version V0.0.1
  * @date    01-July-2016
  * @brief   DTS HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the DTS peripheral:
  *           + Initialization and de-initialization functions
  *           + Start/Stop operation functions in polling mode.
  *           + Start/Stop operation functions in interrupt mode.
  *           + Peripheral Control functions
  *           + Peripheral State functions
  *
  @verbatim
================================================================================
          ##### DTS Peripheral features #####
================================================================================

  [..]
      The STM32MP1xx device family integrate one DTS sensor interface :


            ##### How to use this driver #####
================================================================================
  [..]


  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32mp1xx_hal.h"

/** @addtogroup STM32MP1xx_HAL_Driver
  * @{
  */

/** @defgroup DTS DTS
  * @brief DTS HAL module driver
  * @{
  */

#ifdef HAL_DTS_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup DTS_Private_Constants
  * @{
  */

/* @brief CFGR register reset value */
#define DTS_CFGR1_RESET_VALUE (0U)

/* @brief Delay for DTS startup time
 * @note  Delay required to get ready for DTS Block.
 * @note  Unit: ms
 */
#define DTS_DELAY_STARTUP (1U)

/* @brief DTS measure ready flag time out value.
 * @note  Maximal measurement time is when LSE is selected as ref_clock and
 *        maximal sampling time is used, taking calibration into account this
 *        is equivalent to ~620 us. Use 5 ms as arbitrary timeout
 * @note Unit: ms
 */
#define TS_TIMEOUT_MS (5U)

/* @brief DTS High Speed Clock frequency
 * @note Unit: 100 Hz
 */
#if defined(STM32MP1)
#define HS_CLK_FREQ   (HAL_RCC_GetPCLK3Freq())
#endif
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup DTS_Exported_Functions DTS Exported Functions
  * @{
  */

/** @defgroup DTS_Exported_Functions_Group1 Initialization/de-initialization functions
 *  @brief    Initialization and de-initialization functions.
 *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions to initialize and de-initialize comparators

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the DTS according to the specified
  *         parameters in the DTS_InitTypeDef and initialize the associated handle.
  * @note   If the selected comparator is locked, initialization can't be performed.
  *         To unlock the configuration, perform a system reset.
  * @param  hdts  DTS handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_Init(DTS_HandleTypeDef *hdts)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the DTS handle allocation and lock status */
  /* Init not allowed if calibration is ongoing */
  if ((hdts == NULL)  || (hdts->State == HAL_DTS_STATE_BUSY_LOCKED)
      || (hdts->State == HAL_DTS_STATE_CALIBBUSY))
  {
    return HAL_ERROR;
  }
  else
  {
    /* Check the parameters */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));
    assert_param(IS_DTS_QUICKMEAS(hdts->Init.QuickMeasure));
    assert_param(IS_DTS_REFCLK(hdts->Init.RefClock));
    assert_param(IS_DTS_TRIGGERINPUT(hdts->Init.TriggerInput));
    assert_param(IS_DTS_SAMPLINGTIME(hdts->Init.SamplingTime));
    assert_param(IS_DTS_INTERRUPTTHRESHOLD(hdts->Init.HighInterruptThreshold));
    assert_param(IS_DTS_INTERRUPTTHRESHOLD(hdts->Init.LowInterruptThreshold));

    if (hdts->State == HAL_DTS_STATE_RESET)
    {
      /* Init the low level hardware */
      HAL_DTS_MspInit(hdts);
    }

    /* Check factory settings */
    if (!hdts->Instance->RAMPVALR || !hdts->Instance->T0VALR1)
    {
      goto DeInit;
    }

    /* Check Quick Measure option is enabled or disabled */
    if (hdts->Init.QuickMeasure == DTS_QUICKMEAS_DISABLE)
    {
      /* Check Reference clock selection */
      if (hdts->Init.RefClock == DTS_REFCLKSEL_PCLK)
      {
        assert_param(IS_DTS_DIVIDER_RATIO_NUMBER(hdts->Init.Divider));
      }
      /* Quick measurement mode disabled */
      CLEAR_BIT(hdts->Instance->CFGR1, DTS_CFGR1_Q_MEAS_OPT);
    }
    else
    {
      /* DTS_QUICKMEAS_ENABLE shall be used only when the LSE clock is
         selected as reference clock */
      if (hdts->Init.RefClock != DTS_REFCLKSEL_LSE)
      {
        goto DeInit;
      }
      /* Quick measurement mode enabled
         No calibration needed */
      SET_BIT(hdts->Instance->CFGR1, DTS_CFGR1_Q_MEAS_OPT);
    }

    /* set the DTS clk source */
    if (hdts->Init.RefClock == DTS_REFCLKSEL_LSE)
    {
      SET_BIT(hdts->Instance->CFGR1, DTS_CFGR1_REFCLK_SEL);
    }
    else
    {
      CLEAR_BIT(hdts->Instance->CFGR1, DTS_CFGR1_REFCLK_SEL);
    }

    MODIFY_REG(hdts->Instance->CFGR1, DTS_CFGR1_HSREF_CLK_DIV,
               (hdts->Init.Divider << DTS_CFGR1_HSREF_CLK_DIV_Pos));
    MODIFY_REG(hdts->Instance->CFGR1, DTS_CFGR1_TS1_SMP_TIME,
               hdts->Init.SamplingTime);

    if (hdts->Init.TriggerInput == DTS_TRIGGER_HW_NONE_CONT)
    {
      MODIFY_REG(hdts->Instance->CFGR1, DTS_CFGR1_TS1_INTRIG_SEL,
                 DTS_TRIGGER_HW_NONE);
    }
    else
    {
      MODIFY_REG(hdts->Instance->CFGR1, DTS_CFGR1_TS1_INTRIG_SEL,
                 hdts->Init.TriggerInput);
    }

    MODIFY_REG(hdts->Instance->ITR1, DTS_ITR1_TS1_HITTHD,
               (hdts->Init.HighInterruptThreshold << DTS_ITR1_TS1_HITTHD_Pos));
    MODIFY_REG(hdts->Instance->ITR1, DTS_ITR1_TS1_LITTHD,
               hdts->Init.LowInterruptThreshold);

    /* Set HAL DTS handle state */
    if (hdts->State == HAL_DTS_STATE_RESET)
    {
      hdts->State = HAL_DTS_STATE_READY;
    }
  }

  return status;

DeInit:
  HAL_DTS_MspDeInit(hdts);
  return HAL_ERROR;

}

/**
  * @brief  DeInitialize the DTS peripheral.
  * @note   Deinitialization cannot be performed if the DTS configuration is locked.
  *         To unlock the configuration, perform a system reset.
  * @param  hdts  DTS handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_DeInit(DTS_HandleTypeDef *hdts)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the DTS handle allocation */
  /* DeInit not allowed if calibration is on going */
  if ((hdts == NULL) || (hdts->State == HAL_DTS_STATE_CALIBBUSY))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));

    /* Set DTS_CFGR register to reset value */
    WRITE_REG(hdts->Instance->CFGR1, DTS_CFGR1_RESET_VALUE);

    /* DeInit the low level hardware */
    HAL_DTS_MspDeInit(hdts);

    hdts->State = HAL_DTS_STATE_RESET;

    /* Release Lock */
    __HAL_UNLOCK(hdts);
  }

  return status;
}

/**
  * @brief  Initialize the DTS MSP.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_MspInit(DTS_HandleTypeDef *hdts)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_DTS_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitialize the DTS MSP.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_MspDeInit(DTS_HandleTypeDef *hdts)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_DTS_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup DTS_Exported_Functions_Group2 Start-Stop operation functions
 *  @brief   Start-Stop operation functions.
 *
@verbatim
 ===============================================================================
                      ##### DTS Start Stop operation functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Start a DTS Sensor without interrupt.
      (+) Stop a DTS Sensor  without interrupt.
      (+) Start a DTS Sensor  with interrupt generation.
      (+) Stop a DTS Sensor  with interrupt generation.

@endverbatim
  * @{
  */

/**
  * @brief  Start the DTS sensor.
  * @param  hdts  DTS handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_Start(DTS_HandleTypeDef *hdts)
{
  uint32_t tickstart = 0;
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the DTS handle allocation and lock status */
  if ((hdts == NULL) || (hdts->State == HAL_DTS_STATE_CALIBBUSY))
  {
    status = HAL_ERROR;
  }
  else
  {
    if (hdts->State == HAL_DTS_STATE_READY)
    {
      /* Enable the selected DTS sensor */
      __HAL_DTS_ENABLE(hdts);

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till TS1_RDY flag is set */
      while (__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_RDY) == RESET)
      {
        if ((int32_t)(HAL_GetTick() - tickstart) > DTS_DELAY_STARTUP)
        {
          return HAL_TIMEOUT;
        }
      }

      if (__HAL_DTS_GET_TRIGGER(hdts, DTS_TRIGGER_HW_NONE_CONT) == SET)
      {
        /* Start continuous measures */
        SET_BIT(hdts->Instance->CFGR1, DTS_CFGR1_TS1_START);

        /* Ensure start is taken into account */
        HAL_Delay(1);
      }

      hdts->State = HAL_DTS_STATE_BUSY;
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}
/**
  * @brief  Enable the interrupt(s) and start the DTS sensor
  * @param  hdts  DTS handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_Start_IT(DTS_HandleTypeDef *hdts)
{
  uint32_t tickstart = 0;
  HAL_StatusTypeDef status = HAL_OK;

  assert_param(IS_DTS_INTERRUPTSOURCE(hdts->Init.InterruptSource));

  /* Check the DTS handle allocation and lock status */
  if ((hdts == NULL) || (hdts->State == HAL_DTS_STATE_CALIBBUSY))
  {
    status = HAL_ERROR;
  }
  else
  {
    if (hdts->State == HAL_DTS_STATE_READY)
    {
      /* On Asynchronous mode enable the DTS EXTI line */
      if ((hdts->Init.InterruptSource & DTS_IT_ASYNC) != RESET)
      {
        if (__HAL_DTS_GET_REFCLK(hdts, DTS_REFCLKSEL_LSE) != SET)
        {
          status = HAL_ERROR;
        }
        else
        {
          __HAL_DTS_EXTI_ENABLE_IT();
        }
      }

      /* Enable the IT(s) */
      __HAL_DTS_ENABLE_IT(hdts, hdts->Init.InterruptSource);

      /* Enable the selected DTS sensor */
      __HAL_DTS_ENABLE(hdts);

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till TS1_RDY flag is set */
      while (__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_RDY) == RESET)
      {
        if ((int32_t)(HAL_GetTick() - tickstart) > DTS_DELAY_STARTUP)
        {
          return HAL_TIMEOUT;
        }
      }

      if (__HAL_DTS_GET_TRIGGER(hdts, DTS_TRIGGER_HW_NONE_CONT) == SET)
      {
        /* Start continuous measures */
        SET_BIT(hdts->Instance->CFGR1, DTS_CFGR1_TS1_START);

        /* Ensure start is taken into account */
        HAL_Delay(1);
      }

      hdts->State = HAL_DTS_STATE_BUSY;
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}
/**
  * @brief  Stop the DTS Sensor.
  * @param  hdts  DTS handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_Stop(DTS_HandleTypeDef *hdts)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the DTS handle allocation and lock status */
  if ((hdts == NULL) || (hdts->State == HAL_DTS_STATE_BUSY_LOCKED) \
      || (hdts->State == HAL_DTS_STATE_CALIBBUSY))
  {
    status = HAL_ERROR;
  }
  else
  {
    if (hdts->State == HAL_DTS_STATE_BUSY)
    {
      if (__HAL_DTS_GET_TRIGGER(hdts, DTS_TRIGGER_HW_NONE_CONT) == SET)
      {
        CLEAR_BIT(hdts->Instance->CFGR1, DTS_CFGR1_TS1_START);

        /* Ensure stop is taken into account */
        HAL_Delay(1);
      }

      /* Disable the selected DTS sensor */
      __HAL_DTS_DISABLE(hdts);

      hdts->State = HAL_DTS_STATE_READY;
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}
/**
  * @brief  Disable the interrupt(s) and stop the DTS sensor.
  * @param  hdts  DTS handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_Stop_IT(DTS_HandleTypeDef *hdts)
{
  HAL_StatusTypeDef status = HAL_OK;

  assert_param(IS_DTS_INTERRUPTSOURCE(hdts->Init.InterruptSource));

  /* Check the DTS handle allocation and lock status */
  if ((hdts == NULL) || (hdts->State == HAL_DTS_STATE_BUSY_LOCKED) \
      || (hdts->State == HAL_DTS_STATE_CALIBBUSY))
  {
    status = HAL_ERROR;
  }
  else
  {
    if (hdts->State == HAL_DTS_STATE_BUSY)
    {

      if (__HAL_DTS_GET_TRIGGER(hdts, DTS_TRIGGER_HW_NONE_CONT) == SET)
      {
        CLEAR_BIT(hdts->Instance->CFGR1, DTS_CFGR1_TS1_START);

        /* Ensure stop is taken into account */
        HAL_Delay(1);
      }

      /* Disable the selected DTS sensor */
      __HAL_DTS_DISABLE(hdts);

      __HAL_DTS_DISABLE_IT(hdts, hdts->Init.InterruptSource);

      /* On Asynchronous mode disable the DTS EXTI line */
      if ((hdts->Init.InterruptSource & DTS_IT_ASYNC) != RESET)
      {
        __HAL_DTS_EXTI_DISABLE_IT();
      }

      hdts->State = HAL_DTS_STATE_READY;
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}
/**
  * @brief  Get temperature from DTS
  * @param  hdts  DTS handle
  * @note   If DTS_TRIGGER_HW_NONE (SW trigger) is used one measure is
  *         triggered by this function. Else (HW trigger) this function
  *         retrieves latest available measure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_GetDTS(DTS_HandleTypeDef *hdts)
{
  uint32_t tickstart = 0;

  int32_t freq_meas = 0;
  int32_t samples = 0;

  int32_t t0_temp ;
  int32_t t0_freq;
  int32_t ramp_coeff;
  uint32_t count_read = 0; /* @TODO: Erase after debug */
  uint32_t tmp_smp_time = 0;

  /* Software trigger */
  if (__HAL_DTS_GET_TRIGGER(hdts, DTS_TRIGGER_HW_NONE) == SET)
  {
    /* Start measure */
    SET_BIT(hdts->Instance->CFGR1, DTS_CFGR1_TS1_START);

    /* Ensure start is taken into account */
    HAL_Delay(1);

    /* Stop measure */
    CLEAR_BIT(hdts->Instance->CFGR1, DTS_CFGR1_TS1_START);

    /* Get Start Tick*/
    tickstart = HAL_GetTick();

    /* Wait till TS1_RDY flag is set */
    while (__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_RDY) == RESET)
    {
      count_read++;
      if ((HAL_GetTick() - tickstart) > TS_TIMEOUT_MS)
      {
        /* @TODO: erase after debug Ts1_Count_R and hdts->Measure.Ts1_Sample*/
        hdts->Measure.Ts1_Count_R = count_read;
        hdts->Measure.Ts1_Sample = (hdts->Instance->DR & DTS_DR_TS1_MFREQ);
        return HAL_TIMEOUT;
      }
    }
  }

  /* Get the total number of samples */
  samples = (hdts->Instance->DR & DTS_DR_TS1_MFREQ);

  /* Save the total number of samples (debug). @TODO: To erase */
  hdts->Measure.Ts1_Sample = samples;

  if (samples == RESET)
  {
    return HAL_ERROR;
  }

  tmp_smp_time = ((hdts->Instance->CFGR1 & DTS_CFGR1_TS1_SMP_TIME) >> DTS_CFGR1_TS1_SMP_TIME_Pos);
  if (tmp_smp_time == 0)
  {
    tmp_smp_time = 1;
  }

  /* Get the mean number of samples per period */
  samples /= tmp_smp_time;

  if ((hdts->Instance->CFGR1 & DTS_CFGR1_REFCLK_SEL) == DTS_REFCLKSEL_LSE)
  {
    freq_meas = LSE_VALUE * samples; /* On Hz */
  }
  else
  {
    freq_meas = HS_CLK_FREQ / samples; /* On Hz */
  }

  /* Save measured frequency (debug) */
  hdts->Measure.Ts1_Fmt = freq_meas;

  /* Read factory settings */
  t0_temp = hdts->Instance->T0VALR1 >> DTS_T0VALR1_TS1_T0_Pos;
  if ((t0_temp  & DTS_T0VALR1_TS1_T0) == 0U)
  {
    t0_temp = 30; /* 30 oC */
  }
  t0_freq = (hdts->Instance->T0VALR1 & DTS_T0VALR1_TS1_FMT0) * 100; /* [Hz] */
  ramp_coeff = hdts->Instance->RAMPVALR & DTS_RAMPVALR_TS1_RAMP_COEFF; /* [oC/Hz] */

  if (ramp_coeff == RESET)
  {
    return HAL_ERROR;
  }

  /* Figure out the temperature [oC] */
  hdts->Measure.Ts1_Temp = t0_temp + ((freq_meas - t0_freq) / ramp_coeff);

  return HAL_OK;
}

/**
  * @brief  DTS sensor IRQ Handler.
  * @param  hdts  DTS handle
  * @retval HAL status
  */
void HAL_DTS_IRQHandler(DTS_HandleTypeDef *hdts)
{
  /* DTS interrupts */

  /* Check end of measure Asynchronous IT */
  if ((__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_AITEF)) != RESET)
  {
    __HAL_DTS_CLEAR_FLAG(hdts, DTS_CLEAR_TS1_CAITEF);
    HAL_DTS_AsyncEndCallback(hdts);
  }

  /* Check low threshold Asynchronous IT */
  if ((__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_AITLF)) != RESET)
  {
    __HAL_DTS_CLEAR_FLAG(hdts, DTS_CLEAR_TS1_CAITLF);
    HAL_DTS_AsyncLowCallback(hdts);
  }

  /* Check high threshold Asynchronous IT */
  if ((__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_AITHF)) != RESET)
  {
    __HAL_DTS_CLEAR_FLAG(hdts, DTS_CLEAR_TS1_CAITHF);
    HAL_DTS_AsyncHighCallback(hdts);
  }

  /* Check end of measure IT */
  if ((__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_ITEF)) != RESET)
  {
    __HAL_DTS_CLEAR_FLAG(hdts, DTS_CLEAR_TS1_CITEF);
    HAL_DTS_EndCallback(hdts);
  }

  /* Check low threshold IT */
  if ((__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_ITLF)) != RESET)
  {
    __HAL_DTS_CLEAR_FLAG(hdts, DTS_CLEAR_TS1_CITLF);
    HAL_DTS_LowCallback(hdts);
  }

  /* Check high threshold IT */
  if ((__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_ITHF)) != RESET)
  {
    __HAL_DTS_CLEAR_FLAG(hdts, DTS_CLEAR_TS1_CITHF);
    HAL_DTS_HighCallback(hdts);

  }
}

/**
  * @}
  */

/** @defgroup DTS_Exported_Functions_Group3 Peripheral Control functions
 *  @brief   Management functions.
 *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the DTS sensors.

@endverbatim
  * @{
  */


/**
  * @brief  DTS Sensor End measure callback.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_EndCallback(DTS_HandleTypeDef *hdts)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_DTS_EndCallback should be implemented in the user file
   */
}

/**
  * @brief  DTS Sensor End measure callback.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_LowCallback(DTS_HandleTypeDef *hdts)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_DTS_EndCallback should be implemented in the user file
   */
}

/**
  * @brief  DTS Sensor End measure callback.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_HighCallback(DTS_HandleTypeDef *hdts)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_DTS_EndCallback should be implemented in the user file
   */
}

/**
  * @brief  DTS Sensor End measure callback.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_AsyncEndCallback(DTS_HandleTypeDef *hdts)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_DTS_EndCallback should be implemented in the user file
   */
}

/**
  * @brief  DTS Sensor End measure callback.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_AsyncLowCallback(DTS_HandleTypeDef *hdts)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_DTS_EndCallback should be implemented in the user file
   */
}

/**
  * @brief  DTS Sensor End measure callback.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_AsyncHighCallback(DTS_HandleTypeDef *hdts)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_DTS_EndCallback should be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup DTS_Exported_Functions_Group4 Peripheral State functions
 *  @brief   Peripheral State functions.
 *
@verbatim
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================
    [..]
    This subsection permits to get in run-time the status of the peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Return the DTS handle state.
  * @param  hdts  DTS handle
  * @retval HAL state
  */
HAL_DTS_StateTypeDef HAL_DTS_GetState(DTS_HandleTypeDef *hdts)
{
  /* Check the DTS handle allocation */
  if (hdts == NULL)
  {
    return HAL_DTS_STATE_RESET;
  }

  /* Return DTS handle state */
  return hdts->State;
}
/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_COMP_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
