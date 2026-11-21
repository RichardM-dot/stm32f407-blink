/* stm32f4xx_hal_msp.c — HAL MSP 初始化（中文注释） */
#include "main.h"
void HAL_MspInit(void) {
    __HAL_RCC_SYSCFG_CLK_ENABLE();
}
