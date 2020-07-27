################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include xip/subdir.mk
-include utilities/subdir.mk
-include usb/phy/subdir.mk
-include usb/device/source/lpcip3511/subdir.mk
-include usb/device/source/khci/subdir.mk
-include usb/device/source/ehci/subdir.mk
-include usb/device/source/subdir.mk
-include usb/device/class/subdir.mk
-include startup/subdir.mk
-include source/smartcar/subdir.mk
-include source/nncu/Network/Dense_Simple_Plus/subdir.mk
-include source/nncu/subdir.mk
-include source/cm_backtrace/fault_handler/gcc/subdir.mk
-include source/cm_backtrace/subdir.mk
-include source/ac_lib/subdir.mk
-include source/subdir.mk
-include sdmmc/src/subdir.mk
-include sdmmc/port/subdir.mk
-include littlefs/subdir.mk
-include fatfs/source/fsl_sd_disk/subdir.mk
-include fatfs/source/fsl_ram_disk/subdir.mk
-include fatfs/source/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include component/serial_manager/subdir.mk
-include component/osa/subdir.mk
-include component/lists/subdir.mk
-include board/subdir.mk
-include amazon-freertos/freertos_kernel/portable/MemMang/subdir.mk
-include amazon-freertos/freertos_kernel/portable/GCC/ARM_CM4F/subdir.mk
-include amazon-freertos/freertos_kernel/subdir.mk
-include CMSIS/NN/SoftmaxFunctions/subdir.mk
-include CMSIS/NN/PoolingFunctions/subdir.mk
-include CMSIS/NN/NNSupportFunctions/subdir.mk
-include CMSIS/NN/FullyConnectedFunctions/subdir.mk
-include CMSIS/NN/ConvolutionFunctions/subdir.mk
-include CMSIS/NN/ActivationFunctions/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: AdventCirno_RT1052_NNCU.axf

# Tool invocations
AdventCirno_RT1052_NNCU.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\libs" -Xlinker -Map="AdventCirno_RT1052_NNCU.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -T AdventCirno_RT1052_NNCU_Debug.ld -o "AdventCirno_RT1052_NNCU.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) AdventCirno_RT1052_NNCU.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "AdventCirno_RT1052_NNCU.axf"; # arm-none-eabi-objcopy -v -O binary "AdventCirno_RT1052_NNCU.axf" "AdventCirno_RT1052_NNCU.bin" ; # checksum -p MIMXRT1052xxxxB -d "AdventCirno_RT1052_NNCU.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
