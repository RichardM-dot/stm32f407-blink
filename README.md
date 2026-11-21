# stm32f407-blink — 裸机 FreeRTOS（STM32F407VGT6）

STM32F407VGT6（Cortex-M4F）+ HAL + FreeRTOS V11.1.0 工程：LED（PD12）以
500 ms 周期闪烁。使用 **make + 本地 ARM GNU 工具链** 构建，不依赖任何 IDE。

## 当前状态

| 项目 | 状态 |
| --- | --- |
| 编译 / 链接（elf/hex/bin） | 通过 |
| 向量表 RTOS 三入口重定向 | 已验证 |
| FreeRTOS 关键符号链接 | 已验证（SVC/PendSV/SysTick/pxCurrentTCBs 共 4 项） |
| 上板实测 | **尚未进行** |

## 关键配置

| 项 | 值 |
| --- | --- |
| 芯片 | STM32F407VGT6（1024K Flash / 128K RAM + 64K CCM） |
| 内核 | Cortex-M4F（`-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard`） |
| 时钟 | HSI 16 MHz ÷16 × PLLN336 ÷ PLLP2 = **168 MHz**，APB1 ÷4 = 42 MHz，Flash 5 WS |
| LED | **PD12** 推挽输出（Discovery-F407 板载绿灯） |
| RTOS 端口 | `portable/GCC/ARM_CM4F`（V11.1.0，硬浮点上下文） |
| FreeRTOS 堆 | 32 KB heap_4 |
| HAL 时基 | TIM3 @ 1 kHz（SysTick 归 FreeRTOS） |
| 编译宏 | `-DUSE_HAL_DRIVER -DSTM32F407xx` |

## 工程结构

```
stm32f407-blink/
├── Makefile
├── STM32F407VGTx_FLASH.ld    # 手写链接脚本（1M Flash / 128K SRAM / 64K CCM）
├── startup_stm32f407xx.s     # 向量表 3 项已指向 FreeRTOS 端口
├── Config/
│   ├── FreeRTOSConfig.h      # 堆 32KB / CPU 168MHz / tick 1kHz
│   └── stm32f4xx_hal_timebase.c
├── Core/{Inc,Src}/
├── Drivers/                  # CMSIS + F4 HAL 驱动（vendored，自包含）
├── Middlewares/Third_Party/FreeRTOS/   # 内核源码 + ARM_CM4F 端口 + heap_4
└── build/
```

## 构建 / 自检

```bash
make            # 产出 build/stm32f407-blink.{elf,hex,bin} + size 汇总
make size / dis / symbols / clean
```

### 逆向自检（已验证的参考值）

```bash
TOOL=/Users/theo-richard/workspace/toolchains/arm-none-eabi/bin/arm-none-eabi-

xxd -l 8 build/stm32f407-blink.bin
# 期望：00000220 e9260008   (SP=0x20020000, Reset=0x080026E9)

$TOOL objdump -s -j .isr_vector build/stm32f407-blink.elf | head -8   # 0x2C/0x38/0x3C 槽位
$TOOL nm build/stm32f407-blink.elf | grep -Eic 'vPortSVCHandler|xPortPendSVHandler|xPortSysTickHandler|pxCurrentTCB'
# 应为 4
```

当前固件尺寸：**text 11032 B + data 24 B**，**bss 35320 B**（含 32 KB RTOS 堆）。

> `_write ... will always fail` 与 `LOAD segment with RWX` 为无害警告，见
> 工作区根目录《说明文档》9.1 节。

## 烧录

```bash
st-flash write build/stm32f407-blink.bin 0x08000000
# 或
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "program build/stm32f407-blink.elf verify reset exit"
```

上板后 PD12 LED 以 500 ms 周期闪烁。

## 扩展指南

- 时钟改用外部 HSE：改 `Core/Src/main.c` 的 `SystemClock_Config`（PLLM/PLLN/PLLP）
- FPU 已启用（hard ABI）； FreeRTOS `ARM_CM4F` 端口会在上下文切换时惰性保存 FPU 寄存器
- 新增 HAL 模块 / FromISR 中断优先级规则与 f411 模板一致（优先级数值 ≥ 5）
