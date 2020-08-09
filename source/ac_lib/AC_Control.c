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
extern int8_t g_AD_Data[NUMBER_INDUCTORS];
extern uint8_t g_Switch_Data;
extern int16_t g_AD_nncu_Output[3];
extern uint8_t Stop_Flag;
extern int g_AD_nncu_RoadType;
float g_dir_error_1 = 0, g_dir_error_sum = 0, s_dir = 0;

float g_speed_error_left_1 = 0, g_speed_error_right_1 = 0, g_speed_error_left_2 = 0, g_speed_error_right_2 = 0;
float s_speed_left_now = 0, s_speed_right_now = 0, s_speed_left = 0, s_speed_right = 0;
float s_speed_aim_left = 0, s_speed_aim_right = 0;
float s_error_left = 0, s_error_right = 0;
float Differencial = 0;

extern uint8_t EM_AD[NUMBER_INDUCTORS];
int16_t middleline_nncu;
int16_t MID_Sample[MID_CollectTimes];
int16_t MID_Temp;
float Huandao_zhongzhi_figure;
int Huandao_shibie_flag = 0;
int Zhidao_shibie_flag = 0; //0表示非弯道，1表示识别
int Shizi_shibie_flag = 0;
int Shexingwan_jishi_flag = 0;
int Shexingwan_tuolijishi_flag;

float s_error_yuanshi_H;     //横电感的原始偏差
int s_dir_flag=0;
int s_dir_1_flag=0;


int flag_zhidao=0;
int flag_wandao=0;
int flag_shexingwan=0;
int flag_shizi=0;
int flag_huandao=0;
int flag_zhuangtai;
int flag_zhuangtai_former;
int flag_jishi=0;//1计时开始
int flag_shijian = 0;
int flag_wandao_panduan=0;
int flag_wandao_zhixing=0;
pwm_t my1 = {PWM2, kPWM_Module_0, 20 * 1000, 0, 0, kPWM_HighTrue};	//L,dutyA为正时正转
pwm_t my2 = {PWM2, kPWM_Module_1, 20 * 1000, 0, 0, kPWM_HighTrue};  //R,dutyB为正时正转
gpio_t OE_B = {GPIO5,02U,0 };										//L7 74LVC245_OE_B, 电机PWM相关，低电平有效

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
    float AD_fenmu_H;
    float AD_fenmu_S;
    float s_error_H;
    float s_error_S;
    float dir_angle = 0;

    /*摄像头&AI模式*/
    if (data[data_identifier].mode == 0)
    {
    	/*PID*/
        //s_error = CAMERA_M - mid_line[data[data_identifier].forward_view];
    	middleline_nncu = (g_AD_nncu_Output[2]/data[data_identifier].NNCU_NormalizeFactor)+20;
    	if (middleline_nncu > 160)	middleline_nncu = 160;
    	else if (middleline_nncu < 28) middleline_nncu = 28;
    	//Middleline_Filter();
    	s_error = CAMERA_M - middleline_nncu;
        s_dir = 0.0001 * (data[data_identifier].dirkp * s_error + data[data_identifier].dirki * g_dir_error_sum + data[data_identifier].dirkd * (s_error - g_dir_error_1));
        g_dir_error_1 = s_error;

        /*舵机限幅保护*/
        Servo_Protect(&s_dir);

        /*获取差速系数*/
        dir_angle = s_dir;    				//以占空比的变化当作舵机偏角
        Differencial = 0.01*SIZE_CONSTANT*tan(dir_angle);

        /*输出占空比*/
        PWM_AC_SetServoDuty((uint16_t)(100*(DIR_M + s_dir)));
    }

    /*电磁模式*/
    else if (data[data_identifier].mode == 1)
    {

    	/**/
        if (EM_AD[0] + EM_AD[6] != 0)
        {
        	AD_fenmu_H = EM_AD[0] + EM_AD[6];
        }
        else
        {
        	AD_fenmu_H = 200; //防止分母为零
        }
        if (EM_AD[2] + EM_AD[4] != 0)
        {
        	AD_fenmu_S = EM_AD[2] + EM_AD[4];
        }
        else
        {
        	AD_fenmu_S = 200; //防止分母为零
        }

    	Dir_Control_Huandao_Shibie();

    	/*十字识别*/
        if ((EM_AD[1] >= 150) && (EM_AD[5] >= 150))
        {
        	Shizi_shibie_flag = 1;
        }
        else
        {
        	Shizi_shibie_flag = 0;
        }

        /*判据选择*/
        if ((Shizi_shibie_flag == 1) || (Huandao_shibie_flag == 1)) //十字特殊通过方法
        {
        	s_error = (EM_AD[0] - EM_AD[6]) / (EM_AD[0] + EM_AD[6] + 1); 
        	//s_error = 0.5 * s_error_1 + 0.5 * s_error;
        }
        else
        {
        	s_error_H = ((EM_AD[0] - EM_AD[6]) / AD_fenmu_H);
        	s_error_S = ((EM_AD[2] - EM_AD[4]) / AD_fenmu_S);
        	s_error = s_error_H * data[data_identifier].Weight_x + s_error_S * data[data_identifier].Weight_y;
        }
        int a = 0;
        if (s_error > 0)
        {
        	a = 1;
        }
        else
        {
        	a = -1;
        }
        s_error = a * s_error * s_error;//偏差平方

        /*PID*/
        if ((Shizi_shibie_flag == 1) || (Huandao_shibie_flag == 1))
        {
        	s_dir = 0.001*data[data_identifier].dirkp_z * s_error + data[data_identifier].dirki_z * g_dir_error_sum + data[data_identifier].dirkd_z * (s_error - g_dir_error_1);
        	g_dir_error_1 = s_error;
        }
        else
        {
        	s_dir = 0.000001 * (data[data_identifier].dirkp * s_error + data[data_identifier].dirki * g_dir_error_sum + data[data_identifier].dirkd * (s_error - g_dir_error_1));
        	g_dir_error_1 = s_error;
        }



        /*
        s_error_H = ((EM_AD[0] - EM_AD[6]) / AD_fenmu_H);
        s_error_S = ((EM_AD[2] - EM_AD[4]) / AD_fenmu_S);
        s_error = s_error_H * data[data_identifier].Weight_x + s_error_S * data[data_identifier].Weight_y;
        if(a>=data[data_identifier].zhidao_yuzhi)//直道
        {
            s_error=s_error_H;
            s_dir = 0.001*data[data_identifier].dirkp_z * s_error + data[data_identifier].dirki_z * g_dir_error_sum + data[data_identifier].dirkd_z * (s_error - g_dir_error_1);
        	g_dir_error_1 = s_error;
            flag_zhidao=1;
            flag_wandao=0;
            flag_shexingwan=0;
            flag_shizi=0
            flag_huandao=0;
        }
        else if ((b>=data[data_identifier].wandao_yuzhi)&&(a>=))//弯前减速,注意多种判据组合
        {
            s_error = s_error_H * data[data_identifier].Weight_x + s_error_S * data[data_identifier].Weight_y;
            s_dir = 0.001*data[data_identifier].dirkp_w * s_error + data[data_identifier].dirki_w * g_dir_error_sum + data[data_identifier].dirkd_w * (s_error - g_dir_error_1);
        	g_dir_error_1 = s_error;
            flag_zhidao=0;
            flag_wandao=1;
            flag_shexingwan=0;
            flag_shizi=0
            flag_huandao=0;

        }
        else if(c>=data[data_identifier].shexingwan_yuzhi)
        {
            s_error=s_error_S;
            s_dir = 0.001*data[data_identifier].dirkp_sw * s_error + data[data_identifier].dirki_sw * g_dir_error_sum + data[data_identifier].dirkd_sw * (s_error - g_dir_error_1);
        	g_dir_error_1 = s_error;
            flag_zhidao=0;
            flag_wandao=0;
            flag_shexingwan=1;
            flag_shizi=0;
            flag_huandao=0;

        }
        else if (d>=data[data_identifier].shizi_yuzhi||e>=data[data_identifier].huandao_yuzhi)
        {
            s_error=s_error_H;
            s_dir = 0.001*data[data_identifier].dirkp_z * s_error + data[data_identifier].dirki_z * g_dir_error_sum + data[data_identifier].dirkd_z * (s_error - g_dir_error_1);
        	g_dir_error_1 = s_error;
            flag_zhidao=0;
            flag_wandao=0;
            flag_shexingwan=0;
            flag_shizi=1;
            flag_huandao=1;

        }
        else
        {
            s_error = s_error_H ;
            s_dir = 0.001*data[data_identifier].dirkp * s_error + data[data_identifier].dirki * g_dir_error_sum + data[data_identifier].dirkd * (s_error - g_dir_error_1);
        	g_dir_error_1 = s_error;
            flag_zhidao=0;
            flag_wandao=0;
            flag_shexingwan=0;
            flag_shizi=0;
            flag_huandao=0;
        }
        */

    	/*PID Old*/
//        s_error = ((EM_AD[0]+128) - (EM_AD[6]+128)) / ((EM_AD[0]+128) * (EM_AD[6]+128)+1);
//        s_dir = 10 * (data[data_identifier].dirkp * s_error + data[data_identifier].dirki * g_dir_error_sum + data[data_identifier].dirkd * (s_error - g_dir_error_1));
//        g_dir_error_1 = s_error;

        /*获取差速系数*/
        Servo_Protect(&s_dir);
        dir_angle = s_dir;    				//以占空比的变化当作舵机偏角
        Differencial = 0.01*SIZE_CONSTANT*tan(dir_angle);

        /*输出占空比*/
        PWM_AC_SetServoDuty((uint16_t)(100*DIR_M + 100*s_dir));
    }
}

void Speed_Control(void)
{
    //Running_Time();

    float s_speed_aim = 0;

    /*当前速度*/
//    if (ENC_Positionget(ENC3) > 11550)	s_speed_left_now = 0;
//    else s_speed_left_now = ENC_Positionget(ENC3) * FTM_CONSTANT;
//    if (-ENC_Positionget(ENC2) > 11550) s_speed_right_now = 0;
//    else s_speed_right_now = -ENC_Positionget(ENC2) * FTM_CONSTANT;

    s_speed_left_now = ENC_Positionget(ENC3) * FTM_CONSTANT;
    s_speed_right_now = -ENC_Positionget(ENC2) * FTM_CONSTANT;

    /*摄像头&AI模式*/
    if (data[data_identifier].mode == 0)
    {
        if (Stop_Flag == 1)
        {
        	s_speed_aim=0;
        }
        else if (g_AD_Data[0] <= -123 && g_AD_Data[6] <= -123)
        {
          s_speed_aim = 0;
        }
        else
        {
          s_speed_aim = 0.1 * data[data_identifier].speed;
        }

    }

    /*电磁模式*/
    else if (data[data_identifier].mode == 1)
    {
        // if (Stop_Flag == 1)

            s_speed_aim=0.1 * data[data_identifier].speed;

    }

    s_speed_aim_left = s_speed_aim*(1-data[data_identifier].speedkl*Differencial);
    s_speed_aim_right = s_speed_aim*(1+data[data_identifier].speedkr*Differencial);

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

    Speed_Judge(s_speed_left, s_speed_right);

    //Send_Variable();

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

#if defined (CAR_NAME) && (CAR_NAME == 0)
    if (speed_L >= 0 && speed_R >= 0)
    {

        my1.dutyA = speed_L;
        my1.dutyB = 0;
        my2.dutyA = 0;
        my2.dutyB = speed_R;

    }
    else if (speed_L >= 0 && speed_R < 0)
    {

    	speed_R = -speed_R;
        my1.dutyA = speed_L;
        my1.dutyB = 0;
        my2.dutyA = speed_R;
        my2.dutyB = 0;
    }
    else if (speed_L < 0 && speed_R >= 0)
    {

    	speed_L = -speed_L;
        my1.dutyA = 0;
        my1.dutyB = speed_L;
        my2.dutyA = 0;
        my2.dutyB = speed_R;
    }
    else
    {

    	speed_L = -speed_L;
    	speed_R = -speed_R;
        my1.dutyA = 0;
        my1.dutyB = speed_L;
        my2.dutyA = speed_R;
        my2.dutyB = 0;
    }

#else if defined (CAR_NAME) && (CAR_NAME == 1)

    if (speed_L >= 0 && speed_R >= 0)
    {

        my1.dutyA = 0;
        my1.dutyB = speed_L;
        my2.dutyA = 0;
        my2.dutyB = speed_R;

    }
    else if (speed_L >= 0 && speed_R < 0)
    {

    	speed_R = -speed_R;
        my1.dutyA = 0;
        my1.dutyB = speed_L;
        my2.dutyA = speed_R;
        my2.dutyB = 0;
    }
    else if (speed_L < 0 && speed_R >= 0)
    {

    	speed_L = -speed_L;
        my1.dutyA = speed_L;
        my1.dutyB = 0;
        my2.dutyA = 0;
        my2.dutyB = speed_R;
    }
    else
    {

    	speed_L = -speed_L;
    	speed_R = -speed_R;
        my1.dutyA = speed_L;
        my1.dutyB = 0;
        my2.dutyA = speed_R;
        my2.dutyB = 0;
    }

#endif

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

void Jishi_Time(void)
{
  flag_shijian++;
  //Control_time = data[data_identifier].running_time * 50;
  // if (Wandao_shibie_huicha_flag > 20)
  // {
  //   Shexingwan_tuolijishi_flag = 1;
  // } //一个中断20ms，来计算停止的时间
}

void Dir_Control_Huandao_Shibie(void)
{
  int figure_ad;
  float s_error;
  figure_ad = EM_AD[3];
  //if(figure_ad > 1.4*Huandao_zhongzhi_figure)
  if (figure_ad > 130)
  {
    Huandao_shibie_flag = 1;
  }
  else
  {
    Huandao_shibie_flag = 0;
  }
}

void Dir_Control_Zhongxian_Biaoding(void)
{
  if (EM_AD[3] > Huandao_zhongzhi_figure)
  {
    Huandao_zhongzhi_figure = EM_AD[3];
  }
}

////////////////////////////////////////////////////////////
//作用：中值滤波
//输入：NULL
//输出：NULL
////////////////////////////////////////////////////////////
void Middleline_Filter(void)
{
	for (uint8_t i = 0; i <= MID_CollectTimes - 1; i++)
	{
		MID_Sample[i] = (g_AD_nncu_Output[2]/data[data_identifier].NNCU_NormalizeFactor)+10;
	}
	for (uint8_t i = 0; i <= MID_CollectTimes - 1; i++)
	{
	      if (MID_Sample[i] > 188)
	      {
	        MID_Sample[i] = 188;
	      }
	      if (MID_Sample[i] < 0)
	      {
	        MID_Sample[i] = 0;
	      }
	}
    for (uint8_t i = 0; i <= MID_CollectTimes - 2; i++)
    {
    	for (uint8_t j = i + 1; j <= MID_CollectTimes - 1; j++)
    	{
    		if (MID_Sample[i] > MID_Sample[j])
    			swap(&MID_Sample[i], &MID_Sample[j]);
    	}
    }
    MID_Temp = 0;
    for (uint8_t i = 3; i <= MID_CollectTimes - 4; i++)
    {
    	MID_Temp += MID_Sample[i];
    }
    MID_Temp = MID_Temp / (MID_CollectTimes - 6);
    middleline_nncu = MID_Temp;
}//Middleline_Filter()

////////////////////////////////////////////////////////////
//作用：交换函数
//输入：NULL
//输出：NULL
////////////////////////////////////////////////////////////
void swap(uint16_t* x,uint16_t* y)
{
    uint16_t temp=0;
    temp=*x;
    *x=*y;
    *y=temp;
}//swap
