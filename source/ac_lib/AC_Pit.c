#include "AC_Pit.h"
#include "smartcar/sc_pit.h"
#include "smartcar/status.h"
#include "smartcar/sc_pwm.h"
#include "smartcar/sc_gpio.h"
#include "AC_Control.h"
#include "AC_Menu.h"
#include "Image.h"

TaskHandle_t AC_Pit_task_handle;

extern int Flag_InitComplete;

uint16_t duty;  //测试舵机用

/**
 * @brief Init IRQ for AC.
 * @param pv
 * @note    here we start 3 irq services
 *
 *          Camera & image Process-20ms
 *
 *          Servo-20ms
 *
 *          Motor
 * @ingroup Pit
 */
void AC_Pit(void *pv)
{
    PIT_Init2(kPIT_Chnl_0, 20 * 1000);	    //Camera & image Process-20ms
    PIT_Init2(kPIT_Chnl_1, 20 * 1000);      //Servo-20ms
    PIT_Init2(kPIT_Chnl_2, 20 * 1000);      //Motor-20ms
    PIT_Init2(kPIT_Chnl_3, 20 * 1000);      //NULL
    NVIC_SetPriority(PIT_IRQn, 6);	        //设置pit中断优先级为6
    PIT_StartTimer(PIT, kPIT_Chnl_0);
    PIT_StartTimer(PIT, kPIT_Chnl_1);
    PIT_StartTimer(PIT, kPIT_Chnl_2);
    PIT_StartTimer(PIT, kPIT_Chnl_3);

    /*Wait for IRQ*/
    while (1) {
        vTaskDelay(10);
    }

    vTaskDelete(NULL);
}

/**
 * @brief IRQHandler for AC_Pit
 *
 * @note  here we start 3 irq services
 *
 *          Camera & image Process-20ms
 *
 *          Servo-20ms
 *
 *          Motor-20ms
 * @ingroup Pit
 */
/*RAMFUNC_ITC*/ void PIT_IRQHandler(void)
{
    /*清除中断标志位 （要用的时候解注释，通道可换）*/
    if (PIT_GetStatusFlags(PIT, kPIT_Chnl_0) == kPIT_TimerFlag)
    {
        PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);

        /*TODO: Image Capture & Process sequence here */

    }
    if (PIT_GetStatusFlags(PIT, kPIT_Chnl_1) == kPIT_TimerFlag)
    {
        PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag);

        /*TODO: Servo control sequence here */

//        if (Flag_InitComplete == 1)
//        	{
//        		if (data[data_identifier].mode == 0)
//        		{
//        			//Find_Zebra_Crossing();
//
//        			if (Flag_Find == 0)
//        			{
//        				Dir_Control();
//        			}
//        			else if (Flag_Find == 2)
//        			{
//        				Dir_Control_Zebra_Crossing(); //中值
//        			}
//        		}
//        		else if (data[data_identifier].mode == 1)
//        		{
//        			Dir_Control();
//        		}

        		Dir_Control();

//        	}
        	// Ftm_PWM_Change(FTM3, kFTM_Chnl_6, 50, 7.5);

    }
    if (PIT_GetStatusFlags(PIT, kPIT_Chnl_2) == kPIT_TimerFlag)
    {
        PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, kPIT_TimerFlag);

        /*TODO: Motor control sequence here */

        Speed_Control();

    }
    if (PIT_GetStatusFlags(PIT, kPIT_Chnl_3) == kPIT_TimerFlag)
    {
        PIT_ClearStatusFlags(PIT, kPIT_Chnl_3, kPIT_TimerFlag);

        /*TODO: Test code*/

        //Tset Servo
//		if (0 == GPIO_PinRead(GPIO2, 0U))
//		{
//			duty += 1;
//		}
//		if (0 == GPIO_PinRead(GPIO2, 1U))
//		{
//			duty -= 1;
//		}
//        OLED_Fill(0x00);
//        OLED_Print_Num(0, 0, duty);
//        PWM_AC_SetServoDuty(duty);
    }

    /*DO NOT DELETE */
    /*中断服务函数内容*/
    __DSB();
    /*DSB--数据同步屏障
     * 作用：因为CPU时钟的主频大于IP总线时钟故会出现中断标志位还未被清掉时中断服务函数中的内容会执行完，而使得程序一遍一遍的进入中断服务函数，
     * 而DSB则是为了预防这一情况的发生。*/
}
