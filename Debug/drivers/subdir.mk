################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_adc.c \
../drivers/fsl_adc_etc.c \
../drivers/fsl_cache.c \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_csi.c \
../drivers/fsl_dmamux.c \
../drivers/fsl_edma.c \
../drivers/fsl_enc.c \
../drivers/fsl_flexspi.c \
../drivers/fsl_gpio.c \
../drivers/fsl_gpt.c \
../drivers/fsl_lpi2c.c \
../drivers/fsl_lpi2c_edma.c \
../drivers/fsl_lpspi.c \
../drivers/fsl_lpspi_edma.c \
../drivers/fsl_lpuart.c \
../drivers/fsl_lpuart_edma.c \
../drivers/fsl_pit.c \
../drivers/fsl_pwm.c \
../drivers/fsl_usdhc.c \
../drivers/fsl_xbara.c \
../drivers/fsl_xbarb.c 

OBJS += \
./drivers/fsl_adc.o \
./drivers/fsl_adc_etc.o \
./drivers/fsl_cache.o \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_csi.o \
./drivers/fsl_dmamux.o \
./drivers/fsl_edma.o \
./drivers/fsl_enc.o \
./drivers/fsl_flexspi.o \
./drivers/fsl_gpio.o \
./drivers/fsl_gpt.o \
./drivers/fsl_lpi2c.o \
./drivers/fsl_lpi2c_edma.o \
./drivers/fsl_lpspi.o \
./drivers/fsl_lpspi_edma.o \
./drivers/fsl_lpuart.o \
./drivers/fsl_lpuart_edma.o \
./drivers/fsl_pit.o \
./drivers/fsl_pwm.o \
./drivers/fsl_usdhc.o \
./drivers/fsl_xbara.o \
./drivers/fsl_xbarb.o 

C_DEPS += \
./drivers/fsl_adc.d \
./drivers/fsl_adc_etc.d \
./drivers/fsl_cache.d \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_csi.d \
./drivers/fsl_dmamux.d \
./drivers/fsl_edma.d \
./drivers/fsl_enc.d \
./drivers/fsl_flexspi.d \
./drivers/fsl_gpio.d \
./drivers/fsl_gpt.d \
./drivers/fsl_lpi2c.d \
./drivers/fsl_lpi2c_edma.d \
./drivers/fsl_lpspi.d \
./drivers/fsl_lpspi_edma.d \
./drivers/fsl_lpuart.d \
./drivers/fsl_lpuart_edma.d \
./drivers/fsl_pit.d \
./drivers/fsl_pwm.d \
./drivers/fsl_usdhc.d \
./drivers/fsl_xbara.d \
./drivers/fsl_xbarb.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MIMXRT1052CVL5B -DXIP_BOOT_HEADER_DCD_ENABLE=1 -DXIP_BOOT_HEADER_ENABLE=1 -DSKIP_SYSCLK_INIT -DCPU_MIMXRT1052CVL5B_cm7 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DFSL_RTOS_FREE_RTOS -DSDK_OS_FREE_RTOS -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__NEWLIB__ -DXIP_EXTERNAL_FLASH=1 -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\xip" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\drivers" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source\fsl_ram_disk" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\amazon-freertos\freertos_kernel\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\amazon-freertos\freertos_kernel\portable\GCC\ARM_CM4F" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\sdmmc\port" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source\fsl_sd_disk" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\phy" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\CMSIS" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\osa" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\CMSIS\NN\Include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\littlefs" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\serial_manager" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\lists" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\source\generated" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\sdmmc\inc" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\utilities" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\component\uart" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\device" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\class" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\ehci" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\khci" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\usb\device\source\lpcip3511" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\fatfs\source" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\tensorflow-lite" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\tensorflow-lite\third_party\flatbuffers\include" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU\board" -I"D:\Repos\MCUXpresso_workspace\AdventCirno_RT1052_NNCU" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


