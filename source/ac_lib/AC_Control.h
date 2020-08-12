/*
 * AC_Control.h
 *
 *  Created on: 2020年7月5日
 *      Author: Jerry
 */

#ifndef AC_LIB_AC_CONTROL_H_
#define AC_LIB_AC_CONTROL_H_

#include "AC_Menu.h"
#include "ac_lib/Image.h"
#include "smartcar/sc_pwm.h"
#include "smartcar/sc_gpio.h"
#include "smartcar/sc_enc.h"
#include <math.h>

#define CAMERA_M  94
#define CAMERA_H  120
#define CAMERA_W  188
#define DIR_M 7.35/*7.77*/
#define MAX_SERVO_DUTY 0.9                        //舵机限幅
#define MAX_MOTOR_DUTY 95                        //电机限幅
#define MOTOR_START_LIMIT 90
#define FTM_CONSTANT 0.0216						//1/(11550*0.004)=1/((走1米编码器的返回值)*控制周期);K66:2890;RT1052:11550
#define SIZE_CONSTANT 15.5/(19.9*2)             //  B/2L，B是两后轮间距，L是前后轮间距
#define CAR_NAME 0
#define MID_CollectTimes 25
#define NUMBER_INDUCTORS 9
#define MOTOR_START_LIMIT_STEP 1000

void Control_Init();
void Running_Time(void);
void Dir_Control(void);
void Speed_Control(void);
void Servo_Protect(float *dir);
void Motor_Protect(float *speed);
void Speed_Judge(float speed_L, float speed_R);
void step_cnt(int *stepcnt,int stepcnt_flag);

extern Data_t data[10];
extern float s_speed_aim_left;
extern float s_speed_aim_right;
extern float s_speed_left_now;
extern float s_speed_right_now;
extern float s_speed_left;
extern float s_speed_right;

#endif /* AC_LIB_AC_CONTROL_H_ */
