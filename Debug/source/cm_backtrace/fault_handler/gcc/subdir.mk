################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../source/cm_backtrace/fault_handler/gcc/cmb_fault.S 

OBJS += \
./source/cm_backtrace/fault_handler/gcc/cmb_fault.o 


# Each subdirectory must supply rules for building sources it contributes
source/cm_backtrace/fault_handler/gcc/%.o: ../source/cm_backtrace/fault_handler/gcc/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -D__NEWLIB__ -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\board" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\amazon-freertos\freertos_kernel\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\amazon-freertos\freertos_kernel\portable\GCC\ARM_CM4F" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\drivers" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\device" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\CMSIS" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\xip" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\utilities" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\serial_manager" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\lists" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\uart" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\littlefs" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source\fsl_ram_disk" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source\fsl_sd_disk" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\sdmmc\inc" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\CMSIS\NN\Include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\sdmmc\port" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\phy" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\osa" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\class" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\ehci" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\khci" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\lpcip3511" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\source\generated" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\tensorflow-lite" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\tensorflow-lite\third_party\flatbuffers\include" -g3 -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


