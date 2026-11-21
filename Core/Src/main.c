/* main.c — STM32F407xx 裸机 + FreeRTOS 示例（中文注释）
 * 时钟: HSI/16*336/2=168MHz 5WS
 * LED : PD12
 */
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void led_task(void *arg);

int main(void) {
    HAL_Init();                 /* HAL 初始化（含 TIM3 时基） */
    SystemClock_Config();       /* 芯片时钟配置 */
    MX_GPIO_Init();             /* LED GPIO 初始化 */
    xTaskCreate(led_task, "led", 256, NULL, 2, NULL);
    vTaskStartScheduler();      /* 启动 FreeRTOS 调度器 */
    for (;;) { }                /* 调度器异常返回时死循环 */
}

/* LED 翻转任务：500ms 周期 */
static void led_task(void *arg) {
    (void)arg;
    for (;;) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* 时钟配置：HSI/16*336/2=168MHz 5WS */
static void SystemClock_Config(void) {
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState = RCC_HSI_ON;
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    osc.PLL.PLLM = 16;                        /* 16MHz/16 = 1MHz */
    osc.PLL.PLLN = 336;                       /* 1MHz*336 = 336MHz */
    osc.PLL.PLLP = RCC_PLLP_DIV2;             /* 168MHz */
    osc.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&osc) != HAL_OK) Error_Handler();
    clk.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV4;       /* 42MHz */
    clk.APB2CLKDivider = RCC_HCLK_DIV2;       /* 84MHz */
    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_5) != HAL_OK) Error_Handler();
}

/* LED GPIO 初始化：PD12 */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef g = {0};
    __HAL_RCC_GPIOD_CLK_ENABLE();
    g.Pin   = LED_Pin;
    g.Mode  = GPIO_MODE_OUTPUT_PP;
    g.Pull  = GPIO_NOPULL;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_Port, &g);
}

void Error_Handler(void) { __disable_irq(); for (;;) { } }


/* ===== 钩子兜底空实现（中文注释）=====
 * 若 FreeRTOSConfig.h 打开 configCHECK_FOR_STACK_OVERFLOW 或
 * configUSE_MALLOC_FAILED_HOOK，Kernel 会引用以下符号；此处提供空实现，
 * 保证任何配置下都能链接通过。实际产品可改为记录日志/复位。
 */
void vApplicationStackOverflowHook(void *xTask, char *pcTaskName) {
    (void)xTask; (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    for (;;) { }
}
void vApplicationMallocFailedHook(void) {
    taskDISABLE_INTERRUPTS();
    for (;;) { }
}
