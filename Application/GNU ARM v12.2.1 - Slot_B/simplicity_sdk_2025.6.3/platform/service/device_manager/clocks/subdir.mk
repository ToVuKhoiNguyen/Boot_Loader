################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../simplicity_sdk_2025.6.3/platform/service/device_manager/clocks/sl_device_clock_efr32xg22.c 

OBJS += \
./simplicity_sdk_2025.6.3/platform/service/device_manager/clocks/sl_device_clock_efr32xg22.o 

C_DEPS += \
./simplicity_sdk_2025.6.3/platform/service/device_manager/clocks/sl_device_clock_efr32xg22.d 


# Each subdirectory must supply rules for building sources it contributes
simplicity_sdk_2025.6.3/platform/service/device_manager/clocks/sl_device_clock_efr32xg22.o: ../simplicity_sdk_2025.6.3/platform/service/device_manager/clocks/sl_device_clock_efr32xg22.c simplicity_sdk_2025.6.3/platform/service/device_manager/clocks/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c18 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DBGM220PC22HNA=1' '-DSL_CODE_COMPONENT_SYSTEM=system' '-DHARDWARE_BOARD_DEFAULT_RF_BAND_2400=1' '-DHARDWARE_BOARD_SUPPORTS_1_RF_BAND=1' '-DHARDWARE_BOARD_SUPPORTS_RF_BAND_2400=1' '-DSL_BOARD_NAME="BRD4314A"' '-DSL_BOARD_REV="A02"' '-DSL_CODE_COMPONENT_CLOCK_MANAGER=clock_manager' '-DSL_COMPONENT_CATALOG_PRESENT=1' '-DSL_CODE_COMPONENT_GPIO=gpio' '-DSL_CODE_COMPONENT_HAL_COMMON=hal_common' '-DSL_CODE_COMPONENT_HAL_GPIO=hal_gpio' '-DSL_CODE_COMPONENT_INTERRUPT_MANAGER=interrupt_manager' '-DCMSIS_NVIC_VIRTUAL=1' '-DCMSIS_NVIC_VIRTUAL_HEADER_FILE="cmsis_nvic_virtual.h"' '-DSL_CODE_COMPONENT_CORE=core' -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\config" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\autogen" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\Device\SiliconLabs\BGM22\Include" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\hardware\board\inc" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\service\clock_manager\inc" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\service\clock_manager\src" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\CMSIS\Core\Include" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\common\inc" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\service\device_manager\inc" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\service\device_init\inc" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\emlib\inc" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\driver\gpio\inc" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\peripheral\inc" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\service\interrupt_manager\inc" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\service\interrupt_manager\src" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\service\interrupt_manager\inc\arm" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\service\memory_manager\inc" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\service\sl_main\inc" -I"C:\Users\Admin\SimplicityStudio\v5_workspace\Mock_Project\Application\simplicity_sdk_2025.6.3\platform\service\sl_main\src" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -mcmse -mfpu=fpv5-sp-d16 -mfloat-abi=hard -fno-lto --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"simplicity_sdk_2025.6.3/platform/service/device_manager/clocks/sl_device_clock_efr32xg22.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


