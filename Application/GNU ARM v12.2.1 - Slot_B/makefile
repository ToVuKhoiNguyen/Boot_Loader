################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include simplicity_sdk_2025.6.3/platform/service/sl_main/src/subdir.mk
-include simplicity_sdk_2025.6.3/platform/service/memory_manager/src/subdir.mk
-include simplicity_sdk_2025.6.3/platform/service/interrupt_manager/src/subdir.mk
-include simplicity_sdk_2025.6.3/platform/service/device_manager/src/subdir.mk
-include simplicity_sdk_2025.6.3/platform/service/device_manager/clocks/subdir.mk
-include simplicity_sdk_2025.6.3/platform/service/device_init/src/subdir.mk
-include simplicity_sdk_2025.6.3/platform/service/clock_manager/src/subdir.mk
-include simplicity_sdk_2025.6.3/platform/peripheral/src/subdir.mk
-include simplicity_sdk_2025.6.3/platform/emlib/src/subdir.mk
-include simplicity_sdk_2025.6.3/platform/driver/gpio/src/subdir.mk
-include simplicity_sdk_2025.6.3/platform/common/src/subdir.mk
-include simplicity_sdk_2025.6.3/platform/Device/SiliconLabs/BGM22/Source/subdir.mk
-include simplicity_sdk_2025.6.3/hardware/board/src/subdir.mk
-include autogen/subdir.mk
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


BUILD_ARTIFACT_NAME := ${Slot_B}
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX := raw_b
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: raw_b${Slot_B}.axf

# Tool invocations
raw_b${Slot_B}.axf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -T "C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\linker_slot_b.ld" -fno-lto -Wl,--no-warn-rwx-segments -Xlinker --gc-sections -Xlinker -Map="raw_b.map" -mfpu=fpv5-sp-d16 -mfloat-abi=hard --specs=nano.specs -o raw_b.axf -Wl,--start-group "./app.o" "./main.o" "./autogen/sl_board_default_init.o" "./autogen/sl_event_handler.o" "./simplicity_sdk_2025.6.3/hardware/board/src/sl_board_control_gpio.o" "./simplicity_sdk_2025.6.3/hardware/board/src/sl_board_init.o" "./simplicity_sdk_2025.6.3/platform/Device/SiliconLabs/BGM22/Source/startup_bgm22.o" "./simplicity_sdk_2025.6.3/platform/Device/SiliconLabs/BGM22/Source/system_bgm22.o" "./simplicity_sdk_2025.6.3/platform/common/src/sl_assert.o" "./simplicity_sdk_2025.6.3/platform/common/src/sl_core_cortexm.o" "./simplicity_sdk_2025.6.3/platform/common/src/sl_syscalls.o" "./simplicity_sdk_2025.6.3/platform/driver/gpio/src/sl_gpio.o" "./simplicity_sdk_2025.6.3/platform/emlib/src/em_cmu.o" "./simplicity_sdk_2025.6.3/platform/emlib/src/em_emu.o" "./simplicity_sdk_2025.6.3/platform/emlib/src/em_gpio.o" "./simplicity_sdk_2025.6.3/platform/emlib/src/em_msc.o" "./simplicity_sdk_2025.6.3/platform/emlib/src/em_system.o" "./simplicity_sdk_2025.6.3/platform/emlib/src/em_timer.o" "./simplicity_sdk_2025.6.3/platform/peripheral/src/sl_hal_gpio.o" "./simplicity_sdk_2025.6.3/platform/service/clock_manager/src/sl_clock_manager.o" "./simplicity_sdk_2025.6.3/platform/service/clock_manager/src/sl_clock_manager_hal_s2.o" "./simplicity_sdk_2025.6.3/platform/service/clock_manager/src/sl_clock_manager_init.o" "./simplicity_sdk_2025.6.3/platform/service/clock_manager/src/sl_clock_manager_init_hal_s2.o" "./simplicity_sdk_2025.6.3/platform/service/device_init/src/sl_device_init_dcdc_s2.o" "./simplicity_sdk_2025.6.3/platform/service/device_init/src/sl_device_init_emu_s2.o" "./simplicity_sdk_2025.6.3/platform/service/device_manager/clocks/sl_device_clock_efr32xg22.o" "./simplicity_sdk_2025.6.3/platform/service/device_manager/src/sl_device_clock.o" "./simplicity_sdk_2025.6.3/platform/service/device_manager/src/sl_device_gpio.o" "./simplicity_sdk_2025.6.3/platform/service/interrupt_manager/src/sl_interrupt_manager_cortexm.o" "./simplicity_sdk_2025.6.3/platform/service/memory_manager/src/sl_memory_manager_region.o" "./simplicity_sdk_2025.6.3/platform/service/sl_main/src/sl_main_init.o" "./simplicity_sdk_2025.6.3/platform/service/sl_main/src/sl_main_init_memory.o" "./simplicity_sdk_2025.6.3/platform/service/sl_main/src/sl_main_process_action.o" "./src/app_2.o" -lgcc -lc -lm -lnosys -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building out file: raw_b${Slot_B}.out'
	arm-none-eabi-objcopy "raw_b${Slot_B}.axf" "raw_b${Slot_B}.out"
	@echo ' '

	@echo 'Building hex file: raw_b${Slot_B}.hex'
	arm-none-eabi-objcopy -O ihex "raw_b${Slot_B}.axf" "raw_b${Slot_B}.hex"
	@echo ' '

	@echo 'Building bin file: raw_b${Slot_B}.bin'
	arm-none-eabi-objcopy -O binary "raw_b${Slot_B}.axf" "raw_b${Slot_B}.bin"
	@echo ' '

	@echo 'Building s37 file: raw_b${Slot_B}.s37'
	arm-none-eabi-objcopy -O srec "raw_b${Slot_B}.axf" "raw_b${Slot_B}.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "raw_b${Slot_B}.axf" -A
	@echo ' '

# Other Targets
clean:
	-@$(foreach FILE, $(EXECUTABLES), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(OBJS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(C_DEPS), $(shell $(RM) $(FILE)) )
	-$(RM) raw_b${Slot_B}.axf
	-@echo ' '

.PHONY: all clean dependents main-build

-include ../makefile.targets
