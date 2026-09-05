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


BUILD_ARTIFACT_NAME := Application
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: Application.axf

# Tool invocations
Application.axf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -T "Application.ld" -Xlinker --gc-sections -Xlinker -Map="Application.map" -mfpu=fpv5-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o Application.axf "./src/app_breathing_led.o" "./src/app_toggle_led.o" "./src/em_timer.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building out file: Application.out'
	arm-none-eabi-objcopy "Application.axf" "Application.out"
	@echo ' '

	@echo 'Building hex file: Application.hex'
	arm-none-eabi-objcopy -O ihex "Application.axf" "Application.hex"
	@echo ' '

	@echo 'Building bin file: Application.bin'
	arm-none-eabi-objcopy -O binary "Application.axf" "Application.bin"
	@echo ' '

	@echo 'Building s37 file: Application.s37'
	arm-none-eabi-objcopy -O srec "Application.axf" "Application.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "Application.axf" -A
	@echo ' '

# Other Targets
clean:
	-@$(foreach FILE, $(EXECUTABLES), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(OBJS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(C_DEPS), $(shell $(RM) $(FILE)) )
	-$(RM) Application.axf
	-@echo ' '

.PHONY: all clean dependents main-build

-include ../makefile.targets
