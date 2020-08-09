/******************************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2019,哈工大智能车创新俱乐部
 * All rights reserved.
 *
 * @file       		上位机程序
 * @company	        哈工大智能车创新俱乐部
 * @author     		吴文华 qq1280390945        刘睿智qq786240762
 * @version    		v1.0
 * @Software 		IAR 7.7+
 * @Target core		K66
 * @date       		2019-9-27
 *
 * @note：
		NUM_VAR为需要发送到上位机的变量个数,UP_UART为上位机UART口。Send_Variable()适用于名优科创上位机，
                void Img_Upload(void)适用于红树伟业上位机

		哈尔滨工业大学智能车创新俱乐部专用，请勿泄露
***************************************************************************************************************************/

#include "sc_upload.h"

#define NUM_VAR  6
#define UP_UART  LPUART4

float Variable[NUM_VAR]; //发送缓存数组

extern float s_speed_left_now;
extern float s_speed_right_now;
extern float s_speed_aim_left;
extern float s_speed_aim_right;
extern float g_error;
extern int Round_flag;
extern int Straight_Flag;
extern int Cross_flag;
extern int State_Change_Flag;
extern uint32_t g_roll_sigma;

/**********************************************************************************************************************
*  @brief      向上位机发送一个字节
*  @return     void
*  @since      v1.0
*  @Sample usage:          Putchar(temp)
**********************************************************************************************************************/
void Putchar(char temp)
{
    //UART_WriteBlocking(UP_UART,&temp,1); //根据实际的串口号来修改
	UART_PutChar(UP_UART, temp);
}

/**********************************************************************************************************************
*  @brief      上位机帧头
*  @return     void
*  @since      v1.0
*  @Sample usage:          Send_Begin();
*  @note   用来通知上位机新的一组数据开始，要保存数据必须发送它
**********************************************************************************************************************/
void Send_Begin(void)
{
  Putchar(0x55);
  Putchar(0xaa);
  Putchar(0x11);
}

/**********************************************************************************************************************
*  @brief      上传数据
*  @return     void
*  @since      v1.0
*  @Sample usage:          Send_Variable();
**********************************************************************************************************************/
void  Send_Variable(void)//发送实时变量
{
  uint8_t i=0,ch=0;
  float temp=0;
  Send_Begin();
  Variable[0] = Round_flag;			//此处将需要发送的变量赋值到Variable
  Variable[1] = Cross_flag;
  Variable[2] = State_Change_Flag;
  Variable[3] = g_roll_sigma;
  Variable[4] = g_error;
  Variable[5] = 5;
  Putchar(0x55);
  Putchar(0xaa);
  Putchar(0x11);
  Putchar(0x55);
  Putchar(0xaa);
  Putchar(0xff);
  Putchar(0x01);
  Putchar(NUM_VAR);
 for(i=0;i<NUM_VAR;i++)
  {
    temp=Variable[i];
    ch=BYTE0(temp);
      Putchar(ch);
    ch=BYTE1(temp);
      Putchar(ch);
    ch=BYTE2(temp);
      Putchar(ch);
    ch=BYTE3(temp);
      Putchar(ch);
  }
     Putchar(0x01);
}


