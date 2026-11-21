/* main.h — 公共声明（中文注释） */
#ifndef MAIN_H
#define MAIN_H
#include "stm32f4xx_hal.h"
extern void Error_Handler(void);
#define LED_Pin       GPIO_PIN_12
#define LED_GPIO_Port GPIOD
#endif
