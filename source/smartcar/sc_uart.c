//
// Created by 17616 on 2020/3/23.
//

#include "sc_uart.h"

#include "ac_lib/Image.h"

#define NUM_VAR 6
float Variable[NUM_VAR]; //发送缓存数组
extern uint8_t image_Buffer_0[CAMERA_H][CAMERA_W];

status_t UART_Init(LPUART_Type *base, uint32_t baudRate_Bps_,
                   uint32_t srcClock_Hz) {
    lpuart_config_t config;
    /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kLPUART_ParityDisabled;
     * config.stopBitCount = kLPUART_OneStopBit;
     * config.txFifoWatermark = 0;
     * config.rxFifoWatermark = 0;
     * config.enableTx = false;
     * config.enableRx = false;
     */
    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = baudRate_Bps_;
    config.enableTx = true;
    config.enableRx = true;

    /* 设置奇偶校验 */
    config.parityMode = kLPUART_ParityDisabled;

    /* 设置停止位 */
    config.stopBitCount = kLPUART_OneStopBit;

    /* 设置数据位宽 */
    config.dataBitsCount = kLPUART_EightDataBits;

    return LPUART_Init(base, &config, srcClock_Hz);
}

/**
 * @brief    发送一个字符
 * @param    base  ： LPUART1 - LPUART8
 * @param    data  ： 发送的字符
 * @return
 * @example  UART_PutChar(LPUART1, char);  //串口1  发送char
 */
void UART_PutChar(LPUART_Type *base, uint8_t data) {
    /* 等待base->STAT为空 */
    while (!(base->STAT & LPUART_STAT_TDRE_MASK))
        ;

    base->DATA = data;
}
