/* stm32f4xx_hal_timebase.c — 用 TIM3 作为 HAL 时基（中文注释） */
#include "main.h"
TIM_HandleTypeDef htim3;

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
    RCC_ClkInitTypeDef clk = {0};
    uint32_t lat = 0, timclk, psc;
    HAL_RCC_GetClockConfig(&clk, &lat);
    timclk = HAL_RCC_GetPCLK1Freq();
    if (clk.APB1CLKDivider != RCC_HCLK_DIV1) timclk *= 2U;   /* APB1 分频≠1 时定时器时钟×2 */
    psc = (timclk / 1000000U) - 1U;                            /* 1MHz 计数 */
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = psc;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 999;                                   /* 1ms 中断 */
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK) return HAL_ERROR;
    if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK) return HAL_ERROR;
    HAL_NVIC_SetPriority(TIM3_IRQn, TickPriority, 0U);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
    uwTickFreq = HAL_TICK_FREQ_DEFAULT;
    return HAL_OK;
}
void HAL_SuspendTick(void)   { __HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE); }
void HAL_ResumeTick(void)    { __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE); }
void TIM3_IRQHandler(void)   { HAL_TIM_IRQHandler(&htim3); }
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM3) HAL_IncTick();
}
