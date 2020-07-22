/*
 * test.h
 *
 *  Created on: 2020年4月15日
 *      Author: 17616
 */
//#define EXAMPLES_TEST_H_

#ifndef EXAMPLES_TEST_H_

#define EXAMPLES_TEST_H_

#include "FreeRTOS.h"
#include "task.h"
#include "fsl_debug_console.h"
#include "smartcar/status.h"
#include "board.h"
#include "smartcar/sc_ac_delay.h"
#include "MIMXRT1052.h"
#include "fsl_gpio.h"

#define TEST_ADC
#define TEST_CAMERA
#define TEST_ENC
#define TEST_FLASH
//#define TEST_GPIO//这个就不用写示例了
//#define TEST_IIC//没法单独测试
//#define TEST_KEYPAD
//#define TEST_LCD
#define TEST_OLED
//#define TEST_PIT
//#define TEST_PWM
#define TEST_SD
#define TEST_STATUS
#define TEST_UART


#define TEST_KEY_5D
#define TEST_MPU6050
#define TEST_AC_PWM

void test(TaskFunction_t pxTaskCode, const char *const pcName, uint32_t TimeOutMs, int runDirect, int exitDirect)
{
	if (TimeOutMs == 0)
	{
		TimeOutMs = ~0;
	}
	TaskStatus_t xTaskDetails;
	vTaskGetInfo(NULL, &xTaskDetails, pdTRUE, eInvalid);
	//    xTaskDetails.uxCurrentPriority;//调用Test函数任务的优先级
	if (runDirect == 0)
	{
		PRINTF("[O K] Run task: %s? y/n \r\n", pcName);
		char ch = GETCHAR();
		if (ch == 'y' || ch == 'Y')
		{
		}
		else
		{
			return;
		}
	}
	PRINTF("[O K] Run task: %s...\r\n", pcName);
	uint32_t msp = TimerUsGet();
	TaskHandle_t task_handle;
	xTaskCreate(pxTaskCode, pcName, 2048, NULL, xTaskDetails.uxCurrentPriority, &task_handle);
	vTaskDelay(100);
	eTaskState e;
	while (1)
	{
		e = eTaskGetState(task_handle);
		if (e == eDeleted || e == eInvalid)
		{
			if (exitDirect == 0)
			{
				PRINTF("[O K] Task: %s exit! Press any key to continue\r\n", pcName);
				GETCHAR();
			}
			else
			{
				PRINTF("[O K] Task: %s exit!\r\n", pcName);
			}
			return;
		}
		else if (TimerUsGet() - msp >= TimeOutMs)
		{
			vTaskDelete(task_handle);
			PRINTF("[Err] Task: %s time out!\r\n", pcName);
			return;
		}
		vTaskDelay(100);
	}
}

#ifdef TEST_ADC
#include "smartcar/sc_adc.h"
void adc(void *pv)
{
	ADC_Init2();
	PRINTF("adc\tchannel\tvalue\r\n");
	for (int i = 3; i < 9; i++)
	{
		float val = (float)ADC_Read(ADC1, i) / 4096.0;
		PRINTF("adc1\%d\t%d.%d%d%dv\r\n", i, (int)val,
			   ((int)(val * 10)) % 10,
			   ((int)(val * 100)) % 10,
			   ((int)(val * 1000)) % 10);
	}
	PRINTF("\r\n");
	vTaskDelete(NULL);
}
#endif // TEST_ADC

#ifdef TEST_CAMERA
#include <stdio.h>
#include "smartcar/sc_sd.h"
#include "smartcar/sc_camera_ov7725.h"
#include "smartcar/sc_camera_zzf.h"
#include "smartcar/sc_lcd.h"
BSS_SDRAM_NOCACHE uint8_t buff1[120 * 184] ALIGN(64); //最多4缓存，这里声明为1mb的缓存
BSS_SDRAM_NOCACHE uint8_t buff2[120 * 184] ALIGN(64); //是因为这俩摄像头都用这个缓存
BSS_SDRAM_NOCACHE uint8_t buff3[120 * 184] ALIGN(64); //实际图片多大，缓存就多大
BSS_SDRAM_NOCACHE uint8_t buff4[120 * 184] ALIGN(64); //
void camera(void *pv)									//采集图像并且保存到sd卡中
{
	FIL png;
	int error;
	char str[1000];
	img_t img;
//	if (SD_Mount() == kStatus_Success)
//	{
		char ch;
		OLED_Init();
		while (1)
		{
			PRINTF("which one?o->ov7725 z->zzf\r\n");
			ch = GETCHAR();
			if (ch == 'o' || ch == 'O' || ch == 'z' || ch == 'Z')
			{
				break;
			}
		}
		if (ch == 'o' || ch == 'O') //初始化ov7725摄像头
		{
			PRINTF("select ov7725\r\n");
			CAMERA_MclkSet(24 * 1000 * 1000);
			OV7725_Init(OV7725_FrameSizeVGA480x640);
			OV7725_Light_Mode(0);
			OV7725_Color_Saturation(0);
			OV7725_Brightness(0);
			OV7725_Contrast(0);
			OV7725_Special_Effects(0);
			img.format = PixelFormatRGB565;
			img.width = 640;
			img.height = 480;
		}
		else if (ch == 'z' || ch == 'Z') //初始化总钻风摄像头
		{
			PRINTF("select zzf\r\n");
			UART_Init(LPUART6, 9600, 80 * 1000 * 1000);
			ZZF_Init(ZZF_FrameSize480x752, LPUART6);
			img.format = PixelFormatGray;
			img.width = 752;
			img.height = 480;
		}
		//摄像头初始化完毕，开始接收图像
		CAMERA_SubmitBuff(buff1);
		CAMERA_SubmitBuff(buff2);
		CAMERA_SubmitBuff(buff3);
		CAMERA_SubmitBuff(buff4);
		assert(kStatus_Success == CAMERA_ReceiverStart());
//		for (int i = 0; i < 1000; i++)
//		{
while(1)
{
			if (kStatus_Success == CAMERA_FullBufferGet(&img.pImg))
			{
//				snprintf(str, 1000, "/picture/%d.png", i);
//				error = f_open(&png, str, (FA_WRITE | FA_CREATE_ALWAYS));
//				if (error)
//				{
//					if (error == FR_EXIST)
//					{
//						PRINTF("%s exists.\r\n", str);
//					}
//					else
//					{
//						PRINTF("Open %s failed.\r\n", str);
//						vTaskDelete(NULL);
//						return;
//					}
//				}
//				CAMERA_Save2PngFile(&img, &png); //保存到sd卡中
				OLED_PrintPicture(&img,100);			 //在屏幕上显示
//				if (FR_OK == f_close(&png))
//				{
//					PRINTF("Save %s success.\r\n", str);
//				}
//				else
//				{
//					PRINTF("close %s failed.\r\n", str);
//					vTaskDelete(NULL);
//					return;
//				}
				PRINTF("fps=%d\r\n", (int)CAMERA_FpsGet());
				CAMERA_SubmitBuff(img.pImg); //将空缓存提交
			}
//			else
//			{
//				i--;
//				vTaskDelay(1);
//			}
//		}
		//100张图片采集达成，统计一下fps
//		PRINTF("fps=%d\r\n", (int)CAMERA_FpsGet());
//		CAMERA_ReceiverStop(); //停止传输
//	}

//	else
//	{
//		PRINTF("Please insert SD card\r\n");
//	}
}
	vTaskDelete(NULL);
}
#endif // TEST_CAMERA

#ifdef TEST_ENC
#include "smartcar/sc_enc.h"
void enc(void *pv)
{

#ifdef TEST_OLED
	OLED_Init();
	OLED_Fill(0x00);
	OLED_P6x8Str(0, 0, "Init ENC test...");
#endif

	ENC_Init_t(ENC1);
	ENC_Init_t(ENC2);
	ENC_Init_t(ENC3);
	ENC_Init_t(ENC4);

#ifdef TEST_OLED
	while (1)
	{
		OLED_P6x8Str(0, 1, "ENC  Date  Pos  Rev");
		OLED_Print_Num1(0, 2, 1);
		OLED_Print_Num1(0, 3, 2);
		OLED_Print_Num1(0, 4, 3);
		OLED_Print_Num1(0, 5, 4);

		OLED_Print_Num1(10, 2, (int)ENC_Dateget(ENC1));
		OLED_Print_Num1(10, 3, (int)ENC_Dateget(ENC2));
		OLED_Print_Num1(10, 4, (int)ENC_Dateget(ENC3));
		OLED_Print_Num1(10, 5, (int)ENC_Dateget(ENC4));

		OLED_Print_Num1(60, 2, (int)ENC_Positionget(ENC1));
		OLED_Print_Num1(60, 3, (int)ENC_Positionget(ENC2));
		OLED_Print_Num1(60, 4, (int)ENC_Positionget(ENC3));
		OLED_Print_Num1(60, 5, (int)ENC_Positionget(ENC4));

		OLED_Print_Num1(90, 2, (int)ENC_Revolutionget(ENC1));
		OLED_Print_Num1(90, 3, (int)ENC_Revolutionget(ENC2));
		OLED_Print_Num1(90, 4, (int)ENC_Revolutionget(ENC3));
		OLED_Print_Num1(90, 5, (int)ENC_Revolutionget(ENC4));

		vTaskDelay(500);
		OLED_Fill(0x00);
	}
#else
	char ch;
	while (1)
	{
		PRINTF("\r\np->print status of enc and e->exit\r\n");
		ch = GETCHAR();
		if (ch == 'p' || ch == 'P')
		{
			PRINTF("ENC\tDate\tPosition\tRevolution\r\n");
			PRINTF("1\t%d\t%d\t%d\r\n", (int)ENC_Dateget(ENC1), (int)ENC_Positionget(ENC1), (int)ENC_Revolutionget(ENC1));
			PRINTF("2\t%d\t%d\t%d\r\n", (int)ENC_Dateget(ENC2), (int)ENC_Positionget(ENC2), (int)ENC_Revolutionget(ENC2));
			PRINTF("3\t%d\t%d\t%d\r\n", (int)ENC_Dateget(ENC3), (int)ENC_Positionget(ENC3), (int)ENC_Revolutionget(ENC3));
			PRINTF("4\t%d\t%d\t%d\r\n", (int)ENC_Dateget(ENC4), (int)ENC_Positionget(ENC4), (int)ENC_Revolutionget(ENC4));
		}
		else if (ch == 'e' || ch == 'E')
		{
			break;
		}
	}
#endif
	ENC_Dateclear(ENC1);
	ENC_Dateclear(ENC2);
	ENC_Dateclear(ENC3);
	ENC_Dateclear(ENC4);
	vTaskDelete(NULL);
}
#endif // TEST_ENC

#ifdef TEST_FLASH
#include "smartcar/sc_flash.h"

void flash_init(void *pv)
{

#ifdef TEST_OLED
    OLED_Init();
    OLED_Fill(0x00);
    OLED_P6x8Str(0, 0, "Init Flash test...");
#endif

    PRINTF("[O K] AC: Init flash test\r\n");
    assert(0 == FLASH_Init());

#ifdef TEST_OLED
    OLED_P6x8Str(0, 1, "[O K] Flash Ok");
#endif
    PRINTF("[O K] AC: Flash test success!\r\n");
    vTaskDelete(NULL);
}
void flash(void *pv)
{

#ifdef TEST_OLED
	OLED_Init();
	OLED_Fill(0x00);
	OLED_P6x8Str(0, 0, "Init Flash test...");
#endif

	PRINTF("[O K] Init flash test\r\n");
	assert(0 == FLASH_Init());
	int addr_start = 3 * 1024 * 1024;
	static uint8_t buff_r[FLASH_SECTOR_SIZE];
	static uint8_t buff_w[FLASH_SECTOR_SIZE];
	assert(0 == FLASH_Erase(addr_start));
	assert(0 == FLASH_Read(addr_start, buff_r, FLASH_SECTOR_SIZE));
	for (size_t i = 0; i < FLASH_SECTOR_SIZE; i++)
	{
		assert(0xff == buff_r[i]);
	}
	for (size_t i = 0; i < FLASH_PAGE_SIZE; i++)
	{
		buff_w[i] = i;
	}
	for (size_t i = 0; i < FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE; i++)
	{
		assert(0 == FLASH_Prog(addr_start + i * FLASH_PAGE_SIZE, buff_w));
	}
	assert(0 == FLASH_Read(addr_start, buff_r, FLASH_SECTOR_SIZE));
	for (size_t i = 0; i < FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE; i++)
	{
		for (size_t j = 0; j < FLASH_PAGE_SIZE; j++)
		{
			assert(buff_r[j] == j);
		}
	}
#ifdef TEST_OLED
	OLED_P6x8Str(0, 1, "[O K] Flash Ok");
#endif
	PRINTF("flash test success\r\n");
	vTaskDelete(NULL);
}

void flash_lfs(void *pv)
{
	PRINTF("flash rw with lfs\r\n");
	assert(0 == FLASH_Init());
	static lfs_t lfs;
	static struct lfs_config cfg;
	static lfs_file_t file;
	FLASH_LfsGetDefaultConfig(&cfg);
	int err = lfs_mount(&lfs, &cfg);

	// reformat if we can't mount the filesystem
	// this should only happen on the first boot
	if (err)
	{
		lfs_format(&lfs, &cfg);
		lfs_mount(&lfs, &cfg);
		PRINTF("reformat!\r\n");
	}
	// read current count
	uint32_t boot_count = 0;
	lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
	lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

	// update boot count
	boot_count += 1;
	lfs_file_rewind(&lfs, &file);
	lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

	// remember the storage is not updated until the file is closed successfully
	lfs_file_close(&lfs, &file);

	// release any resources we were using
	lfs_unmount(&lfs);

	// print the boot count
	PRINTF("%s boot_count: %d\r\n", BOARD_NAME, boot_count);

	//    while (1){vTaskDelay(1000);TaskStatusPrint();}
	vTaskDelete(NULL);
}
#endif // TEST_FLASH

#ifdef TEST_KEYPAD
#include "pin_mux.h"
#include "sc_keypad.h"
void keypad(void *pv)
{
	gpio_t row_list[] = {
		{LCD_D0_GPIO, LCD_D0_PIN, 0},
		{LCD_D1_GPIO, LCD_D1_PIN, 0},
		{LCD_D2_GPIO, LCD_D2_PIN, 0},
		{LCD_D3_GPIO, LCD_D3_PIN, 0},
		{LCD_D4_GPIO, LCD_D4_PIN, 0},
		{LCD_D5_GPIO, LCD_D5_PIN, 0},
		{LCD_D6_GPIO, LCD_D6_PIN, 0},
		{LCD_D7_GPIO, LCD_D7_PIN, 0},
		{0, 0, 0}};

	gpio_t col_list[] = {
		{LCD_C0_GPIO, LCD_C0_PIN, 0},
		{LCD_C1_GPIO, LCD_C1_PIN, 0},
		{0, 0, 0}};
	char ch;
	KEYPAD_Init(&g_keypad, col_list, row_list);
	while (1)
	{
		PRINTF("\r\np->print status of keypad and e->exit\r\n");
		ch = GETCHAR();
		if (ch == 'p' || ch == 'P')
		{
			PRINTF("\tc0\tc1\r\n");
			for (int i = 0; i < 8; i++)
			{
				PRINTF("r%d\t", i);
				for (int j = 0; j < 2; j++)
				{
					PRINTF("%d\t", (int)KEYPAD_Get(&g_keypad, i, j));
				}
				PRINTF("\r\n");
			}
		}
		else if (ch == 'e' || ch == 'E')
		{
			break;
		}
	}
	vTaskDelete(NULL);
}

#endif // TEST_KEYPAD

#ifdef TEST_LCD
#include "sc_lcd.h"
void lcd(void *pv)
{

	Lcd_Init(); //初始化OLED
	LCD_Clear(WHITE);
	BACK_COLOR = WHITE;
	LCD_ShowString(10, 35, "2.4 TFT SPI 240*320", RED);
	LCD_ShowString(10, 55, "LCD_W:", RED);
	LCD_ShowNum(70, 55, LCD_W, 3, RED);
	LCD_ShowString(110, 55, "LCD_H:", RED);
	LCD_ShowNum(160, 55, LCD_H, 3, RED);
	LCD_ShowNum1(80, 95, 3.14159f, 5, RED);
	vTaskDelete(NULL);
}
#endif // TEST_LCD

#ifdef TEST_OLED
#include "smartcar/sc_oled.h"
void oled(void *pv)
{
	vTaskSuspendAll();

	OLED_Init();
	OLED_Fill(0x00);

	OLED_Logo();
	delay_ms(100);
	OLED_Fill(0x00);

	OLED_Logo_100Limited();
	delay_ms(100);
	OLED_Fill(0x00);

	OLED_P6x8Str(0, 0, "Init Boot Test...");
	OLED_P6x8Str(0, 1, "[ O K ] OLED");
	delay_ms(100);
	OLED_Fill(0x00);

	PRINTF("api OLED_PrintPicture test 64*128\r\n");
	img_t a;
	a.height = 64;
	a.width = 128;
	a.format = PixelFormatGray;
	a.pImg = pvPortMalloc(64 * 128);

	memset(a.pImg, 0, 64 * 128);
	for (size_t i = 0; i < 128; i++)
	{
		((uint8_t*)a.pImg)[30 * 128 + i] = 255;
		((uint8_t*)a.pImg)[63 * 128 + i] = 255;
		((uint8_t*)a.pImg)[50 * 128 + i] = 255;
	}

	for (size_t i = 0; i < 64; i++)
	{
		((uint8_t*)a.pImg)[i* 128 + 30] = 255;
		((uint8_t*)a.pImg)[i* 128 + 90] = 255;
		((uint8_t*)a.pImg)[i * 128 + 120] = 255;
	}
	OLED_PrintPicture(&a, 100);
	vPortFree(a.pImg);
	PRINTF("any key api OLED_PrintPicture test 240*320\r\n");
	GETCHAR();
	OLED_Fill(0x00);
	a.height = 240;
	a.width = 320;
	a.format = PixelFormatGray;
	a.pImg = pvPortMalloc(240 * 320);

	memset(a.pImg, 0, 240 * 320);
	for (size_t i = 0; i < 320; i++)
	{
		((uint8_t*)a.pImg)[30 * 320 + i] = 255;
		((uint8_t*)a.pImg)[31 * 320 + i] = 255;
		((uint8_t*)a.pImg)[32 * 320 + i] = 255;
		((uint8_t*)a.pImg)[33 * 320 + i] = 255;

		((uint8_t*)a.pImg)[50 * 320 + i] = 255;
		((uint8_t*)a.pImg)[51 * 320 + i] = 255;
		((uint8_t*)a.pImg)[52 * 320 + i] = 255;
		((uint8_t*)a.pImg)[53 * 320 + i] = 255;

		((uint8_t*)a.pImg)[200 * 320 + i] = 255;
		((uint8_t*)a.pImg)[201 * 320 + i] = 255;
		((uint8_t*)a.pImg)[202 * 320 + i] = 255;
		((uint8_t*)a.pImg)[203 * 320 + i] = 255;
	}

	for (size_t i = 0; i < 240; i++)
	{
		((uint8_t*)a.pImg)[i * 320 + 30] = 255;
		((uint8_t*)a.pImg)[i * 320 + 31] = 255;
		((uint8_t*)a.pImg)[i * 320 + 32] = 255;
		((uint8_t*)a.pImg)[i * 320 + 33] = 255;
		((uint8_t*)a.pImg)[i * 320 + 90] = 255;
		((uint8_t*)a.pImg)[i * 320 + 91] = 255;
		((uint8_t*)a.pImg)[i * 320 + 92] = 255;
		((uint8_t*)a.pImg)[i * 320 + 93] = 255;
		((uint8_t*)a.pImg)[i * 320 + 120] = 255;
		((uint8_t*)a.pImg)[i * 320 + 121] = 255;
		((uint8_t*)a.pImg)[i * 320 + 122] = 255;
		((uint8_t*)a.pImg)[i * 320 + 123] = 255;
	}
	OLED_PrintPicture(&a, 100);
	vPortFree(a.pImg);



	xTaskResumeAll();
	vTaskDelete(NULL);
}
#endif // TEST_OLED

#ifdef TEST_PIT
#include "sc_pit.h"
#include "status.h"
#include "sc_pwm.h"
#include "sc_gpio.h"
int c0count = 0;
int c1count = 0;
int c2count = 0;
int c3count = 0;
uint32_t c0time_us = 0;
uint32_t c1time_us = 0;
uint32_t c2time_us = 0;
uint32_t c3time_us = 0;
uint32_t c0pit_us = 0;
uint32_t c1pit_us = 0;
uint32_t c2pit_us = 0;
uint32_t c3pit_us = 0;
void pit(void *pv)
{
//	gpio_pin_config_t pinconfig;
//	pinconfig.direction = kGPIO_DigitalOutput;
//	pinconfig.outputLogic = 1;
//	GPIO_PinInit(GPIO1, 0U, &pinconfig);



	PIT_Init2(kPIT_Chnl_0, 500);	   //500us
	PIT_Init2(kPIT_Chnl_1, 5 * 1000);  //5ms
	PIT_Init2(kPIT_Chnl_2, 10 * 1000); //10ms
	PIT_Init2(kPIT_Chnl_3, 20 * 1000); //20ms
	NVIC_SetPriority(PIT_IRQn, 6);	   //设置pit中断优先级为6
	PIT_StartTimer(PIT, kPIT_Chnl_0);
	PIT_StartTimer(PIT, kPIT_Chnl_1);
	PIT_StartTimer(PIT, kPIT_Chnl_2);
	PIT_StartTimer(PIT, kPIT_Chnl_3);



	while (1)
	{
		vTaskDelay(10);
//		if (c0count >= 3 &&
//			c1count >= 3 &&
//			c2count >= 3 &&
//			c3count >= 3)
//		{
//			PRINTF("Period us-->ch0:%d,ch1:%d,ch2:%d,ch3:%d\r\n", c0pit_us, c1pit_us, c2pit_us, c3pit_us);
//			break;
//		}
	}
	vTaskDelete(NULL);
}
//中断服务函数（不准直接改名字可以用define改名字）
//注意四个pit通道共用一个中断服务函数
/*RAMFUNC_ITC*/ void PIT_IRQHandler(void)
{
	/*清除中断标志位 （要用的时候解注释，通道可换）*/
	if (PIT_GetStatusFlags(PIT, kPIT_Chnl_0) == kPIT_TimerFlag)
	{
		PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
		c0count++;
		c0pit_us = TimerUsGet() - c0time_us;
		c0time_us = TimerUsGet();
		if (c0count >= 3)
		{
			PIT_StopTimer(PIT, kPIT_Chnl_0);
		}
	}
	if (PIT_GetStatusFlags(PIT, kPIT_Chnl_1) == kPIT_TimerFlag)
	{
		PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag);
		c1count++;
		c1pit_us = TimerUsGet() - c1time_us;
		c1time_us = TimerUsGet();
		if (c1count >= 3)
		{
			PIT_StopTimer(PIT, kPIT_Chnl_1);
		}
	}
	if (PIT_GetStatusFlags(PIT, kPIT_Chnl_2) == kPIT_TimerFlag)
	{
		PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, kPIT_TimerFlag);
		c2count++;
		c2pit_us = TimerUsGet() - c2time_us;
		c2time_us = TimerUsGet();
		if (c2count >= 3)
		{
			PIT_StopTimer(PIT, kPIT_Chnl_2);
		}
	}
	int g_ServoHighFlag = 0
	if (PIT_GetStatusFlags(PIT, kPIT_Chnl_3) == kPIT_TimerFlag)
	{
		PIT_ClearStatusFlags(PIT, kPIT_Chnl_3, kPIT_TimerFlag);

		if(0==g_ServoHighFlag)
		{
			GPIO_PinWrite(GPIO1,0U,1);
			PIT_Init2(kPIT_Chnl_0, 15000);
		}
		else
		{
			GPIO_PinWrite(GPIO1,0U,1);
			PIT_Init2(kPIT_Chnl_0, 185000);
		}
	}
	/*中断服务函数内容*/
	__DSB();
	/*DSB--数据同步屏障
	 * 作用：因为CPU时钟的主频大于IP总线时钟故会出现中断标志位还未被清掉时中断服务函数中的内容会执行完，而使得程序一遍一遍的进入中断服务函数，
	 * 而DSB则是为了预防这一情况的发生。*/
}
#endif // TEST_PIT

#ifdef TEST_PWM
#include "sc_pwm.h"
#include "sc_gpio.h"
pwm_t test_my1 = {PWM2, kPWM_Module_0, 20 * 1000, 0, 0, kPWM_HighTrue};	//L
pwm_t test_my2 = {PWM2, kPWM_Module_1, 20 * 1000, 0, 0, kPWM_HighTrue};  //R
pwm_t test_my3 = {PWM2, kPWM_Module_3, 50, 0, 0, kPWM_HighTrue};			//servo
//pwm_t test_my4 = {PWM2, kPWM_Module_1, 25 * 1000, 0, 0, kPWM_HighTrue};
gpio_t test_OE_B = {GPIO5,02U,0 };	//L7 74LVC245_OE_B, 电机PWM相关，低电平有效
void pwm(void *pv)
{
	pwm_t *list[] =
		{
			&test_my1,
			&test_my2,
			&test_my3,
			NULL};
	PWM_Init2(list);
	GPIO_Init(&test_OE_B);
//	test_my1.dutyA = 0;
//	test_my1.dutyB = 0;
//	test_my2.dutyA = 30;
//	test_my2.dutyB = 0;
	test_my3.dutyA = 7.5;
	test_my3.dutyB = 7.5;
//	test_my4.dutyA = 10.0;
//	test_my4.dutyB = 40.0;
	PWM_Change(&test_my1);
	PWM_Change(&test_my2);
//	PWM_Change(&test_my3);
//	PWM_Change(&test_my4);
	GPIO_Write(&test_OE_B, 0);//使能缓冲芯片输出
	vTaskDelete(NULL);
}
#endif // TEST_PWM

#ifdef TEST_SD
#include"smartcar/sc_sd.h"
void sd(void* pv)//关于读写文件系统的示例见摄像头的示例
{
	if (kStatus_Success != SD_Mount())
	{
		PRINTF("Please insert SD card\r\n");
	}
	//下面开始测试sd卡存图程序是否好使
	static img_t a;
	static FIL png;
	a.height = 64;
	a.width = 128;
	a.format = PixelFormatGray;
	a.pImg = pvPortMalloc(64 * 128);
	memset(a.pImg, 0, 64 * 128);
	for (size_t i = 0; i < 128; i++)
	{
		((uint8_t*)a.pImg)[30 * 128 + i] = 255;
		((uint8_t*)a.pImg)[63 * 128 + i] = 255;
		((uint8_t*)a.pImg)[50 * 128 + i] = 255;
	}
	for (size_t i = 0; i < 64; i++)
	{
		((uint8_t*)a.pImg)[i * 128 + 30] = 255;
		((uint8_t*)a.pImg)[i * 128 + 90] = 255;
		((uint8_t*)a.pImg)[i * 128 + 120] = 255;
	}
	//准备好图片开始存
	char* str = "/64128.png";
	int error = f_open(&png, str, (FA_WRITE | FA_CREATE_ALWAYS));
	if (error)
	{
		if (error == FR_EXIST)
		{
			PRINTF("%s exists.\r\n", str);
		}
		else
		{
			PRINTF("Open %s failed.\r\n", str);
			vTaskDelete(NULL);
			return;
		}
	}
	CAMERA_Save2PngFile(&a, &png);//保存到sd卡中
	if (FR_OK == f_close(&png))
	{
		PRINTF("Save %s success.\r\n", str);
	}
	else
	{
		PRINTF("close %s failed.\r\n", str);
		vTaskDelete(NULL);
	}


	vPortFree(a.pImg);



	a.height = 240;
	a.width = 320;
	a.format = PixelFormatGray;
	a.pImg = pvPortMalloc(240 * 320);

	memset(a.pImg, 0, 240 * 320);
	for (size_t i = 0; i < 320; i++)
	{
		((uint8_t*)a.pImg)[30 * 320 + i] = 255;
		((uint8_t*)a.pImg)[31 * 320 + i] = 255;
		((uint8_t*)a.pImg)[32 * 320 + i] = 255;
		((uint8_t*)a.pImg)[33 * 320 + i] = 255;

		((uint8_t*)a.pImg)[50 * 320 + i] = 255;
		((uint8_t*)a.pImg)[51 * 320 + i] = 255;
		((uint8_t*)a.pImg)[52 * 320 + i] = 255;
		((uint8_t*)a.pImg)[53 * 320 + i] = 255;

		((uint8_t*)a.pImg)[200 * 320 + i] = 255;
		((uint8_t*)a.pImg)[201 * 320 + i] = 255;
		((uint8_t*)a.pImg)[202 * 320 + i] = 255;
		((uint8_t*)a.pImg)[203 * 320 + i] = 255;
	}

	for (size_t i = 0; i < 240; i++)
	{
		((uint8_t*)a.pImg)[i * 320 + 30] = 255;
		((uint8_t*)a.pImg)[i * 320 + 31] = 255;
		((uint8_t*)a.pImg)[i * 320 + 32] = 255;
		((uint8_t*)a.pImg)[i * 320 + 33] = 255;
		((uint8_t*)a.pImg)[i * 320 + 90] = 255;
		((uint8_t*)a.pImg)[i * 320 + 91] = 255;
		((uint8_t*)a.pImg)[i * 320 + 92] = 255;
		((uint8_t*)a.pImg)[i * 320 + 93] = 255;
		((uint8_t*)a.pImg)[i * 320 + 120] = 255;
		((uint8_t*)a.pImg)[i * 320 + 121] = 255;
		((uint8_t*)a.pImg)[i * 320 + 122] = 255;
		((uint8_t*)a.pImg)[i * 320 + 123] = 255;
	}
	//准备好图片开始存
	str = "/240320.png";
	error = f_open(&png, str, (FA_WRITE | FA_CREATE_ALWAYS));
	if (error)
	{
		if (error == FR_EXIST)
		{
			PRINTF("%s exists.\r\n", str);
		}
		else
		{
			PRINTF("Open %s failed.\r\n", str);
			vTaskDelete(NULL);
			return;
		}
	}
	CAMERA_Save2PngFile(&a, &png);//保存到sd卡中
	if (FR_OK == f_close(&png))
	{
		PRINTF("Save %s success.\r\n", str);
	}
	else
	{
		PRINTF("close %s failed.\r\n", str);
		vTaskDelete(NULL);
	}
	vPortFree(a.pImg);



	vTaskDelete(NULL);
}
#endif // TEST_SD


#ifdef TEST_STATUS
void status(void *pv)
{
	TaskStatusPrint();
	APP_PrintRunFrequency(0);
	PRINTF("%dms\r\n", TimerMsGet());
	vTaskDelete(NULL);
}
#endif //TEST_STATUS

#ifdef TEST_KEY_5D
#include "smartcar/sc_ac_key_5D.h"

void key_5d(void *pv)
{
	vTaskSuspendAll();

#ifdef TEST_OLED
	OLED_Init();
	OLED_Fill(0x00);
	OLED_P6x8Str(0, 0, "Init key test...");
#endif

	KEY_5D_Init();

	int keyCheckPass = 0;
	int keyCheckStatus[5];

	while (1)
	{
		keyCheckPass = 0;
		if (0 == GPIO_PinRead(GPIO2, 0U))
		{
			keyCheckStatus[0] = 1;
		}
		if (0 == GPIO_PinRead(GPIO2, 1U))
		{
			keyCheckStatus[1] = 1;
		}
		if (0 == GPIO_PinRead(GPIO2, 2U))
		{
			keyCheckStatus[2] = 1;
		}
		if (0 == GPIO_PinRead(GPIO2, 3U))
		{
			keyCheckStatus[3] = 1;
		}
		if (0 == GPIO_PinRead(GPIO1, 9U))
		{
			keyCheckStatus[4] = 1;
		}

#ifdef TEST_OLED
		OLED_Print_Num(0, 1, GPIO_PinRead(GPIO2, 0U));
		OLED_Print_Num(0, 2, GPIO_PinRead(GPIO2, 1U));
		OLED_Print_Num(0, 3, GPIO_PinRead(GPIO2, 2U));
		OLED_Print_Num(0, 4, GPIO_PinRead(GPIO2, 3U));
		OLED_Print_Num(0, 5, GPIO_PinRead(GPIO1, 9U));
#endif

		for (int j = 0; j <= 4; j++)
		{
			keyCheckPass += keyCheckStatus[j];
		}
		if(5 == keyCheckPass)
		{
#ifdef TEST_OLED
			OLED_P6x8Rst(0,6,"Success!");
#endif
			delay_ms(1000);
			OLED_Fill(0x00);
			break;
		}

		delay_ms(100);
		OLED_Fill(0x00);
	}

	xTaskResumeAll();
	vTaskDelete(NULL);
}

#endif //5d_Key

#ifdef TEST_UART
#include "smartcar/sc_uart.h"

void uart(void *pv)
{
	vTaskSuspendAll();

#ifdef TEST_OLED
	OLED_Init();
	OLED_Fill(0x00);
	OLED_P6x8Str(0, 0, "Init BT test...");
#endif

	char tmp = 0x78;
	UART_Init(LPUART4,115200,80000000);
	while(1)
	{
		UART_WriteBlocking(LPUART4,&tmp,8);
	}


	xTaskResumeAll();
	vTaskDelete(NULL);
}

#endif //TEST_UART


#ifdef TEST_MPU6050
#include "smartcar/sc_ac_MPU6050.h"

/* 声明外部延时函数 */

void test_MPU6050(void)
{
    char  txt[30];
	short aacx,aacy,aacz;	         //加速度传感器原始数据
	short gyrox,gyroy,gyroz;        //陀螺仪原始数据

#if (IMU_USE_SELECT == USE_HAL_IIC)
    I2C_Init(LPI2C1, 100000,10000000);
#elif (IMU_USE_SELECT == USE_SOFT_IIC)
    IIC_Init();
#endif

#ifdef TEST_OLED
	OLED_Init();
	OLED_Fill(0x00);
	OLED_P6x8Str(0,0,"[O K] MPU 6050 Test");
#endif
    PRINTF("[O K] MPU 6050 Test\n");

    if(MPU6050_Init())
    {
#ifdef TEST_OLED
		OLED_P6x8Str(15,2,"[Err] MPU 6050 Test Failed!");	
#endif
    PRINTF("[Err] MPU 6050 Test Failed!\n");
		
		delay_ms(5000);
    }

    while(1)
    {
        MPU_Get_Raw_data(&aacx,&aacy,&aacz,&gyrox,&gyroy,&gyroz);	//得到加速度传感器数据
#ifdef TEST_OLED
		sprintf((char*)txt,"ax:%06d",aacx);
        OLED_P6x8Str(0,2,txt);
        sprintf((char*)txt,"ay:%06d",aacy);
        OLED_P6x8Str(0,3,txt);
        sprintf((char*)txt,"az:%06d",aacz);
        OLED_P6x8Str(0,4,txt);
        sprintf((char*)txt,"gx:%06d",gyrox);
        OLED_P6x8Str(0,5,txt);
        sprintf((char*)txt,"gy:%06d",gyroy);
        OLED_P6x8Str(0,6,txt);
        sprintf((char*)txt,"gz:%06d",gyroz);
        OLED_P6x8Str(0,7,txt);
#endif
        PRINTF("\r\nAX: %d  ",aacx);
        PRINTF("\r\nAY: %d  ",aacy);
        PRINTF("\r\nAZ: %d  ",aacz);
        PRINTF("\r\nGX: %d  ",gyrox);
        PRINTF("\r\nGY: %d  ",gyroy);
        PRINTF("\r\nGZ: %d  ",gyroz);
        delay_ms(100);
        PRINTF("\r\n*********************\r\n");
    }

}

#endif //TEST_MPU6050

#ifdef TEST_AC_PWM

#include "smartcar/sc_ac_pwm.h"
gpio_t test_OE_B = {GPIO5,02U,0 };	//L7 74LVC245_OE_B, 电机PWM相关，低电平有效

void pwm_ac(void *pv)
{
    /*初始化舵机PWM接口  kPWM_PwmA  M3  M4*/
    PWM_AC_Init(PWM2, kPWM_Module_3, kPWM_PwmA,  50);  //M3 M4

    GPIO_Init(&test_OE_B);

    /*初始化电机PWM接口  A8,A9,B9,C9*/
	PWM_AC_Init(PWM2, kPWM_Module_0, kPWM_PwmA,  20000);
	PWM_AC_Init(PWM2, kPWM_Module_0, kPWM_PwmB,  20000);
	PWM_AC_Init(PWM2, kPWM_Module_1, kPWM_PwmA,  20000);
	PWM_AC_Init(PWM2, kPWM_Module_1, kPWM_PwmB,  20000);

    PWM_AC_SetServoDuty(750);  //中值

	//PWM_AC_SetMotorDuty(MOTOR_BACK_L,3000);
	//PWM_AC_SetMotorDuty(MOTOR_BACK_R,3000);

	//GPIO_Write(&test_OE_B, 0);//使能缓冲芯片输出

    while(1)
    {
    	;
    }

	vTaskDelete(NULL);
}




#endif	//TEST_AC_PWM




#endif //EXAMPLES_TEST_H_
