/**
  * ==============================================================================
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  * @project        : OpenDVL
  * ==============================================================================
  */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
/* LTC1144 Switched-Capacitor Inverter Control */
#define LTC1144_SHDN_PIN       GPIO_PIN_4
#define LTC1144_SHDN_PORT      GPIOC

/* MAX4427 Gate Driver Inputs (TIM1 PWM outputs) */
#define MAX4427_INA_PIN        GPIO_PIN_9
#define MAX4427_INA_PORT       GPIOE
#define MAX4427_INB_PIN        GPIO_PIN_11
#define MAX4427_INB_PORT       GPIOE

/* AD600JNZ TVG DAC outputs */
#define TVG_DAC_CH1_PIN        GPIO_PIN_4
#define TVG_DAC_CH1_PORT       GPIOA
#define TVG_DAC_CH2_PIN        GPIO_PIN_5
#define TVG_DAC_CH2_PORT       GPIOA


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
