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
#define TR_SWITCH_PIN         GPIO_PIN_0
#define TR_SWITCH_PORT        GPIOB
#define PGA_CS1_PIN           GPIO_PIN_1
#define PGA_CS1_PORT          GPIOB
#define PGA_CS2_PIN           GPIO_PIN_2
#define PGA_CS2_PORT          GPIOB

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
