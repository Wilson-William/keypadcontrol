/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32g0xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GPIO_SWITCH1_Pin GPIO_PIN_14
#define GPIO_SWITCH1_GPIO_Port GPIOC
#define KEYPAD_BUZZER_Pin GPIO_PIN_15
#define KEYPAD_BUZZER_GPIO_Port GPIOC
#define GPIO_W5500PMODE2_Pin GPIO_PIN_1
#define GPIO_W5500PMODE2_GPIO_Port GPIOA
#define GPIO_W5500RST_Pin GPIO_PIN_2
#define GPIO_W5500RST_GPIO_Port GPIOA
#define SPI2_CS_Pin GPIO_PIN_5
#define SPI2_CS_GPIO_Port GPIOA
#define GPIO_RELAY_Pin GPIO_PIN_6
#define GPIO_RELAY_GPIO_Port GPIOA
#define GPIO_W5500PMODE0_Pin GPIO_PIN_7
#define GPIO_W5500PMODE0_GPIO_Port GPIOA
#define GPIO_W5500PMODE1_Pin GPIO_PIN_0
#define GPIO_W5500PMODE1_GPIO_Port GPIOB
#define KEYPAD_INT_Pin GPIO_PIN_1
#define KEYPAD_INT_GPIO_Port GPIOB
#define GPIO_REEDSWITCH_Pin GPIO_PIN_2
#define GPIO_REEDSWITCH_GPIO_Port GPIOB
#define GPIO_TAMPERSWITCH1_Pin GPIO_PIN_8
#define GPIO_TAMPERSWITCH1_GPIO_Port GPIOA
#define GPIO_TAMPERSWITCH2_Pin GPIO_PIN_9
#define GPIO_TAMPERSWITCH2_GPIO_Port GPIOA
#define GPIO_PIR_Pin GPIO_PIN_6
#define GPIO_PIR_GPIO_Port GPIOC
#define SPI1_NSS_2_Pin GPIO_PIN_12
#define SPI1_NSS_2_GPIO_Port GPIOA
#define SPI1_NSS_Pin GPIO_PIN_15
#define SPI1_NSS_GPIO_Port GPIOA
#define GPIO_SWITCH3_Pin GPIO_PIN_6
#define GPIO_SWITCH3_GPIO_Port GPIOB
#define GPIO_SWITCH2_Pin GPIO_PIN_7
#define GPIO_SWITCH2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
