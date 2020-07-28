/*
 * AC_Control.c
 *
 *  Created on: 2020年7月5日
 *      Author: Jerry
 */

#include "AC_Control.h"

extern int Delayed_departure_flag;
extern int Delayed_departure_flag_1;
extern int Stop_Car_flag;
extern int data_identifier;
extern int Boma2_flag;
extern int Control_timenow;
extern int Control_time;
extern uint8_t g_AD_Data[12];
float g_dir_error_1 = 0, g_dir_error_sum = 0, s_dir = 0;

float g_speed_error_left_1 = 0, g_speed_error_right_1 = 0, g_speed_error_left_2 = 0, g_speed_error_right_2 = 0;
float s_speed_left_now = 0, s_speed_right_now = 0, s_speed_left = 0, s_speed_right = 0;
float s_speed_aim_left = 0, s_speed_aim_right = 0;
float s_error_left = 0, s_error_right = 0;
float Differencial = 0;

float Huandao_zhongzhi_figure;
int Huandao_shibie_flag = 0;
float s_error_former[10];

pwm_t my1 = {PWM2, kPWM_Module_0, 16 * 1000, 0, 0, kPWM_HighTrue};	//L,dutyA为正时正转
pwm_t my2 = {PWM2, kPWM_Module_1, 16 * 1000, 0, 0, kPWM_HighTrue};  //R,dutyB为正时正转
gpio_t OE_B = {GPIO5,02U,0 };	//L7 74LVC245_OE_B, 电机PWM相关，低电平有效

void Control_Init()
{
    pwm_t *list[] =
            {
                    &my2,
                    &my1,
                    NULL};
    PWM_Init2(list);
    GPIO_Init(&OE_B);
}

void Dir_Control(void)
{
    float s_error = 0;
    float dir_angle = 0;

    if (data[data_identifier].mode == 0)
    {
    	/*PID*/
        s_error = CAMERA_M - mid_line[data[data_identifier].forward_view];
        s_dir = 0.0001 * (data[data_identifier].dirkp * s_error + data[data_identifier].dirki * g_dir_error_sum + data[data_identifier].dirkd * (s_error - g_dir_error_1));
        g_dir_error_1 = s_error;
        /*获取差速系数*/
        dir_angle = s_dir;    				//暂时以占空比的变化当作舵机偏角
        Differencial = 0.01*SIZE_CONSTANT*tan(dir_angle);
        /*输出占空比*/
        //Servo_Protect(&s_dir); 			//舵机限幅保护(已经在sc_ac_pwm.c中)
        PWM_AC_SetServoDuty((uint16_t)(100*(DIR_M + s_dir)));
    }
    else if (data[data_identifier].mode == 1)
    {
    	/*PID*/
        if(Huandao_shibie_flag==1)
        {
          s_error = (g_AD_Data[0] - g_AD_Data[6]) / (g_AD_Data[0] * g_AD_Data[6]+1);
        }
        else
        {
          s_error = (data[data_identifier].Weight_x * 0.01*((float)  g_AD_Data[0] - (float)g_AD_Data[6]) + data[data_identifier].Weight_y * 0.01*((float)g_AD_Data[1] - (float)g_AD_Data[5])) / ((float)g_AD_Data[0] + (float)g_AD_Data[6]+1);
        }
        s_dir = 10 * (data[data_identifier].dirkp * s_error + data[data_identifier].dirki * g_dir_error_sum + data[data_identifier].dirkd * (s_error - g_dir_error_1));
        g_dir_error_1 = s_error;
        /*获取差速系数*/
        dir_angle = s_dir;    				//暂时以占空比的变化当作舵机偏角
        Differencial = 0.01*SIZE_CONSTANT*tan(dir_angle);
        /*输出占空比*/
        //Servo_Protect(&s_dir);
    	PWM_AC_SetServoDuty((uint16_t)(100*DIR_M + s_dir));
    }

}

void Speed_Control(void)
{
    Running_Time();

    float s_speed_aim = 0;

    /*当前速度*/
    s_speed_right_now = -(ENC_Positionget(ENC2) * FTM_CONSTANT);
    s_speed_left_now = ENC_Positionget(ENC3) * FTM_CONSTANT;

    /*目标速度*/
    s_speed_aim = 0.1*data[data_identifier].speed;
    //s_speed_aim_left = s_speed_aim*(1-data[data_identifier].speedkl*Differencial);
    //s_speed_aim_right = s_speed_aim*(1+data[data_identifier].speedkr*Differencial);

    /*PID*/
    s_error_left = s_speed_aim_left - s_speed_left_now;
    s_error_right = s_speed_aim_right - s_speed_right_now;

    s_speed_left += (data[data_identifier].speedkp_l * (s_error_left - g_speed_error_left_1) + data[data_identifier].speedki_l * s_error_left + data[data_identifier].speedkd_l * (s_error_left - 2 * g_speed_error_left_1 + g_speed_error_left_2));
    s_speed_right += (data[data_identifier].speedkp_r * (s_error_right - g_speed_error_right_1) + data[data_identifier].speedki_r * s_error_right + data[data_identifier].speedkd_r * (s_error_right - 2 * g_speed_error_right_1 + g_speed_error_right_2));

    g_speed_error_left_2 = g_speed_error_left_1;
    g_speed_error_right_2 = g_speed_error_right_1;
    g_speed_error_left_1 = s_error_left;
    g_speed_error_right_1 = s_error_right;

    /*电机限幅*/
    Motor_Protect(&s_speed_left);
    Motor_Protect(&s_speed_right);

    /*出赛道保护*/
    if (data[data_identifier].mode == 1)
    {
        if (g_AD_Data[0] == 0 || g_AD_Data[6] == 0) //电磁出赛道保护
        {
//            Ftm_PWM_Change(FTM0, kFTM_Chnl_0, 20000, 0);
//            Ftm_PWM_Change(FTM0, kFTM_Chnl_1, 20000, 0);
//            Ftm_PWM_Change(FTM0, kFTM_Chnl_2, 20000, 0);
//            Ftm_PWM_Change(FTM0, kFTM_Chnl_3, 20000, 0);

            my1.dutyA = 0;
            my1.dutyB = 0;
            my2.dutyA = 0;
            my2.dutyB = 0;
            PWM_Change(&my1);
            PWM_Change(&my2);

            GPIO_Write(&OE_B, 0);//使能缓冲芯片输出
        }
        else
        {
            Speed_Judge(s_speed_left, s_speed_right);
        }
    }
    else
    {
		Speed_Judge(s_speed_left, s_speed_right);
    }

    Send_Variable();

    ENC_Dateclear(ENC2);
    ENC_Dateclear(ENC3);

}

void Servo_Protect(float *dir)
{
    if (*dir > MAX_SERVO_DUTY)
    {
        *dir = MAX_SERVO_DUTY;
    }
    else if (*dir < -MAX_SERVO_DUTY)
    {
        *dir = -MAX_SERVO_DUTY;
    }
}

void Motor_Protect(float *speed)
{
    if (*speed > MAX_MOTOR_DUTY)
    {
        *speed = MAX_MOTOR_DUTY;
    }
    if (*speed < MIN_MOTOR_DUTY)
    {
        *speed = MIN_MOTOR_DUTY;
    }
}

void Speed_Judge(float speed_L, float speed_R)
{
    if (speed_L >= 0 && speed_R >= 0)
    {
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_0, 20000, 0);
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_1, 20000, 0);
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_2, 20000, speed_L);
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_3, 20000, speed_R);

        my1.dutyA = speed_L;
        my1.dutyB = 0;
        my2.dutyA = 0;
        my2.dutyB = speed_R;

    }
    else if (speed_L >= 0 && speed_R < 0)
    {
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_0, 20000, 0);
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_1, 20000, -speed_R);
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_2, 20000, speed_L);
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_3, 20000, 0);

    	speed_R = -speed_R;
        my1.dutyA = speed_L;
        my1.dutyB = 0;
        my2.dutyA = speed_R;
        my2.dutyB = 0;
    }
    else if (speed_L < 0 && speed_R >= 0)
    {
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_0, 20000, -speed_L);
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_1, 20000, 0);
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_2, 20000, 0);
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_3, 20000, speed_R);

    	speed_L = -speed_L;
        my1.dutyA = 0;
        my1.dutyB = speed_L;
        my2.dutyA = 0;
        my2.dutyB = speed_R;
    }
    else
    {
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_0, 20000, -speed_L);
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_1, 20000, -speed_R);
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_2, 20000, 0);
//        Ftm_PWM_Change(FTM0, kFTM_Chnl_3, 20000, 0);

    	speed_L = -speed_L;
    	speed_R = -speed_R;
        my1.dutyA = 0;
        my1.dutyB = speed_L;
        my2.dutyA = speed_R;
        my2.dutyB = 0;
    }

    PWM_Change(&my1);
    PWM_Change(&my2);

    GPIO_Write(&OE_B, 0);//使能缓冲芯片输出
}

void Running_Time(void)
{
    Control_timenow++;
    Control_time = data[data_identifier].running_time * 50;
    if (Control_timenow > Control_time)
    {
        Delayed_departure_flag_1 = 0;
    } //一个中断20ms，来计算停止的时间
}

void Dir_Control_Zebra_Crossing(void)
{
    //Ftm_PWM_Change(FTM3, kFTM_Chnl_6, 50, DIR_M);
	PWM_AC_SetServoDuty(100*DIR_M);
}

void Dir_Control_Huandao_Shibie(void)
{
  int figure_ad;
  float s_error;
  figure_ad = g_AD_Data[3];
  if(figure_ad > 1.2*Huandao_zhongzhi_figure)
  {
    Huandao_shibie_flag = 1 ;
  }
  else
  {
    Huandao_shibie_flag = 0;
  }

}

void Dir_Control_Zhongxian_Biaoding(void)
{
  if(g_AD_Data[3] > Huandao_zhongzhi_figure)
  {
    Huandao_zhongzhi_figure = g_AD_Data[3];
  }
}
