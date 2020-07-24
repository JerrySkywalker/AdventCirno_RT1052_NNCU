/*
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    AdventCirno_RT1052_NNCU.c
 * @brief   Application entry point.
 */
/*
 * @Descripttion:
 * @version:
 * @Author: JerryW
 * @Date: 2020-05-18 15:11:47
 * @LastEditors: JerryW
 * @LastEditTime: 2020-06-23 01:21:37
 */

#include "cm_backtrace/cm_backtrace.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1052.h"
#include "fsl_debug_console.h"
#include "smartcar/sc_gpio.h"
#include "task.h"
#include "examples/test.h"
#include "smartcar/sc_oled.h"
#include "smartcar/sc_flash.h"
#include "smartcar/sc_pwm.h"
#include "smartcar/status.h"
#include "ac_lib/AC_Control.h"
#include "ac_lib/AC_Menu.h"
#include "ac_lib/AC_Command.h"
#include "ac_lib/AC_Pit.h"
#include "ac_lib/Image.h"
#include "nncu/nncu_Config.h"

BSS_DTC uint8_t heap_heap1[64 * 1024] ALIGN(8);
BSS_OC uint8_t heap_heap2[128 * 1024] ALIGN(8);
BSS_SDRAM uint8_t heap_heap3[4 * 1024 * 1024] ALIGN(8);
BSS_SDRAM_NOCACHE uint8_t buff1[120 * 184] ALIGN(64); //最多4缓存，这里声明为1mb的缓存
BSS_SDRAM_NOCACHE uint8_t buff2[120 * 184] ALIGN(64); //是因为这俩摄像头都用这个缓存
BSS_SDRAM_NOCACHE uint8_t buff3[120 * 184] ALIGN(64); //实际图片多大，缓存就多大
BSS_SDRAM_NOCACHE uint8_t buff4[120 * 184] ALIGN(64); //
const HeapRegion_t xHeapRegions[] =
	{
		{&heap_heap1[0], sizeof(heap_heap1)},
		{&heap_heap2[0], sizeof(heap_heap2)},
		{&heap_heap3[0], sizeof(heap_heap3)},
		{NULL, 0} /* Terminates the array. */
};

/*TODO: VAR declaration here*/
uint8_t g_Flag_WakeUp = 0;
extern Data_t data[10];
extern int data_identifier;
extern uint8_t image_Buffer_0[CAMERA_H][CAMERA_W];
extern float s_dir;

//clock_t start, stop; //clock_t是clock（）函数返回的变量类型
double duration;
int flag_refreshScreen = 5;
int Delayed_departure_flag = 1;   //拨码2中用于延时发车,0=stop
int Delayed_departure_flag_1 = 1; //普通情况的运行时长 0=stop
int Control_timenow = 0;		  //计数
int Control_time;				  //s->ms Delayed_departure time
int Stop_Car_flag = 0;
int Boma2_flag = 0;
int Flag_InitComplete = 0;
int Flag_Find = 0; //找到斑马线 2找到 0初始

int Flag_Signal = 0;
uint32_t g_time_us = 0;
uint32_t g_time_duration_us = 0;

/*TODO: Buffer declaration here*/
uint8_t g_flash_buff_r[FLASH_SECTOR_SIZE];
uint8_t g_flash_buff_w[FLASH_SECTOR_SIZE];
int8_t g_AD_Data[12];
uint8_t g_Boma[6];
uint8_t g_Boma_Compressed;


/*TODO: TaskHandle declaration here*/
extern TaskHandle_t AC_Menu_task_handle;
extern TaskHandle_t AC_Pit_task_handle;


TaskHandle_t LED_task_handle;
void LED_task(void *pvData)
{
	led_t led1 = {{XSNVS_PMIC_STBY_GPIO, XSNVS_PMIC_STBY_PIN, 0}};
	LED_Init(&led1);
	while (1)
	{
		LED_ON(&led1);
		vTaskDelay(1 * configTICK_RATE_HZ);
		LED_OFF(&led1);
		vTaskDelay(1 * configTICK_RATE_HZ);
	}
}

TaskHandle_t start_task_handle;
void start_task(void *pvData)
{
	gpio_t wakeUp = {XSNVS_WAKEUP_GPIO, XSNVS_WAKEUP_PIN, 0};
	PRINTF("[O K] TEST SEQUENCE: Press wakeup key to start\r\n");
	while (GPIO_Read(&wakeUp))
	{
		vTaskDelay(10);
	}

	//test(oled, "Test oled", 0, 1, 0); /*二代主控：OK*/ /*一代主控：OK*/
	//test(key_5d, "Test key_5D", 0, 1, 0);				/*二代主控：OK*//*一代主控：硬件设计缺陷（非致命），“向左”键恒为低电平，Boot电路下拉导致*/
	//test(uart,"Test uart",0,1,0);
	//test(adc, "adc", 0, 1, 1);						/*一代主控：未测试，没有电感板子*/
	//test(camera, "camera ov7725", 0, 0, 0);			/*二代主控：ING*//*一代主控：硬件设计缺陷（致命），共用boot引脚，未测试，没有摄像头*/
	//test(camera, "camera zzf", 0, 0, 0);              /*二代主控：ING*//*一代主控：硬件设计缺陷（致命），共用boot引脚，未测试，没有摄像头/
	//test(enc, "enc", 0, 1, 1);						/*一代主控：硬件设计缺陷（致命）：电阻多焊*/
	//test(flash, "Test flash", 0, 1, 1); /*二代主控：OK*/ /*一代主控：OK*/
	//test(flash_lfs, "Test flash_lfs", 1000, 0, 1);		/*二代主控：OK*//*一代主控：OK*/
	//test(keypad, "keypad", 0, 1, 1);					/*二代主控：NULL*//*一代主控：未测试，采用替代方案-五向按键*/
	//test(lcd, "lcd", 0, 1, 0);						/*二代主控：NULL*//*一代主控：未测试，采用替代方案-OLED（128*64）*/
	//test(test_MPU6050, "Test MPU6050", 0, 1, 0); 		/*二代主控：硬件IIC-Err: MRDR值不为0，正在调查*/
	//test(pit, "pit", 0, 1, 0);						/*二代主控：OK（中断方式使用舵机正常）*/
	//test(pwm, "pwm", 0, 1, 0);							/*二代主控：学长的库有问题*/
	//test(sd, "Test sd", 0, 1, 0);						/*二代主控：OK*//*一代主控：检测不到SD*/
	//test(status, "status", 0, 1, 1);
	test(pwm_ac, "pwm_ac", 0, 1, 0);					/*二代主控：Servo OK*/
	vTaskDelete(NULL);
}

/**
 * @description: Advent Cirno main function. By pressing the WakeUp Key ,you can lock and unlock the screen.
 * 				Only when the screen is unlocked can you change the settings.
 * @param void
 * @return: void
 */
TaskHandle_t AC_task_handle;

void AC_Task(void *pvData)
{
	img_t capture;
	PRINTF("[O K] AC: Sleeping. Press WAKEUP key to Start...\r\n");
	gpio_t wakeUp = {XSNVS_WAKEUP_GPIO, XSNVS_WAKEUP_PIN, 0};
	while (GPIO_Read(&wakeUp))
	{
		vTaskDelay(10);
	}

	OLED_Init();
	OLED_Fill(0x00);

	OLED_Logo();
	vTaskDelay(1000);
	OLED_Fill(0x00);

	OLED_Logo_100Limited();
	vTaskDelay(1000);
	OLED_Fill(0x00);

	/*TODO: Your Init Sequence Here*/
	//Init ENC
	ENC_Init_t(ENC2);	//R
	ENC_Init_t(ENC3);	//L
	//Init Motor
	Control_Init();
	//Init Servo
	PWM_AC_Init(PWM2, kPWM_Module_3, kPWM_PwmA, 50);
	//Init key_5D
	KEY_5D_Init();

	/*CAUTION: FLASH OPERATION*/
	PRINTF("[O K] AC: Flash: Init flash test\r\n");
	test(flash_init, "Test flash", 0, 1, 1);
	PRINTF("[O K] AC: Flash: Ready! \r\n");

	assert(0 == FLASH_Read(FLASH_DATA_ADDR_START, g_flash_buff_r, FLASH_SECTOR_SIZE));

	memcpy(&data_identifier, g_flash_buff_r, sizeof(data_identifier));
	memcpy(&data, g_flash_buff_r + FLASH_PAGE_SIZE, sizeof(data));

    delay_ms(500);
    OLED_Fill(0);
	/*
	 * @note: 	有关内存的读取说明。
	 *
	 *   		AC中的内存规划
	 *   		- 分配FLASH_DATA_ADDR_START起始的一个页存放一些单个的固定参数
	 *   		- 分配FLASH_DATA_ADDR_START+FLASH_PAGE_SIZE起始的n个扇区存放菜单参数
	 *
	 *   		采取的读写策略：整体读写；开机整体读一次，后续修改则不停的写Flash
	 *   		- 不同于K66版本的细粒度操作，由于该FLASH封装只支持对于页进行编程，这里的存储实现先利用这个底层进行，也就是说：
	 *   		- 在进程的开始和结束进行整体的读写操作。需要读写的请在全局的flash buffer中进行操作
	 *
	 *   		注意：
	 *   		- 在对页编程之前，必须擦除该页
	 *   		- 读写flash时最好关闭中断（现在的封装中已有这个设计）
	 *   		- 在FreeRTOS 中，读FLASH函数时候应该停止调度器和中断，以免出现错误。现有测试表明停止调度器可避免总先忙的错误。
	 *   		                写FLASH的影响还不明确(至少截至到目前(0702)，写Flash不停调度器没有出现问题)
	 *
	 *   		警告：调试时曾出现玄学的总线忙的严重功能性Bug，具体原因仍有待调查。以下操作可能导致总线忙，使得Flash无法初始化，一定避免
	 *   		- 不要在flash读写过程中，对涉及的地址有任何操作
	 *   		- FLASH读写函数之后的几句(触发范围不清楚)中不要出现PRINTF
	 * 			- 程序其他地方的任何语句
	 *			最好方法就是，做好备份，善用Git。大改动前必须保证有一个可用版本。发现问题及时回溯！！
	 * */

	UART_Init(LPUART6, 9600, 80 * 1000 * 1000);

	//ZZF_Init(ZZF_FrameSize120x184, LPUART6);
	//	capture.format = PixelFormatGray;
	//	capture.width = 184;
	//	capture.height = 120;
	//	CAMERA_SubmitBuff(buff1);
	//	CAMERA_SubmitBuff(buff2);
	//	CAMERA_SubmitBuff(buff3);
	//	CAMERA_SubmitBuff(buff4);
	//	assert(kStatus_Success == CAMERA_ReceiverStart());

	/*两个测试数据*/
	int8_t tmp_AD_Input[12] = {
			0xEF,0xE9,0xC6,0xFE,0x08,0xFE,0x9F,0x9E,0x2B,0x83,0xA0,0x81
	};
	int8_t tmp_AD_Input2[12] = {
	0xD1,0xCA,0x96,0xFE,0xAC,0xFE,0xD9,0xB3,0xDE,0xCF,0xCC,0x83
	};
	/*输出缓存区的指针，必须按照这个格式写*/
	int16_t* g_AD_nncu_OutBuffer = (int16_t*)pvPortMalloc(sizeof(int16_t));

	/*将上面的缓冲区指针的数取出来放这里*/
	int16_t g_AD_nncu_Output[2];

	while (1)
	{
//		if (kStatus_Success == CAMERA_FullBufferGet(&capture.pImg))
//		{
//		uint16_t height;
//		uint16_t width;
//		height = capture.height;
//		width = capture.width;
//		uint8_t* capture_buffer = pvPortMalloc(height * width);
//		if (capture.format == PixelFormatGray)
//			{
//				uint8_t* p = capture.pImg;
//				for (uint32_t i = 0; i < height; i += 1)
//					{
//						for (uint32_t j = 0; j < width; j += 1)
//							{
//								capture_buffer[i * width + j] = p[i * capture.width + j];
//								image_Buffer_0[i][j] = capture_buffer[i * width + j];
//							}
//					}
//			}
//			//THRE(image_Buffer_0, img_original);//image_Proc
//			//map();
//			//IMAGE_PROC_Main(image_Proc);
//			CAMERA_SubmitBuff(capture.pImg); //将空缓存提交
//		}

		//Image_Main(&capture);

//		if (kStatus_Success == CAMERA_FullBufferGet(&capture.pImg))
//					{
//						OLED_PrintPicture(&capture,100);			 //在屏幕上显示
//						PRINTF("fps=%d\r\n", (int)CAMERA_FpsGet());
//						CAMERA_SubmitBuff(capture.pImg); //将空缓存提交
//					}


		/*For Test NNCU Only*/
		g_time_us= TimerUsGet();
		g_AD_nncu_OutBuffer = (int16_t*)RunModel(&tmp_AD_Input);
		memcpy(&g_AD_nncu_Output[0],g_AD_nncu_OutBuffer,sizeof(int16_t));
		g_time_duration_us = TimerUsGet() - g_time_us;

		g_AD_nncu_OutBuffer = (int16_t*)RunModel(&tmp_AD_Input2);

		memcpy(&g_AD_nncu_Output[1],g_AD_nncu_OutBuffer,sizeof(int16_t));

		if (0 == GPIO_Read(&wakeUp))
		{
			vTaskDelay(500);
			if (0 == GPIO_Read(&wakeUp))
			{
				if (0 == g_Flag_WakeUp)
				{
					g_Flag_WakeUp = 1;

					PRINTF("[O K] AC: Screen Unlocked\r\n");

					/*Create your task Here*/
					xTaskCreate(AC_Menu, "AC_Menu", 1024, NULL, 2, &AC_Menu_task_handle);
				}
				else
				{
					OLED_Fill(0x00);
					g_Flag_WakeUp = 0;
					PRINTF("[O K] AC: Screen Locked. \r\n");

					/*Assemble Data to buffer*/
					memset(g_flash_buff_w,0xFFU,FLASH_SECTOR_SIZE);
					memcpy(g_flash_buff_w, &data_identifier, sizeof(data_identifier));
					memcpy(g_flash_buff_w + FLASH_PAGE_SIZE, &data, sizeof(data));

					PRINTF("[O K] AC: Flash: saving data...\r\n");

					/*Save Data to Flash*/
					assert(0 == FLASH_Erase(FLASH_DATA_ADDR_START));
					for (size_t i = 0; i < FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE; i++)
					{
						assert(0 == FLASH_Prog(FLASH_DATA_ADDR_START + i * FLASH_PAGE_SIZE, g_flash_buff_w+ i * FLASH_PAGE_SIZE));
					}

					/*Delete your task  Here*/
					vTaskDelete(AC_Menu_task_handle);
					PRINTF("[O K] AC: Menu Deleted! \r\n");
				}
			}

		}

        if(g_Flag_WakeUp == 0)
        {
            /**TODO: PUT your idea info here!**/

//            OLED_P6x8Str(0,0,(uint8_t*)"#AC Version 0.3.1");
//
//            OLED_P6x8Str(0,1,(uint8_t*)"Servo");
//            OLED_P6x8Str(0,2,(uint8_t*)"nncu-Out");
//            OLED_P6x8Str(0,4,(uint8_t*)"nncu-Time");

            /*Just a demo*/
            Str_Clr(60,2,6);
            Str_Clr(60,3,6);
            Str_Clr(60,4,6);
//            OLED_Print_Num(60,2,g_AD_nncu_Output[0]);
//            OLED_Print_Num(60,3,g_AD_nncu_Output[1]);
//            OLED_Print_Num(60,4,g_time_duration_us);
            OLED_Print_Num1(60,2,g_AD_Data[0]);
            OLED_Print_Num1(60,3,g_AD_Data[10]);
            OLED_Print_Num1(60,4,g_Boma[5]);

            //PRINTF("[OK] AC: Status: nncu time used %d\n",(int)g_time_duration_us);


        }

	}
}

int main(void)
{
	BOARD_ConfigVectors();
	BOARD_ConfigMPU();
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
	BOARD_InitDebugConsole();

	PRINTF("\r\n********************\r\n");
	PRINTF("Advent Cirno v0.3.1");
	PRINTF("\r\n********************\r\n");
    Control_Init();

	cm_backtrace_init("IMRT10XX.axf", "1.1.1", "19.4.14");
	vPortDefineHeapRegions(xHeapRegions);
	//xTaskCreate(LED_task, "LED_task", 128, NULL, 3, &LED_task_handle);//led不停，单片机不s
	//xTaskCreate(start_task, "start_task", 1024, NULL, 2, &start_task_handle);

	//AC: Personal Settings
	xTaskCreate(AC_Task, "AC_Task", 2048, NULL, 2, &AC_task_handle);
    xTaskCreate(AC_Pit, "AC_Pit", 2048, NULL, 2, &AC_Pit_task_handle);

	UART_Init(LPUART2,1152000,80000000);
	LPUART_EnableInterrupts(LPUART2, kLPUART_RxDataRegFullInterruptEnable);
	EnableIRQ(LPUART2_IRQn);

	vTaskStartScheduler();
	return 0;
}

void LPUART2_IRQHandler(void)
{
    uint8_t temp_COM_data_buffer[19];
    /* If new data arrived. */
    if ((kLPUART_RxDataRegFullFlag)&LPUART_GetStatusFlags(LPUART2))
    {
        LPUART_ReadBlocking(LPUART2,temp_COM_data_buffer,19);

#ifdef DEBUG_K66_OUTPUT
        LPUART_WriteBlocking(LPUART1,temp_COM_data_buffer,19);
#endif
        /*Get AD Data*/
        for(int i = 0;i<12;i++)
        {
            g_AD_Data[i] = temp_COM_data_buffer[i+3];
        }

        /*Get Boma Data*/
        g_Boma_Compressed = temp_COM_data_buffer[15];
        for(int i = 0;i<6;i++)
        {
            g_Boma[i] = g_Boma_Compressed % 2;
            g_Boma_Compressed /= 2;
        }

    }
}

