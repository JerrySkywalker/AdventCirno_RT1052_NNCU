//
// Created by 17616 on 2020/3/23.
//

#include "sc_i2c.h"

/****************************************************
 * @note: 常用的通用API为：
 *      I2C_Init    :       初始化函数
 *      I2C_Read    :       读函数           
 *      I2C_Write   :       写函数
 * 
 *      其他的不是通用函数
 * ******************************************************/

status_t I2C_Init(LPI2C_Type *base, uint32_t baudRate_Bps, uint32_t sourceClock_Hz) 
{
    lpi2c_master_config_t masterConfig;
    
    LPI2C_MasterGetDefaultConfig(&masterConfig);
    
    /*
     * i2cConfig.debugEnable = false;
     * i2cConfig.ignoreAck = false;
     * i2cConfig.pinConfig = kLPI2C_2PinOpenDrain;
     * i2cConfig.baudRate_Hz = 100000U;
     * i2cConfig.busIdleTimeout_ns = 0;
     * i2cConfig.pinLowTimeout_ns = 0;
     * i2cConfig.sdaGlitchFilterWidth_ns = 0;
     * i2cConfig.sclGlitchFilterWidth_ns = 0;
     */

    /* Change the default baudrate configuration */
    masterConfig.baudRate_Hz = baudRate_Bps;
    
    /* Initialize the LPI2C master peripheral */
    LPI2C_MasterInit(base, &masterConfig, sourceClock_Hz);
 
    return kStatus_Success;
}

status_t I2C_SetBaudRate(LPI2C_Type *base, uint32_t baudRate_Hz, uint32_t sourceClock_Hz) {
    LPI2C_MasterSetBaudRate(base, sourceClock_Hz, baudRate_Hz);
    return kStatus_Success;
}

status_t I2C_Read(LPI2C_Type *base, uint8_t SlaveAddress7BIT, uint8_t reg, uint8_t *data, uint32_t size) {
    lpi2c_master_transfer_t xfer;

    xfer.flags = kLPI2C_TransferDefaultFlag;    //传输标记
    xfer.slaveAddress = SlaveAddress7BIT;       //器件地址（高7位） 
    xfer.direction = kLPI2C_Read;               //I2C读模式
    xfer.subaddress = reg;                      //寄存器地址
    xfer.subaddressSize = 1U;                   //按8位读
    xfer.data = data;                           //存放读出的数据
    xfer.dataSize = size;                       //要读多少个数据

    return LPI2C_MasterTransferBlocking(base, &xfer);
}

status_t I2C_Write(LPI2C_Type *base, uint8_t SlaveAddress7BIT, uint8_t reg, uint8_t *data, uint32_t size) 
{
    lpi2c_master_transfer_t xfer;

    xfer.flags = kLPI2C_TransferDefaultFlag;    //传输标记
    xfer.slaveAddress = SlaveAddress7BIT;       //器件地址（高7位）
    xfer.direction = kLPI2C_Write;              //I2C写模式
    xfer.subaddress = reg;                      //寄存器地址
    xfer.subaddressSize = 1U;                   //按8位读
    xfer.data = data;                           //要写入的数据
    xfer.dataSize = size;                       //要写多少个数据

    return LPI2C_MasterTransferBlocking(base, &xfer);
}





status_t I2C_Send(LPI2C_Type *base,
             uint8_t deviceAddress,
             uint32_t subAddress,
             uint8_t subAddressSize,
             uint8_t *txBuff,
             uint8_t txBuffSize) {
    lpi2c_master_transfer_t xfer;

    xfer.flags = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress = deviceAddress;
    xfer.direction = kLPI2C_Write;
    xfer.subaddress = subAddress;
    xfer.subaddressSize = subAddressSize;
    xfer.data = txBuff;
    xfer.dataSize = txBuffSize;

    return LPI2C_MasterTransferBlocking(base, &xfer);
}

status_t I2C_Receive(LPI2C_Type *base,
                uint8_t deviceAddress,
                uint32_t subAddress,
                uint8_t subAddressSize,
                uint8_t *rxBuff,
                uint8_t rxBuffSize) {
    lpi2c_master_transfer_t xfer;

    xfer.flags = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress = deviceAddress;
    xfer.direction = kLPI2C_Read;
    xfer.subaddress = subAddress;
    xfer.subaddressSize = subAddressSize;
    xfer.data = rxBuff;
    xfer.dataSize = rxBuffSize;

    return LPI2C_MasterTransferBlocking(base, &xfer);
}

status_t I2C_SendSCCB(LPI2C_Type *base,
                 uint8_t deviceAddress,
                 uint32_t subAddress,
                 uint8_t subAddressSize,
                 uint8_t *txBuff,
                 uint8_t txBuffSize) {
    lpi2c_master_transfer_t xfer;

    xfer.flags = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress = deviceAddress;
    xfer.direction = kLPI2C_Write;
    xfer.subaddress = subAddress;
    xfer.subaddressSize = subAddressSize;
    xfer.data = txBuff;
    xfer.dataSize = txBuffSize;

    return LPI2C_MasterTransferBlocking(base, &xfer);
}

status_t I2C_ReceiveSCCB(LPI2C_Type *base,
                    uint8_t deviceAddress,
                    uint32_t subAddress,
                    uint8_t subAddressSize,
                    uint8_t *rxBuff,
                    uint8_t rxBuffSize) {
    status_t status;
    lpi2c_master_transfer_t xfer;

    xfer.flags = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress = deviceAddress;
    xfer.direction = kLPI2C_Write;
    xfer.subaddress = subAddress;
    xfer.subaddressSize = subAddressSize;
    xfer.data = NULL;
    xfer.dataSize = 0;

    status = LPI2C_MasterTransferBlocking(base, &xfer);

    if (kStatus_Success == status) {
        xfer.subaddressSize = 0;
        xfer.direction = kLPI2C_Read;
        xfer.data = rxBuff;
        xfer.dataSize = rxBuffSize;

        status = LPI2C_MasterTransferBlocking(base, &xfer);
    }

    return status;
}
