# Makefile — stm32f407-blink（STM32F407xx / Cortex-M4F，中文注释）
TOOLCHAIN := /Users/theo-richard/workspace/toolchains/arm-none-eabi/bin/arm-none-eabi-
CC      := $(TOOLCHAIN)gcc
AS      := $(TOOLCHAIN)gcc -x assembler-with-cpp
OBJCOPY := $(TOOLCHAIN)objcopy
OBJDUMP := $(TOOLCHAIN)objdump
NM      := $(TOOLCHAIN)nm
SIZE    := $(TOOLCHAIN)size

TARGET  := stm32f407-blink
BUILD   := build

CPU     := -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
DEFS    := -DUSE_HAL_DRIVER -DSTM32F407xx
INC     := -ICore/Inc -IConfig \
           -IDrivers/CMSIS/Device/ST/STM32F4xx/Include \
           -IDrivers/CMSIS/Include \
           -IDrivers/STM32F4xx_HAL_Driver/Inc \
           -IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy \
           -IMiddlewares/Third_Party/FreeRTOS/Source/include \
           -IMiddlewares/Third_Party/FreeRTOS/portable/GCC/ARM_CM4F \
           -IMiddlewares/Third_Party/FreeRTOS/portable/MemMang

CFLAGS  := $(CPU) $(DEFS) $(INC) -O2 -g -Wall -ffunction-sections -fdata-sections -fno-common
ASFLAGS := $(CPU) $(DEFS) $(INC) -g
LDFLAGS := $(CPU) -TSTM32F407VGTx_FLASH.ld -Wl,--gc-sections -Wl,-Map=$(BUILD)/$(TARGET).map -specs=nano.specs -specs=nosys.specs

C_SOURCES := $(wildcard Core/Src/*.c) \
             $(filter-out %_template.c, $(wildcard Drivers/STM32F4xx_HAL_Driver/Src/*.c)) \
             $(wildcard Middlewares/Third_Party/FreeRTOS/Source/*.c) \
             Middlewares/Third_Party/FreeRTOS/portable/GCC/ARM_CM4F/port.c \
             Middlewares/Third_Party/FreeRTOS/portable/MemMang/heap_4.c \
             Config/stm32f4xx_hal_timebase.c
ASM_SOURCES := startup_stm32f407xx.s

OBJECTS := $(addprefix $(BUILD)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).hex $(BUILD)/$(TARGET).bin
$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) -c $(CFLAGS) $< -o $@
$(BUILD)/%.o: %.s | $(BUILD)
	$(AS) -c $(ASFLAGS) $< -o $@
$(BUILD)/$(TARGET).elf: $(OBJECTS) | $(BUILD)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
$(BUILD)/%.hex: $(BUILD)/%.elf
	$(OBJCOPY) -O ihex $< $@
$(BUILD)/%.bin: $(BUILD)/%.elf
	$(OBJCOPY) -O binary -S $< $@
$(BUILD):
	mkdir -p $@
dis: $(BUILD)/$(TARGET).elf
	$(OBJDUMP) -d $< > $(BUILD)/$(TARGET).dis
symbols: $(BUILD)/$(TARGET).elf
	$(NM) $< > $(BUILD)/$(TARGET).sym
size: $(BUILD)/$(TARGET).elf
	$(SIZE) $<
ccdb: $(BUILD)/$(TARGET).elf
	$(OBJDUMP) -S $< > $(BUILD)/$(TARGET).ccdb
clean:
	rm -rf $(BUILD)
.PHONY: all dis symbols size ccdb clean
