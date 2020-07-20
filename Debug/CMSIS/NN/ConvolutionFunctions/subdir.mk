################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/NN/ConvolutionFunctions/arm_convolve_1x1_HWC_q7_fast_nonsquare.c \
../CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q15_basic.c \
../CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q15_fast.c \
../CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q15_fast_nonsquare.c \
../CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_RGB.c \
../CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_basic.c \
../CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_basic_nonsquare.c \
../CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_fast.c \
../CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_fast_nonsquare.c \
../CMSIS/NN/ConvolutionFunctions/arm_depthwise_conv_u8_basic_ver1.c \
../CMSIS/NN/ConvolutionFunctions/arm_depthwise_separable_conv_HWC_q7.c \
../CMSIS/NN/ConvolutionFunctions/arm_depthwise_separable_conv_HWC_q7_nonsquare.c \
../CMSIS/NN/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15.c \
../CMSIS/NN/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15_reordered.c 

OBJS += \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_1x1_HWC_q7_fast_nonsquare.o \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q15_basic.o \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q15_fast.o \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q15_fast_nonsquare.o \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_RGB.o \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_basic.o \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_basic_nonsquare.o \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_fast.o \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_fast_nonsquare.o \
./CMSIS/NN/ConvolutionFunctions/arm_depthwise_conv_u8_basic_ver1.o \
./CMSIS/NN/ConvolutionFunctions/arm_depthwise_separable_conv_HWC_q7.o \
./CMSIS/NN/ConvolutionFunctions/arm_depthwise_separable_conv_HWC_q7_nonsquare.o \
./CMSIS/NN/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15.o \
./CMSIS/NN/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15_reordered.o 

C_DEPS += \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_1x1_HWC_q7_fast_nonsquare.d \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q15_basic.d \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q15_fast.d \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q15_fast_nonsquare.d \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_RGB.d \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_basic.d \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_basic_nonsquare.d \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_fast.d \
./CMSIS/NN/ConvolutionFunctions/arm_convolve_HWC_q7_fast_nonsquare.d \
./CMSIS/NN/ConvolutionFunctions/arm_depthwise_conv_u8_basic_ver1.d \
./CMSIS/NN/ConvolutionFunctions/arm_depthwise_separable_conv_HWC_q7.d \
./CMSIS/NN/ConvolutionFunctions/arm_depthwise_separable_conv_HWC_q7_nonsquare.d \
./CMSIS/NN/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15.d \
./CMSIS/NN/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15_reordered.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/NN/ConvolutionFunctions/%.o: ../CMSIS/NN/ConvolutionFunctions/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MIMXRT1052CVL5B -DCPU_MIMXRT1052CVL5B_cm7 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DFSL_RTOS_FREE_RTOS -DSDK_OS_FREE_RTOS -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__NEWLIB__ -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\xip" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\drivers" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source\fsl_ram_disk" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\amazon-freertos\freertos_kernel\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\amazon-freertos\freertos_kernel\portable\GCC\ARM_CM4F" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\sdmmc\port" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source\fsl_sd_disk" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\phy" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\CMSIS" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\osa" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\CMSIS\NN\Include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\littlefs" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\serial_manager" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\lists" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\source\generated" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\sdmmc\inc" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\utilities" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\uart" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\device" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\class" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\ehci" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\khci" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\lpcip3511" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\tensorflow-lite" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\tensorflow-lite\third_party\flatbuffers\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\board" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\amazon-freertos\freertos_kernel\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\amazon-freertos\freertos_kernel\portable\GCC\ARM_CM4F" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\drivers" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\device" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\CMSIS" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\xip" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\utilities" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\serial_manager" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\lists" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\uart" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\littlefs" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source\fsl_ram_disk" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source\fsl_sd_disk" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\sdmmc\inc" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\CMSIS\NN\Include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\sdmmc\port" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\phy" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\osa" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\class" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\ehci" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\khci" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\lpcip3511" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\source\generated" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\tensorflow-lite" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\tensorflow-lite\third_party\flatbuffers\include" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


