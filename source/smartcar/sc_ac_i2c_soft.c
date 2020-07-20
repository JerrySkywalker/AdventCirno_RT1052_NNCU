/*
 * @Author: JerryW
 * @Date: 2020-06-10 15:29:16
 * @LastEditTime: 2020-06-11 20:30:12
 * @LastEditors: JerryW
 * @Description:
 * @FilePath: \IMRT10XX\source\smartcar\sc_ac_i2c_soft.c
 */
#include "sc_ac_i2c_soft.h"
#include "MIMXRT1052.h"
#include "sc_gpio.h"

/*****
IIC是半双工通信方式。
IIC协议：
空闲状态：I2C总线总线的SDA和SCL两条信号线同时处于高电平时
开始信号：当SCL为高期间，SDA由高到低的跳变；启动信号是一种电平跳变时序信号，而不是一个电平信号。
停止信号：当SCL为高期间，SDA由低到高的跳变；停止信号也是一种电平跳变时序信号，而不是一个电平信号。
应答信号：对于反馈有效应答位ACK的要求是，接收器在第9个时钟脉冲之前的低电平期间将SDA线拉低，并且确保在该时钟的高电平期间为稳定的低电平。
数据的有效性：I2C总线进行数据传送时，时钟信号为高电平期间，数据线上的数据必须保持稳定，只有在时钟线上的信号为低电平期间，数据线上的高电平或低电平状态才允许变化。
数据传输：在I2C总线上传送的每一位数据都有一个时钟脉冲相对应（或同步控制），即在SCL串行时钟的配合下，在SDA上逐位地串行传送每一位数据。
******/

gpio_t IIC_SOFT_SCL, IIC_SOFT_SDA;

/**
 * @description: 软件IIC，内部用延迟
 * @param {type}
 * @return:
 */
void iic_delay(uint8_t us) {
    for (int i = 0; i < 400; i++) {
        __asm("NOP"); // core bus 600M  情况下大概IIC速率 400K
    }
}

/*---------------------------------------------------------------
            IIC内部函数
----------------------------------------------------------------*/

/**
 * @description:
 * IIC初始化，默认空闲状态：I2C总线总线的SDA和SCL两条信号线同时处于高电平时
 * @param void
 * @return:
 */
void IIC_Init(void) {
    /*Pin Init*/
    IIC_SOFT_SCL.base = GPIO1;
    IIC_SOFT_SCL.pin = 16U;
    IIC_SOFT_SCL.direction = kGPIO_DigitalOutput;

    IIC_SOFT_SDA.base = GPIO1;
    IIC_SOFT_SDA.pin = 17U;
    IIC_SOFT_SDA.direction = kGPIO_DigitalOutput;

    /*空闲状态，全部为1*/
    GPIO_Write(&IIC_SOFT_SDA, 1);
    GPIO_Write(&IIC_SOFT_SCL, 1);
}

/**
 * @description: 产生IIC起始信号
 * @param void
 * @return:
 */
void IIC_Start(void) {
    GPIO_Out(&IIC_SOFT_SDA); // sda线输出
    GPIO_Write(&IIC_SOFT_SDA, 1);
    GPIO_Write(&IIC_SOFT_SCL, 1);
    iic_delay(4);
    GPIO_Write(&IIC_SOFT_SDA,
               0); // START:when CLK is high,DATA change form high to low
    iic_delay(4);
    GPIO_Write(&IIC_SOFT_SCL, 0); //钳住I2C总线，准备发送或接收数据
}

/**
 * @description: 产生IIC停止信号
 * @param void
 * @return:
 */
void IIC_Stop(void) {
    GPIO_Out(&IIC_SOFT_SDA); // sda线输出
    GPIO_Write(&IIC_SOFT_SCL, 0);
    GPIO_Write(&IIC_SOFT_SDA,
               0); // STOP:when CLK is high DATA change form low to high
    iic_delay(4);
    GPIO_Write(&IIC_SOFT_SCL, 1);
    iic_delay(4);
    GPIO_Write(&IIC_SOFT_SDA, 1); //发送I2C总线结束信号
    iic_delay(4);
}

/**
 * @description: 等待应答信号到来 （有效应答：从机第9个 SCL=0 时 SDA
 * 被从机拉低,并且 SCL = 1时 SDA依然为低）
 * @param void
 * @return:  1，接收应答失败, 0，接收应答成功
 * @note: 从机给主机的应答
 */
uint8_t IIC_WaitAck(void) {
    uint8_t ucErrTime = 0;
    GPIO_In(&IIC_SOFT_SDA); // SDA设置为输入  （从机给一个低电平做为应答）

    GPIO_Write(&IIC_SOFT_SDA, 1);
    iic_delay(1);

    GPIO_Write(&IIC_SOFT_SCL, 1);
    iic_delay(1);

    while (GPIO_Read(&IIC_SOFT_SDA)) {
        ucErrTime++;
        if (ucErrTime > 250) {
            IIC_Stop();
            return 1;
        }
    }
    GPIO_Write(&IIC_SOFT_SCL, 0); //时钟输出0
    return 0;
}

/**
 * @description: 产生ACK应答
 * （主机接收完一个字节数据后，主机产生的ACK通知从机一个字节数据已正确接收）
 * @param void
 * @return:
 * @note: 主机给从机的应答
 */
void IIC_Ack(void) {
    GPIO_Write(&IIC_SOFT_SCL, 0);
    GPIO_Out(&IIC_SOFT_SDA);

    GPIO_Write(&IIC_SOFT_SDA, 0);
    iic_delay(1);

    GPIO_Write(&IIC_SOFT_SCL, 1);
    iic_delay(2);

    GPIO_Write(&IIC_SOFT_SCL, 0);
}

/**
 * @description: 产生NACK应答
 * （主机接收完最后一个字节数据后，主机产生的NACK通知从机发送结束，释放SDA,以便主机产生停止信号）
 * @param void
 * @return:
 * @note: 主机给从机的应答
 */
void IIC_NAck(void) {
    GPIO_Write(&IIC_SOFT_SCL, 0);
    GPIO_Out(&IIC_SOFT_SDA);

    GPIO_Write(&IIC_SOFT_SDA, 1);
    iic_delay(1);

    GPIO_Write(&IIC_SOFT_SCL, 1);
    iic_delay(1);

    GPIO_Write(&IIC_SOFT_SCL, 0);
}

/**
 * @description: IIC发送一个字节
 * @param data 要写的数据
 * @return:
 * @note: 主机往从机发
 */
void IIC_SendByte(uint8_t data) {
    uint8_t t;
    GPIO_Out(&IIC_SOFT_SDA);
    GPIO_Write(&IIC_SOFT_SCL, 0); //拉低时钟开始数据传输
    for (t = 0; t < 8; t++) {
        if (data & 0x80) {
            GPIO_Write(&IIC_SOFT_SDA, 1);
        } else {
            GPIO_Write(&IIC_SOFT_SDA, 0);
        }
        iic_delay(1);
        GPIO_Write(&IIC_SOFT_SCL, 1);
        data <<= 1;
        iic_delay(1);
        GPIO_Write(&IIC_SOFT_SCL, 0);
    }
    iic_delay(1);
}

/**
 * @description: IIC读取一个字节
 * @param   ack=1 时，主机数据还没接收完
 *          ack=0 时主机数据已全部接收完成
 * @return: 返回值
 * @note: 从机往主机发
 */
uint8_t IIC_ReadByte(uint8_t ack) {
    uint8_t i, receive = 0;
    GPIO_In(&IIC_SOFT_SDA); // SDA设置为输入模式 等待接收从机返回数据
    for (i = 0; i < 8; i++) {
        GPIO_Write(&IIC_SOFT_SCL, 0);
        iic_delay(1);
        GPIO_Write(&IIC_SOFT_SCL, 1);
        receive <<= 1;
        if (GPIO_Read(&IIC_SOFT_SDA))
            receive++; //从机发送的电平
        iic_delay(1);
    }
    if (ack)
        IIC_Ack(); //发送ACK
    else
        IIC_NAck(); //发送nACK
    return receive;
}

/**
 * @description: 读取指定设备 指定寄存器的一个值
 * @param   I2C_Addr    目标设备地址
 * @param   reg	        寄存器地址
 * @param   buf         读取数据要存储的地址
 * @return: 1 失败
 *          0 成功
 */
uint8_t IIC_ReadByteFromSlave(uint8_t I2C_Addr, uint8_t reg, uint8_t *buf) {
    IIC_Start();
    IIC_SendByte(I2C_Addr); //发送从机地址
    if (IIC_WaitAck())      //如果从机未应答则数据发送失败
    {
        IIC_Stop();
        return 1;
    }
    IIC_SendByte(reg); //发送寄存器地址
    IIC_WaitAck();

    IIC_Start();
    IIC_SendByte(I2C_Addr + 1); //进入接收模式
    IIC_WaitAck();
    *buf = IIC_ReadByte(0);
    IIC_Stop(); //产生一个停止条件
    return 0;
}

/**
 * @description: 写入指定设备 指定寄存器的一个值
 * @param   I2C_Addr    目标设备地址
 * @param   reg	        寄存器地址
 * @param   buf         要写入的数据
 * @return: 1 失败
 *          0 成功
 */
uint8_t IIC_WriteByteToSlave(uint8_t I2C_Addr, uint8_t reg, uint8_t data) {
    IIC_Start();
    IIC_SendByte(I2C_Addr); //发送从机地址
    if (IIC_WaitAck()) {
        IIC_Stop();
        return 1; //从机地址写入失败
    }
    IIC_SendByte(reg); //发送寄存器地址
    IIC_WaitAck();
    IIC_SendByte(data);
    if (IIC_WaitAck()) {
        IIC_Stop();
        return 1; //数据写入失败
    }
    IIC_Stop(); //产生一个停止条件

    // return 1; //status == 0;
    return 0;
}

/**
 * @description: 读取指定设备 指定寄存器的 length个值
 * @param dev     目标设备地址
 * @param reg	   寄存器地址
 * @param length  要读的字节数
 * @param data   读出的数据将要存放的指针
 * @return: 1 成功 
 *          0 失败
 */
uint8_t IIC_ReadMultByteFromSlave(uint8_t dev, uint8_t reg, uint8_t length,
                                  uint8_t *data) {
    uint8_t count = 0;
    uint8_t temp;
    IIC_Start();
    IIC_SendByte(dev); //发送从机地址
    if (IIC_WaitAck()) {
        IIC_Stop();
        return 1; //从机地址写入失败
    }
    IIC_SendByte(reg); //发送寄存器地址
    IIC_WaitAck();
    IIC_Start();
    IIC_SendByte(dev + 1); //进入接收模式
    IIC_WaitAck();
    for (count = 0; count < length; count++) {
        if (count != (length - 1))
            temp = IIC_ReadByte(1); //带ACK的读数据
        else
            temp = IIC_ReadByte(0); //最后一个字节NACK

        data[count] = temp;
    }
    IIC_Stop(); //产生一个停止条件
    // return count;
    return 0;
}

/**
 * @description: 将多个字节写入指定设备 指定寄存器
 * @param dev     目标设备地址
 * @param reg	   寄存器地址
 * @param length  要写的字节数
 * @param data   要写入的数据将要存放的指针
 * @return: 1 成功 
 *          0 失败
 */
uint8_t IIC_WriteMultByteToSlave(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
{
    
 	uint8_t count = 0;
	IIC_Start();
	IIC_SendByte(dev); //发送从机地址
	if(IIC_WaitAck())
	{
		IIC_Stop();
		return 1; //从机地址写入失败
	}
	IIC_SendByte(reg); //发送寄存器地址
    IIC_WaitAck();	  
	for(count=0;count<length;count++)
	{
		IIC_SendByte(data[count]); 
		if(IIC_WaitAck()) //每一个字节都要等从机应答
		{
			IIC_Stop();
			return 1; //数据写入失败
		}
	}
	IIC_Stop(); //产生一个停止条件
    
	return 0;
}




