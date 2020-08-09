#include <ac_lib/AC_Command.h>
#include "peripherals.h"
#include "fsl_lpuart.h"
#include "fsl_debug_console.h"
#include "ac_lib/Image.h"
#include "ac_lib/AC_Control.h"

/*COM Send Buffer: [0-12] are EM data, [13] is boma status*/
volatile uint8_t COM_Buffer_Send[13];

/*COM Send Buffer*/
volatile uint8_t COM_Buffer_Read[13];

/**Uplaod data to PC through bluetooth (UART4), AD_Data + 10* mid_line data+ 1*servo +**/
volatile int8_t COM_BT_Buffer_Send[2 + NUMBER_INDUCTORS + 3 + NUMBER_FORESIGHT + 1];

uint8_t rxIndex = 0;
int Times_L = 0,Times_H = 0;

extern int8_t g_AD_Data[NUMBER_INDUCTORS];
extern int8_t g_Servo_Data;
extern int8_t g_Motor_L_Data;
extern int8_t g_Motor_R_Data;
extern int8_t g_Image_Data[10];


void COM_BT_Putchar(char temp)
{
    UART_WriteBlocking(BT_UART,&temp,1); //根据实际的串口号来修改
}

//void COM_CommandRead_IRQHandler(void)
//{
//    uint8_t data;
//
//    /* If new data arrived. */
//    if ((kLPUART_RxDataRegFullFlag)&LPUART_GetStatusFlags(LPUART2))
//    {
//        data = LPUART_ReadByte(LPUART2);
//
//        /* If ring buffer is not full, add data to ring buffer. */
//        if (rxIndex<13)
//        {
//        	COM_Buffer_Read[rxIndex] = data;
//            rxIndex++;
//        }
//        else
//        {
//			PRINTF("receive start\n");
//			for(int i = 0;i<=16;i++)
//			{
//				PRINTF("%d, ",COM_Buffer_Read[i]);
//			}
//			PRINTF("end\n");
//			rxIndex=0;
//			LPUART_ReadByte()
//        }
//    }
//
//}

/* LPUART2_IRQn interrupt handler */
//void LPUART2_IRQHandler(void) {
//
//  /*  Place your code here */
//
//	PRINTF("interrupt!\n");
//
//	uint8_t data;
//	/* If new data arrived. */
//	if ((kLPUART_RxDataRegFullFlag||kLPUART_RxFifoUnderflowFlag)&LPUART_GetStatusFlags(LPUART2))
//	{
//		//data = LPUART_ReadByte(LPUART2);
//		LPUART_ReadBlocking(LPUART2,&data,1U);
//		PRINTF("%x ",data);
//	}
//
//
//
//  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
//     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
//  #if defined __CORTEX_M && (__CORTEX_M == 4U)
//    __DSB();
//  #endif
//}


void COM_BT_Upload()
{
    COM_BT_Buffer_Send[0] = Times_H;
	COM_BT_Buffer_Send[1] = Times_L;
	if (Times_L == 0xFF)
	{
		Times_L = 0;
		if (Times_H == 0xFF)	Times_H = 0;
		else Times_H ++;
	}
	else Times_L ++;

	/*put EM data buffer here and give it to COM_Buffer_Send*/

	/*电感数据*/
	 for(int i = 0; i < NUMBER_INDUCTORS; i++)
	 {
	 	COM_BT_Buffer_Send[i+2] = g_AD_Data[i];
	 }

//	COM_BT_Buffer_Send[2] = (int8_t)(EM_AD[0][0] - 128);
//	COM_BT_Buffer_Send[3] = (int8_t)(EM_AD[0][1] - 128);
//    COM_BT_Buffer_Send[4] = (int8_t)(EM_AD[0][2] - 128);
//    COM_BT_Buffer_Send[5] = (int8_t)(EM_AD[0][3] - 128);
//    COM_BT_Buffer_Send[6] = (int8_t)(EM_AD[1][0] - 128);
//    COM_BT_Buffer_Send[7] = (int8_t)(EM_AD[1][1] - 128);
//	COM_BT_Buffer_Send[8] = (int8_t)(EM_AD[1][2] - 128);
//
//	COM_BT_Buffer_Send[9] = (int8_t)(EM_AD[1][3] - 128);
//	COM_BT_Buffer_Send[10] = (int8_t)(EM_AD[1][4] - 128);

	/*PWM数据*/
     COM_BT_Buffer_Send[NUMBER_INDUCTORS + 2] = g_Servo_Data;
     COM_BT_Buffer_Send[NUMBER_INDUCTORS + 3] = g_Motor_L_Data;
     COM_BT_Buffer_Send[NUMBER_INDUCTORS + 4] = g_Motor_R_Data;

	/*图像数据*/
	 for(int i = 0; i < NUMBER_FORESIGHT; i++)
	 {
	 	//COM_Buffer_Send[i] = ((middleline[60+2*(i-(NUMBER_INDUCTORS + 5))] - 94)/94)*127;
	 	COM_BT_Buffer_Send[i+NUMBER_INDUCTORS + 5] = g_Image_Data[i];
	 }

	COM_BT_Buffer_Send[NUMBER_INDUCTORS + 5 + NUMBER_FORESIGHT] = 0x5A;

	/*Send Data*/
	COM_BT_Putchar(0xFF);
	COM_BT_Putchar(0xEE);
	COM_BT_Putchar(0xDD);

	for(int i = 0; i < (2 + NUMBER_INDUCTORS + 3 + NUMBER_FORESIGHT + 1); i++)
	{
		COM_BT_Putchar(COM_BT_Buffer_Send[i]);
	}

	COM_BT_Putchar(0xDD);
	COM_BT_Putchar(0xEE);
	COM_BT_Putchar(0xFF);
}

