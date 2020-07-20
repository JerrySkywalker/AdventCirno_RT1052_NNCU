#include <ac_lib/AC_Command.h>
#include "peripherals.h"
#include "fsl_lpuart.h"
#include "fsl_debug_console.h"
#include "ac_lib/Image.h"

/*COM Send Buffer: [0-12] are EM data, [13] is boma status*/
volatile uint8_t COM_Buffer_Send[13];

/*COM Send Buffer*/
volatile uint8_t COM_Buffer_Read[13];

uint8_t rxIndex = 0;

void COM_CommandSend()
{

}


void COM_CommandRead_IRQHandler(void)
{
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

}

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
