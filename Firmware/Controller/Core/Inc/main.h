/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_ENABLE_Pin GPIO_PIN_14
#define LED_ENABLE_GPIO_Port GPIOC
#define DIM1_Pin GPIO_PIN_0
#define DIM1_GPIO_Port GPIOA
#define DIM2_Pin GPIO_PIN_1
#define DIM2_GPIO_Port GPIOA
#define DIM3_Pin GPIO_PIN_2
#define DIM3_GPIO_Port GPIOA
#define DIM4_Pin GPIO_PIN_3
#define DIM4_GPIO_Port GPIOA
#define TRIGGER_TTL_Pin GPIO_PIN_6
#define TRIGGER_TTL_GPIO_Port GPIOA
#define TRIGGER_ISO_Pin GPIO_PIN_7
#define TRIGGER_ISO_GPIO_Port GPIOA
#define FEEDBACK_TTL_Pin GPIO_PIN_0
#define FEEDBACK_TTL_GPIO_Port GPIOB
#define FEEDBACK_ISO_Pin GPIO_PIN_1
#define FEEDBACK_ISO_GPIO_Port GPIOB
#define DEBUG_PIN_Pin GPIO_PIN_15
#define DEBUG_PIN_GPIO_Port GPIOA
#define STATUS_GOOD_Pin GPIO_PIN_4
#define STATUS_GOOD_GPIO_Port GPIOB
#define STATUS_BAD_Pin GPIO_PIN_5
#define STATUS_BAD_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
