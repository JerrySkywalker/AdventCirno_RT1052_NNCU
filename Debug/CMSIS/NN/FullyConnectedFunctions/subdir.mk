################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15.c \
../CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15_opt.c \
../CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_q15.c \
../CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_q15_opt.c \
../CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_q7.c \
../CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_q7_opt.c 

OBJS += \
./CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15.o \
./CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15_opt.o \
./CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_q15.o \
./CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_q15_opt.o \
./CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_q7.o \
./CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_q7_opt.o 

C_DEPS += \
./CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15.d \
./CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15_opt.d \
./CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_q15.d \
./CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_q15_opt.d \
./CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_q7.d \
./CMSIS/NN/FullyConnectedFunctions/arm_fully_connected_q7_opt.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/NN/FullyConnectedFunctions/%.o: ../CMSIS/NN/FullyConnectedFunctions/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MIMXRT1052CVL5B -DXIP_BOOT_HEADER_DCD_ENABLE=1 -DXIP_BOOT_HEADER_ENABLE=1 -DSKIP_SYSCLK_INIT -DCPU_MIMXRT1052CVL5B_cm7 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DFSL_RTOS_FREE_RTOS -DSDK_OS_FREE_RTOS -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__NEWLIB__ -DXIP_EXTERNAL_FLASH=1 -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\xip" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\drivers" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source\fsl_ram_disk" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\amazon-freertos\freertos_kernel\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\amazon-freertos\freertos_kernel\portable\GCC\ARM_CM4F" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\sdmmc\port" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source\fsl_sd_disk" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\phy" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\CMSIS" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\osa" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\CMSIS\NN\Include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\littlefs" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\serial_manager" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\lists" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\source\generated" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\sdmmc\inc" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\utilities" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\uart" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\device" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\class" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\ehci" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\khci" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\lpcip3511" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\tensorflow-lite" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\tensorflow-lite\third_party\flatbuffers\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\board" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\amazon-freertos\freertos_kernel\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\amazon-freertos\freertos_kernel\portable\GCC\ARM_CM4F" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\drivers" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\device" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\CMSIS" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\xip" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\utilities" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\serial_manager" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\lists" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\uart" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\littlefs" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source\fsl_ram_disk" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source\fsl_sd_disk" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\sdmmc\inc" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\CMSIS\NN\Include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\sdmmc\port" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\phy" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\osa" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\class" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\ehci" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\khci" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\lpcip3511" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\source\generated" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\tensorflow-lite" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\tensorflow-lite\third_party\flatbuffers\include" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


