/* stm32f4xx_it.c — 中断入口（中文注释） */
#include "main.h"
void NMI_Handler(void)        { for (;;) { } }
void HardFault_Handler(void)  { for (;;) { } }
void MemManage_Handler(void)  { for (;;) { } }
void BusFault_Handler(void)   { for (;;) { } }
void UsageFault_Handler(void) { for (;;) { } }
void SVC_Handler(void)        { }   /* FreeRTOS 端口接管，见 startup 向量 */
void DebugMon_Handler(void)   { }
void PendSV_Handler(void)     { }   /* FreeRTOS 端口接管 */
void SysTick_Handler(void)    { }   /* FreeRTOS 端口接管 */
