/*** 
 * @Author: JerryW
 * @Date: 2020-06-21 16:56:04
 * @LastEditTime: 2020-06-21 17:10:46
 * @LastEditors: JerryW
 * @Description: 
 * @FilePath: /AdventCirno_RT1052/source/smartcar/sc_ac_pwm.h
 */
#ifndef SMARTCAR_SC_AC_PWM_H_
#define SMARTCAR_SC_AC_PWM_H_

#include "fsl_debug_console.h"
#include "fsl_pwm.h"
#include "MIMXRT1052.h"

//AC custom

/*Motor definition*/
typedef enum MOTOR
{
    MOTOR_BACK_L,
    MOTOR_BACK_R,
}MOTOR_t;


void PWM_AC_Init(PWM_Type *base, pwm_submodule_t subModule, pwm_channels_t pwm_channels, uint16_t Frequency);

void PWM_AC_SetServoDuty(uint16_t duty);

void PWM_AC_SetMotorDuty(MOTOR_t motor, short duty);

void PWM_AC_SetDuty(PWM_Type *base, pwm_submodule_t subModule, pwm_channels_t pwm_channels, uint16_t duty);

void PWM_AC_SetABDuty(PWM_Type *base, pwm_submodule_t subModule, uint16_t dutyA, uint16_t dutyB);

void PWM_AC_UpdateDuty(PWM_Type *base,pwm_submodule_t subModule,pwm_channels_t pwmSignal,
                            pwm_mode_t currPwmMode,uint16_t duty);







#endif /* SMARTCAR_SC_AC_PWM_H_ */
