/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "fsl_common.h"
#include "fsl_csi.h"
#include "fsl_enc.h"
#include "fsl_clock.h"
#include "fsl_lpi2c.h"
#include "fsl_lpuart.h"
#include "fsl_pwm.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/* Definitions for BOARD_InitPeripherals functional group */
/* Definition of peripheral ID */
#define CSI_PERIPHERAL CSI
/* Definition of the clock source frequency */
#define CSI_CLK_FREQ 150000000UL
/* Definition of the MCLK (master clock) clock source frequency */
#define CSI_MCLK_FREQ 24000000UL
/* Definition of the frame width (number of pixels in the line) */
#define CSI_FRAME_WIDTH 184
/* Definition of the frame height (number of lines) */
#define CSI_FRAME_HEIGHT 120
/* Definition of the bytes per pixel size */
#define CSI_BYTES_PER_PIXEL 2
/* Definition of the line pitch size in bytes */
#define CSI_LINE_PITCH_BYTES 368
/* Definition of number of the buffers inside allocated block */
#define CSI_FRAME_BUFFER_COUNT 4
/* Definition of the frame buffer block alignment */
#define CSI_BUFFER_ALIGN 64
/* Definition of peripheral ID */
#define ENC2_PERIPHERAL ENC2
/* Definition of peripheral ID */
#define ENC3_PERIPHERAL ENC3
/* BOARD_InitPeripherals defines for LPI2C1 */
/* Definition of peripheral ID */
#define LPI2C1_PERIPHERAL LPI2C1
/* Definition of clock source */
#define LPI2C1_CLOCK_FREQ 12000000UL
/* Transfer buffer size */
#define LPI2C1_MASTER_BUFFER_SIZE 1
/* Definition of peripheral ID */
#define LPUART1_PERIPHERAL LPUART1
/* Definition of the clock source frequency */
#define LPUART1_CLOCK_SOURCE 80000000UL
/* Definition of peripheral ID */
#define LPUART2_PERIPHERAL LPUART2
/* Definition of the clock source frequency */
#define LPUART2_CLOCK_SOURCE 80000000UL
/* Definition of peripheral ID */
#define LPUART4_PERIPHERAL LPUART4
/* Definition of the clock source frequency */
#define LPUART4_CLOCK_SOURCE 80000000UL
/* Definition of peripheral ID */
#define PWM2_PERIPHERAL PWM2
/* Definition of submodule 3 ID */
#define PWM2_SM0 kPWM_Module_3
/* Definition of submodule 3 counter (PWM) desired frequency in Hertz */
#define PWM2_SM0_COUNTER_FREQ_HZ 50U
/* Definition of submodule 3 counter clock source frequency in Hertz */
#define PWM2_SM0_COUNTER_CLK_SOURCE_FREQ_HZ 150000000U
/* Definition of submodule 3 channel A ID */
#define PWM2_SM0_A kPWM_PwmA
/* Definition of submodule 3 channel B ID */
#define PWM2_SM0_B kPWM_PwmB
/* Definition of submodule 3 channel X ID */
#define PWM2_SM0_X kPWM_PwmX
/* Definition of submodule 1 ID */
#define PWM2_SM1 kPWM_Module_1
/* Definition of submodule 1 counter (PWM) desired frequency in Hertz */
#define PWM2_SM1_COUNTER_FREQ_HZ 20000U
/* Definition of submodule 1 counter clock source frequency in Hertz */
#define PWM2_SM1_COUNTER_CLK_SOURCE_FREQ_HZ 150000000U
/* Definition of submodule 1 channel A ID */
#define PWM2_SM1_A kPWM_PwmA
/* Definition of submodule 1 channel B ID */
#define PWM2_SM1_B kPWM_PwmB
/* Definition of submodule 1 channel X ID */
#define PWM2_SM1_X kPWM_PwmX
/* Definition of submodule 0 ID */
#define PWM2_SM2 kPWM_Module_0
/* Definition of submodule 0 counter (PWM) desired frequency in Hertz */
#define PWM2_SM2_COUNTER_FREQ_HZ 20000U
/* Definition of submodule 0 counter clock source frequency in Hertz */
#define PWM2_SM2_COUNTER_CLK_SOURCE_FREQ_HZ 150000000U
/* Definition of submodule 0 channel A ID */
#define PWM2_SM2_A kPWM_PwmA
/* Definition of submodule 0 channel B ID */
#define PWM2_SM2_B kPWM_PwmB
/* Definition of submodule 0 channel X ID */
#define PWM2_SM2_X kPWM_PwmX
/* Definition of fault Fault0 ID */
#define PWM2_F0_FAULT0 kPWM_Fault_0
/* Definition of fault Fault1 ID */
#define PWM2_F0_FAULT1 kPWM_Fault_1
/* Definition of fault Fault2 ID */
#define PWM2_F0_FAULT2 kPWM_Fault_2
/* Definition of fault Fault3 ID */
#define PWM2_F0_FAULT3 kPWM_Fault_3

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
extern csi_config_t CSI_config;
/* Frame buffer block */
extern uint16_t CSI_Buffer[CSI_FRAME_BUFFER_COUNT][CSI_FRAME_HEIGHT][CSI_FRAME_WIDTH];
extern csi_handle_t CSI_handle;
extern enc_config_t ENC2_config;
extern enc_config_t ENC3_config;
extern const lpi2c_master_config_t LPI2C1_masterConfig;
extern lpi2c_master_transfer_t LPI2C1_masterTransfer;
extern uint8_t LPI2C1_masterBuffer[LPI2C1_MASTER_BUFFER_SIZE];
extern lpi2c_master_handle_t LPI2C1_masterHandle;
extern const lpuart_config_t LPUART1_config;
extern const lpuart_config_t LPUART2_config;
extern const lpuart_config_t LPUART4_config;
extern pwm_config_t PWM2_SM0_config;

extern pwm_signal_param_t PWM2_SM0_pwm_function_config[1];
extern pwm_config_t PWM2_SM1_config;

extern pwm_signal_param_t PWM2_SM1_pwm_function_config[2];
extern pwm_config_t PWM2_SM2_config;

extern pwm_signal_param_t PWM2_SM2_pwm_function_config[2];
extern const pwm_fault_input_filter_param_t PWM2_faultInputFilter_config;
extern const pwm_fault_param_t PWM2_Fault0_fault_config;
extern const pwm_fault_param_t PWM2_Fault1_fault_config;
extern const pwm_fault_param_t PWM2_Fault2_fault_config;
extern const pwm_fault_param_t PWM2_Fault3_fault_config;

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void);

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void);

#if defined(__cplusplus)
}
#endif

#endif /* _PERIPHERALS_H_ */
