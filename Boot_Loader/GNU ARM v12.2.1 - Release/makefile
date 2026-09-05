################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Boot_Loader
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: Boot_Loader.axf

# Tool invocations
Boot_Loader.axf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -T "Boot_Loader.ld" -Xlinker --gc-sections -Xlinker -Map="Boot_Loader.map" -mfpu=fpv5-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o Boot_Loader.axf "./src/btl_boot.o" "./src/btl_button.o" "./src/btl_crc32.o" "./src/btl_flash.o" "./src/btl_gpio.o" "./src/btl_jump.o" "./src/btl_led_pattern.o" "./src/btl_systick.o" "./src/btl_uart.o" "./src/btl_validate.o" "./src/btl_xmodem.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building out file: Boot_Loader.out'
	arm-none-eabi-objcopy "Boot_Loader.axf" "Boot_Loader.out"
	@echo ' '

	@echo 'Building hex file: Boot_Loader.hex'
	arm-none-eabi-objcopy -O ihex "Boot_Loader.axf" "Boot_Loader.hex"
	@echo ' '

	@echo 'Building bin file: Boot_Loader.bin'
	arm-none-eabi-objcopy -O binary "Boot_Loader.axf" "Boot_Loader.bin"
	@echo ' '

	@echo 'Building s37 file: Boot_Loader.s37'
	arm-none-eabi-objcopy -O srec "Boot_Loader.axf" "Boot_Loader.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "Boot_Loader.axf" -A
	@echo ' '

# Other Targets
clean:
	-@$(foreach FILE, $(EXECUTABLES), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(OBJS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(C_DEPS), $(shell $(RM) $(FILE)) )
	-$(RM) Boot_Loader.axf
	-@echo ' '

.PHONY: all clean dependents main-build

-include ../makefile.targets
