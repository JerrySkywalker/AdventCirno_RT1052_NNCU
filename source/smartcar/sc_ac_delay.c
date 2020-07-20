#include "sc_ac_delay.h"
#include "clock_config.h"

void delay_init(void)
{
    SysTick->LOAD  = 10000;                         /* set reload register */
        NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
        SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
        SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
//                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
}                                                                    


void delay_us(uint32_t us)
{        
        uint32_t ticks;
        uint32_t told,tnow,tcnt=0;
        uint32_t reload=SysTick->LOAD;                                //LOAD的值                     
        ticks=us * (CLOCK_GetFreq(kCLOCK_CpuClk) / 1000000); //需要的节拍数
        told=SysTick->VAL;                                        //刚进入时的计数器值
        while(1)
        {
                tnow=SysTick->VAL;        
                if(tnow!=told)
                {            
                        if(tnow<told)tcnt+=told-tnow;        //这里注意一下SYSTICK是一个递减的计数器就可以了.
                        else tcnt+=reload-tnow+told;            
                        told=tnow;
                        if(tcnt>=ticks)break;                        //时间超过/等于要延迟的时间,则退出.
                }  
        };
}
  
void delay_ms(uint16_t ms)
{
        uint32_t i;
        for(i=0;i<ms;i++) delay_us(1000);
}