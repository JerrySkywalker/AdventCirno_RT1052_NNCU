/*
 * sc_ac_pwm.c
 *
 *  Created on: 2020年6月19日
 *      Author: Jerry
 */

#include "sc_ac_pwm.h"
#include "MIMXRT1052.h"
#include "ac_lib/AC_Control.h"

/* 最大占空比 可自行修改 */
/*注意以下的duty换算单位均为万分之一。比如7.5% 在duty中写作750*/
#define AC_DUTY_MAX 9000

/**
  * @brief    PWM初始化
  * @param    base          ：PWM模块 PWM1~PWM4
  * @param    subModule     ：PWM模块下的子模块 kPWM_Module_0 ~ kPWM_Module_3
  * @param    pwm_channels  ：通道选择，kPWM_PwmA/kPWM_PwmB，一般不用kPWM_PwmX
  * @param    Frequency     ：频率，舵机用50，电机用20k
  * @return
  * @example  PWM_AC_Init(PWM2, kPWM_Module_3, kPWM_PwmA,  50);  //M3 M4
  */

void PWM_AC_Init(PWM_Type *base, pwm_submodule_t subModule, pwm_channels_t pwm_channels, uint16_t Frequency)
{
    uint32_t pwmSourceClockInHz; //PWM时钟源
    pwmSourceClockInHz = CLOCK_GetFreq(kCLOCK_IpgClk);

    pwm_config_t pwmConfig; // PWM初始化结构体
    /*pwmConfig默认参数
    * pwmConfig.enableDebugMode = false;
    * pwmConfig.enableWait = false;
    * pwmConfig.reloadSelect = kPWM_LocalReload;
    * pwmConfig.faultFilterCount = 0;
    * pwmConfig.faultFilterPeriod = 0;
    * pwmConfig.clockSource = kPWM_BusClock;
    * pwmConfig.prescale = kPWM_Prescale_Divide_1;
    * pwmConfig.initializationControl = kPWM_Initialize_LocalSync;
    * pwmConfig.forceTrigger = kPWM_Force_Local;
    * pwmConfig.reloadFrequency = kPWM_LoadEveryOportunity;
    * pwmConfig.reloadLogic = kPWM_ReloadImmediate;
    * pwmConfig.pairOperation = kPWM_Independent;        //PWMA和PWMB独立输出
    */
    PWM_GetDefaultConfig(&pwmConfig); //得到默认的PWM初始化结构体

    pwmConfig.reloadLogic = kPWM_ReloadPwmFullCycle; //循环输出

    pwmConfig.enableDebugMode = true;

    /* 计算最佳分频系数 */
    uint32_t temp = pwmSourceClockInHz / Frequency;
    uint8_t div = 0;
    while (temp > 0xFFFF)
    {
        div++;
        temp = temp >> 1;
    }
    assert(div < 8);

    pwmConfig.prescale = (pwm_clock_prescale_t)div;

    if (PWM_Init(base, subModule, &pwmConfig) == kStatus_Fail)
    {
        PRINTF("PWM subModule %d initialization failed\n", subModule);
        return;
    }
    base->SM[subModule].DISMAP[0] = 0; //屏蔽故障检测功能

    pwm_signal_param_t pwmSignal[2]; //PWM子模块初始化结构体
    uint16_t deadTimeVal = 0;
    /* Set deadtime count, we set this to about 650ns */
    //  deadTimeVal = ((uint64_t)pwmSourceClockInHz * 650) / 1000000000;//死区设定 多用于无刷电机PWMA和PWMB互补输出时使用

    pwmSignal[0].pwmChannel = kPWM_PwmA; //默认使用通道A
    if (pwm_channels == kPWM_PwmB)       //如果使用通道B
    {
        pwmSignal[0].pwmChannel = kPWM_PwmB; // 使用PWMB
    }
    pwmSignal[0].level = kPWM_HighTrue;       //输出电平为高电平
    pwmSignal[0].dutyCyclePercent = 0;        //初始占空比 0%
    pwmSignal[0].deadtimeValue = deadTimeVal; //死区时间

    /*当AB两通道同时使用 才有作用*/
    pwmSignal[1].pwmChannel = kPWM_PwmB;      // 使用PWMB
    pwmSignal[1].level = kPWM_HighTrue;       //输出电平为高电平
    pwmSignal[1].dutyCyclePercent = 0;        //初始占空比 0%
    pwmSignal[1].deadtimeValue = deadTimeVal; //死区时间

    /*设置pwm的时钟 = pwmSourceClockInHz，频率 = Frequency 对齐方式 = kPWM_SignedCenterAligned，*/
    PWM_SetupPwm(base, subModule, pwmSignal, 1, kPWM_SignedCenterAligned, Frequency, pwmSourceClockInHz);

    PWM_SetPwmLdok(base, 1u << subModule, true); //设置pwm的 load ok位

    PWM_StartTimer(base, 1u << subModule); //开启定时器
}

/**
  * @brief    （舵机专用）设置舵机的占空比
  * @param    duty         ：占空比,注意这个duty换算单位均为万分之一。比如7.5% 在duty中写作750
  * @return
  * @example  PWM_AC_SetServoDuty(750);  //舵机中值
  */
void PWM_AC_SetServoDuty(uint16_t duty)
{
    if (duty > 100*(DIR_M + MAX_SERVO_DUTY))
    {
        duty = 100*(DIR_M + MAX_SERVO_DUTY); //限幅，可自行修改
        PWM_AC_SetABDuty(PWM2, kPWM_Module_3, duty, duty);
    }
    else if (duty < 100*(DIR_M - MAX_SERVO_DUTY))
    {
    	duty = 100*(DIR_M - MAX_SERVO_DUTY);
    	PWM_AC_SetABDuty(PWM2, kPWM_Module_3, duty, duty);
    }
    else
    {
    	PWM_AC_SetABDuty(PWM2, kPWM_Module_3, duty, duty); //M3 M4
    }
}

/**
  * @brief    (电机使用)设置占空比
  * @param    base          ：PWM模块 PWM1~PWM4
  * @param    subModule     ：PWM模块下的子模块 kPWM_Module_0 ~ kPWM_Module_3
  * @param    pwm_channels  ：PWM模块下的子模块 的通道 ： kPWM_PwmA 、 kPWM_PwmB  、 kPWM_PwmA_B
  * @param    duty          ：占空比*DUTY_MAX
  * @return
  * @note     默认对齐方式kPWM_SignedCenterAligned， 可以自行修改
  * @example  PWM_SetDuty(PWM1,    kPWM_Module_0, kPWM_PwmA, 500); //设置 PWM1 子模块0的通道A 占空比= 500/DUTY_MAX
  */
inline void PWM_AC_SetDuty(PWM_Type *base, pwm_submodule_t subModule, pwm_channels_t pwm_channels, uint16_t duty)
{
    assert(duty <= AC_DUTY_MAX);
    base->MCTRL |= PWM_MCTRL_CLDOK(1u << subModule);
    PWM_AC_UpdateDuty(base, subModule, pwm_channels, kPWM_SignedCenterAligned, duty);
    base->MCTRL |= PWM_MCTRL_LDOK(1u << subModule);
}

/**
  * @brief    （舵机专用）同时设置 一个子模块的A 和 B相  用这个函数
  * @param    base          ：PWM模块 PWM1~PWM4
  * @param    subModule     ：PWM模块下的子模块 kPWM_Module_0 ~ kPWM_Module_3
  * @param    dutyA         ：占空比*DUTY_MAX
  * @param    dutyB         ：占空比*DUTY_MAX
  * @return
  * @note     同时设置 一个子模块的A 和 B相 可以 用这个函数
  * @note     默认对齐方式kPWM_SignedCenterAligned， 可以自行修改
  * @example  PWM_SetABDuty(PWM2, kPWM_Module_2, 500, 1000); //设置 PWM1 子模块0的通道A 占空比= 500/DUTY_MAX 子模块0的通道B 占空比= 1000/DUTY_MAX
  * @date     2019/5/23 星期四
  */
void PWM_AC_SetABDuty(PWM_Type *base, pwm_submodule_t subModule, uint16_t dutyA, uint16_t dutyB)
{
    base->MCTRL |= PWM_MCTRL_CLDOK(1u << subModule);
    PWM_AC_UpdateDuty(base, subModule, kPWM_PwmA, kPWM_SignedCenterAligned, dutyA);
    PWM_AC_UpdateDuty(base, subModule, kPWM_PwmB, kPWM_SignedCenterAligned, dutyB);
    base->MCTRL |= PWM_MCTRL_LDOK(1u << subModule);
}

/**
  * @brief    改变占空比
  *
  * @param    base       ：PWM模块 PWM1~PWM4
  * @param    subModule  ：PWM模块下的子模块 kPWM_Module_0 ~ kPWM_Module_3
  * @param    pwmSignal  ：kPWM_PwmA/kPWM_PwmB
  * @param    currPwmMode：对齐方式
  * @param    duty       ：占空比*DUTY_MAX
  *
  * @return   无
  *
  * @note     内部调用
  *
  * @example
  *
  * @date     2019/5/23 星期四
  */
void PWM_AC_UpdateDuty(PWM_Type *base,
                       pwm_submodule_t subModule,
                       pwm_channels_t pwmSignal,
                       pwm_mode_t currPwmMode,
                       uint16_t duty)
{
    assert(pwmSignal < 2);
    uint16_t pulseCnt = 0, pwmHighPulse = 0;
    int16_t modulo = 0;

    switch (currPwmMode)
    {
    case kPWM_SignedCenterAligned:
        modulo = base->SM[subModule].VAL1;
        pulseCnt = modulo * 2;
        /* Calculate pulse width */
        pwmHighPulse = (pulseCnt * duty) / AC_DUTY_MAX; //占空比 =  duty / DUTY_MAX 可修改

        /* Setup the PWM dutycycle */
        if (pwmSignal == kPWM_PwmA)
        {
            base->SM[subModule].VAL2 = (-(pwmHighPulse / 2));
            base->SM[subModule].VAL3 = (pwmHighPulse / 2);
        }
        else
        {
            base->SM[subModule].VAL4 = (-(pwmHighPulse / 2));
            base->SM[subModule].VAL5 = (pwmHighPulse / 2);
        }
        break;
    case kPWM_CenterAligned:
        pulseCnt = base->SM[subModule].VAL1;
        /* Calculate pulse width */
        pwmHighPulse = (pulseCnt * duty) / AC_DUTY_MAX; //占空比 =  duty / DUTY_MAX 可修改

        /* Setup the PWM dutycycle */
        if (pwmSignal == kPWM_PwmA)
        {
            base->SM[subModule].VAL2 = ((pulseCnt - pwmHighPulse) / 2);
            base->SM[subModule].VAL3 = ((pulseCnt + pwmHighPulse) / 2);
        }
        else
        {
            base->SM[subModule].VAL4 = ((pulseCnt - pwmHighPulse) / 2);
            base->SM[subModule].VAL5 = ((pulseCnt + pwmHighPulse) / 2);
        }
        break;
    case kPWM_SignedEdgeAligned:
        modulo = base->SM[subModule].VAL1;
        pulseCnt = modulo * 2;
        /* Calculate pulse width */
        pwmHighPulse = (pulseCnt * duty) / AC_DUTY_MAX; //占空比 =  duty / DUTY_MAX 可修改

        /* Setup the PWM dutycycle */
        if (pwmSignal == kPWM_PwmA)
        {
            base->SM[subModule].VAL2 = (-modulo);
            base->SM[subModule].VAL3 = (-modulo + pwmHighPulse);
        }
        else
        {
            base->SM[subModule].VAL4 = (-modulo);
            base->SM[subModule].VAL5 = (-modulo + pwmHighPulse);
        }
        break;
    case kPWM_EdgeAligned:
        pulseCnt = base->SM[subModule].VAL1;
        /* Calculate pulse width */
        pwmHighPulse = (pulseCnt * duty) / AC_DUTY_MAX; //占空比 =  duty / DUTY_MAX 可修改

        /* Setup the PWM dutycycle */
        if (pwmSignal == kPWM_PwmA)
        {
            base->SM[subModule].VAL2 = 0;
            base->SM[subModule].VAL3 = pwmHighPulse;
        }
        else
        {
            base->SM[subModule].VAL4 = 0;
            base->SM[subModule].VAL5 = pwmHighPulse;
        }
        break;
    default:
        break;
    }
}

/**
  * @brief    (电机专用)设置电机占空比
  * @param    motor：   L：左后电机： 使用 A8,A9
  *                     R：右后电机： 使用 B9,C9
  * @param    duty     ：占空比，单位为万分之一，例如10%占空比写作1000
  * @return
  * @note     电机正反转可以调节电机接线调整，也可以控制PWM调整  如果电机正反转和程序中定义的不一样，可以自行修改
  * @example
  */
void PWM_AC_SetMotorDuty(MOTOR_t motor, short duty)
{
    assert(motor < 2);

    switch (motor)
    {
    case MOTOR_BACK_L:
        if (duty >= 0)
        {
            PWM_AC_SetDuty(PWM2, kPWM_Module_0, kPWM_PwmB, 0);
            PWM_AC_SetDuty(PWM2, kPWM_Module_0, kPWM_PwmA, duty);
        }
        else
        {
            PWM_AC_SetDuty(PWM2, kPWM_Module_0, kPWM_PwmB, -duty);
            PWM_AC_SetDuty(PWM2, kPWM_Module_0, kPWM_PwmA, 0);
        }
        break;

    case MOTOR_BACK_R:
        if (duty >= 0)
        {
            PWM_AC_SetDuty(PWM2, kPWM_Module_1, kPWM_PwmB, 0);
            PWM_AC_SetDuty(PWM2, kPWM_Module_1, kPWM_PwmA, duty);
        }
        else
        {
            PWM_AC_SetDuty(PWM2, kPWM_Module_1, kPWM_PwmB, -duty);
            PWM_AC_SetDuty(PWM2, kPWM_Module_1, kPWM_PwmA, 0);
        }
        break;
    }
}
