################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include Middlewares/ST/STM32_USB_Device_Library/Core/Src/subdir.mk
-include Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/subdir.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Signal_Generator.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Signal_Generator.list \

OBJCOPY_HEX += \
Signal_Generator.hex \

OBJCOPY_BIN += \
Signal_Generator.bin \


# All Target
all: Signal_Generator.elf secondary-outputs

# Tool invocations
Signal_Generator.elf: $(OBJS) $(USER_OBJS) D:\Git_test\Firmware_F103c8t6_Stm32CubeIDE\STM32F103C8TX_FLASH.ld
	arm-none-eabi-gcc -o "Signal_Generator.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"D:\Git_test\Firmware_F103c8t6_Stm32CubeIDE\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Signal_Generator.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Signal_Generator.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Signal_Generator.list"
	@echo 'Finished building: $@'
	@echo ' '

Signal_Generator.hex: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "Signal_Generator.hex"
	@echo 'Finished building: $@'
	@echo ' '

Signal_Generator.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Signal_Generator.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
