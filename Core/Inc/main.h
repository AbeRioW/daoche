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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_WHITE   0xFFFF
#define COLOR_BLACK   0x0000
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern uint8_t dis_band;
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
#define BEEP_Pin GPIO_PIN_13
#define BEEP_GPIO_Port GPIOC
#define Trig_Pin GPIO_PIN_10
#define Trig_GPIO_Port GPIOE
#define KEY2_Pin GPIO_PIN_7
#define KEY2_GPIO_Port GPIOG
#define KEY2_EXTI_IRQn EXTI9_5_IRQn
#define KEY3_Pin GPIO_PIN_8
#define KEY3_GPIO_Port GPIOG
#define KEY3_EXTI_IRQn EXTI9_5_IRQn
#define SPI_CS_Pin GPIO_PIN_0
#define SPI_CS_GPIO_Port GPIOD
#define LCD_DC_Pin GPIO_PIN_1
#define LCD_DC_GPIO_Port GPIOD
#define LCD_RST_Pin GPIO_PIN_2
#define LCD_RST_GPIO_Port GPIOD
#define KEY1_Pin GPIO_PIN_6
#define KEY1_GPIO_Port GPIOD
#define KEY1_EXTI_IRQn EXTI9_5_IRQn
#define OV2640_PWDN_Pin GPIO_PIN_9
#define OV2640_PWDN_GPIO_Port GPIOG
#define OV2640_RST_Pin GPIO_PIN_15
#define OV2640_RST_GPIO_Port GPIOG
#define OV2640_SCL_Pin GPIO_PIN_8
#define OV2640_SCL_GPIO_Port GPIOB
#define OV2640_SDA_Pin GPIO_PIN_9
#define OV2640_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
