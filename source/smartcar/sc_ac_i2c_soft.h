/*** 
 * @Author: JerryW
 * @Date: 2020-06-10 15:28:50
 * @LastEditTime: 2020-06-11 20:36:48
 * @LastEditors: JerryW
 * @Description: 
 * @FilePath: \IMRT10XX\source\smartcar\sc_ac_i2c_soft.h
 */

#ifndef SMARTCAR_SC_AC_I2C_SOFT_H_
#define SMARTCAR_SC_AC_I2C_SOFT_H_

#include "fsl_common.h"
#include "fsl_gpio.h"


/* 注意 IIC总线规定，IIC空闲时 SCL和SDA都为高电平 最好外部上拉（一定不能下拉） */
/* 模拟 IIC需要注意，地址需要硬件IIC地址左移一位 例如MPU6050 硬件IIC地址用 0x68  用模拟就是0xD0*/
/* 想换用别的IO 直接修改宏定义 SOFT_IIC_SCL_PIN 、 SOFT_IIC_SDA_PIN 即可 */

//待删除

/*---------------------------------------------------------------
            IIC内部函数
----------------------------------------------------------------*/	

void IIC_Start(void);			        //发送IIC开始信号
void IIC_Stop(void);	  	            //发送IIC停止信号
void IIC_Ack(void);					    //IIC发送ACK信号
void IIC_NAck(void);				    //IIC不发送ACK信号
uint8_t IIC_WaitAck(void); 		        //IIC等待ACK信号
void IIC_SendByte(uint8_t data);        //IIC发送一个字节
uint8_t IIC_ReadByte(uint8_t ack);       //IIC读取一个字节


/*---------------------------------------------------------------
            IIC用户函数
----------------------------------------------------------------*/

void IIC_Init(void);                    //初始化IIC的IO口   
uint8_t IIC_ReadByteFromSlave(uint8_t I2C_Addr,uint8_t reg,uint8_t *buf);
uint8_t IIC_ReadMultByteFromSlave(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data);
uint8_t IIC_WriteByteToSlave(uint8_t I2C_Addr,uint8_t reg,uint8_t buf);
uint8_t IIC_WriteMultByteToSlave(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data);

#endif /* SMARTCAR_SC_AC_I2C_SOFT_H_ */
