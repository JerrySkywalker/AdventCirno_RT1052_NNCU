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
extern bee_t Bee_GPIO;
extern uint8_t EM_AD[NUMBER_INDUCTORS];
extern volatile int8_t g_Servo_Devia;
extern volatile int8_t g_Motor_Devia;

/*舵机相关*/
float g_error = 0, g_dir_error_1 = 0, g_dir = 0;
float Kp,Kd;

/*电机相关*/
float g_speed_error_left_1 = 0, g_speed_error_right_1 = 0, g_speed_error_left_2 = 0, g_speed_error_right_2 = 0;
float s_speed_left_now = 0, s_speed_right_now = 0, s_speed_left = 0, s_speed_right = 0;
float s_speed_aim_left = 0, s_speed_aim_right = 0;
float s_error_left = 0, s_error_right = 0;
float Differencial = 0;
float Motor_Kp_L,Motor_Ki_L,Motor_Kp_R,Motor_Ki_R;

float g_error_motor;//电机前瞻处图像原始偏差
float g_dir_error_motor_1;//上次电机前瞻处图像原始偏差(滤波用)
float g_dir_motor = 0;//电机前瞻处PID输出的舵机
float PWM_Dir_Motor = 0;//除以100后是电机前瞻处舵机的占空比
float speed_pid_out_l, speed_pid_out_r;
float speed_expect_l, speed_expect_r;

//变速
float speed_expect_now = 0;
float speed_expect = 0;
float speed_change_flag = 0;
float speed_change_time = 1.0;

/*赛道类型，0不是，1是*/
int Round_flag = 0;
int Straight_Flag = 0;
int Cross_flag = 0;
int State_Change_Flag = 0;
int Round_dir = 0;

extern uint8_t EM_AD[NUMBER_INDUCTORS];
int16_t middleline_nncu;

pwm_t my1 = {PWM2, kPWM_Module_0, 16 * 1000, 0, 0, kPWM_HighTrue};	//L,dutyA为正时正转
pwm_t my2 = {PWM2, kPWM_Module_1, 16 * 1000, 0, 0, kPWM_HighTrue};  //R,dutyB为正时正转
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
    float AD_Sum_X, AD_Sum_Y, AD_Sum_E;
    float s_error_X, s_error_Y, s_error_E;
    float dir_angle = 0;

    if (State_Change_Flag != 0)		State_Change_Flag = 0;

    /*获取标准差*/
    //Get_Standard_Deviation();

    /*摄像头&AI模式*/
    if (data[data_identifier].mode == 0)
    {
    	Kp = 0.01*data[data_identifier].dirkp;
    	Kd = 0.01*data[data_identifier].dirkd;

    	/*AI+PID*/
//        //g_error = CAMERA_M - mid_line[data[data_identifier].forward_view];
//    	middleline_nncu = (g_AD_nncu_Output[2]/data[data_identifier].NNCU_NormalizeFactor)+20;
//    	if (middleline_nncu > 160)	middleline_nncu = 160;
//    	else if (middleline_nncu < 28) middleline_nncu = 28;
//    	//Middleline_Filter();
//    	g_error = CAMERA_M - middleline_nncu;
//        g_dir = 0.0001 * (data[data_identifier].dirkp * g_error + data[data_identifier].dirkd * (g_error - g_dir_error_1));
//        g_dir_error_1 = g_error;

    	/*Improved AI+PID*/
    	g_error = (float)(g_Servo_Devia);
    	g_error_motor = (float)(g_Motor_Devia);

    	/*获取PID*/
    	if (data[data_identifier].NNCU_NormalizeFactor == 0)
    	{
    		data[data_identifier].NNCU_NormalizeFactor = 10;
    	}
    	if (State_Change_Flag!=0)
    	{
    		g_dir = (float)(Kp)/1000*g_error;
    		g_dir_motor = (float)(Kp)/1000*g_error_motor;
    	}
    	else
    	{
    		//g_dir = (float)(g_AD_nncu_Output[2])/(100*data[data_identifier].NNCU_NormalizeFactor);
    		g_dir = ((float)(Kp)/1000*g_error+(float)(Kd)/1000*(g_error-g_dir_error_1));
    		g_dir_motor = ((float)(Kp)/1000*g_error_motor+(float)(Kd)/1000*(g_error_motor-g_dir_error_motor_1));
    	}
        g_dir_error_1 = g_error;
        g_dir_error_motor_1 = g_error_motor;

        /*舵机限幅保护*/
        Servo_Protect(&g_dir);
        Servo_Protect(&g_dir_motor);

        /*期望速度获取*/
        speed_expect_now = 0.1 * (float)data[data_identifier].speed;
		speed_expect = 0.1 * (float)data[data_identifier].speed;

        /*温柔变速*/
        static float speed_change_rate = 0;
        if(speed_change_flag==0)
        {
          if((speed_expect_now - speed_expect) > 1 ||  (speed_expect_now - speed_expect) < -1)
          {
            speed_change_flag = 1;
            speed_change_rate = 0;
          }
        }
        if(speed_change_flag==1)
        {
          speed_expect_now = speed_expect_now * (1-speed_change_rate) + speed_expect * speed_change_rate;
          speed_change_rate += 20*0.001/speed_change_time;
    //      if((speed_expect_now - speed_expect) < 1 &&  (speed_expect_now - speed_expect) > -1)
          if((speed_expect_now - speed_expect) < 1 &&  (speed_expect_now - speed_expect) > -1)
          {
            speed_change_flag = 0;
            speed_expect_now = speed_expect;
          }
        }

        /*差速期望速度获取*/
        float rate;
        float L_rate = (float)data[data_identifier].speedkl;
        float R_rate = (float)data[data_identifier].speedkr;

        if(g_dir_motor > 0)//向左转
        {
          //差速
            rate =1 + 0.5*tan(g_dir_motor*0.0122173)*((float)L_rate/100);
            s_speed_aim_left = (float)speed_expect_now/rate;
            s_speed_aim_right = speed_expect_now;
        }
        else//向右转
        {
        	g_dir_motor = -g_dir_motor;
        	//差速
        	rate =1 + 0.45*tan(g_dir_motor*0.0122173)*((float)R_rate/100);
        	s_speed_aim_right = (float)speed_expect_now/rate;
        	s_speed_aim_left = speed_expect_now;
        }

        /*舵机输出占空比*/
        PWM_AC_SetServoDuty((uint16_t)(100*DIR_M + g_dir));
        PWM_Dir_Motor = (uint16_t)(100*DIR_M + g_dir_motor);
    }

    /*电磁模式*/
    else if (data[data_identifier].mode == 1)
    {
    	int Weight_x = data[data_identifier].Weight_x;
    	int Weight_y = data[data_identifier].Weight_y;
    	int Weight_e = data[data_identifier].Weight_e;

        /*期望速度获取*/
        speed_expect_now = 0.1 * (float)data[data_identifier].speed;
		speed_expect = 0.1 * (float)data[data_identifier].speed;

    	/*防止分母为零*/
        if (EM_AD[0] + EM_AD[6] != 0)	{AD_Sum_X = EM_AD[0] + EM_AD[6];}
        else							{AD_Sum_X = 255*2;}
        if (EM_AD[1] + EM_AD[5] != 0)	{AD_Sum_Y = EM_AD[1] + EM_AD[5];}
        else							{AD_Sum_Y = 255*2;}
        if (EM_AD[2] + EM_AD[4] != 0)	{AD_Sum_E = EM_AD[2] + EM_AD[4];}
        else 							{AD_Sum_E = 255*2;}

    	/*十字识别*/
//        if (((EM_AD[1] >= 100 && EM_AD[5] >= 100) && abs(EM_AD[0] - EM_AD[6]) < 50) ||
//        	(EM_AD[1] >= 200 && EM_AD[5] >= 200))	//老判据
        if (Cross_flag == 0 && Round_flag == 0
        	&& (((EM_AD[1] > 80 && EM_AD[5] > 80) /*&& (EM_AD[2] > 2*EM_AD[4])*/ && EM_AD[3] > 80 && abs(EM_AD[0] - EM_AD[6]) < 100)  /*正入十字*/
        	|| ((EM_AD[1] > 120/*200*/ && EM_AD[5] > 120/*200*/) && EM_AD[0] + EM_AD[3] + EM_AD[6] > 320)))	/*斜入十字*/
        {
        	Cross_flag = 1;
        	State_Change_Flag = 1;
        }

        /*环岛识别*/
//        if (EM_AD[0] + EM_AD[6] >= 350)	//老判据
        if (Cross_flag == 0 && Round_flag == 0
        	&& (EM_AD[0] + EM_AD[3] + EM_AD[6] > 330/*300*/)
			&& EM_AD[3] > 130
			&& (EM_AD[1] < 30 || EM_AD[5] < 30)
			&& 2*EM_AD[3] < EM_AD[0] + EM_AD[6]
			&& EM_AD[1] + EM_AD[5] > 50)
        {
        	Round_flag = 1;
        	State_Change_Flag = 1;
        	speed_change_flag = 1;
        	if (EM_AD[1] > EM_AD[5]) Round_dir = 0;		//左环岛
        	else Round_dir = 1;		//右环岛
        }
        if (Round_flag == 1 && (EM_AD[3] > 190 || EM_AD[0] + EM_AD[3] + EM_AD[6] > 600/*580*/	/*正入环岛*/
        						|| EM_AD[7] + EM_AD[0] > 480 || EM_AD[8] + EM_AD[6] > 480 		/*斜入环岛*/))
        {
        	Round_flag = 2;
        	State_Change_Flag = 1;
        }

        /*偏差获取*/
        s_error_X = (float)(EM_AD[0] - EM_AD[6]) / AD_Sum_X;
        s_error_Y = (float)(EM_AD[1] - EM_AD[5]) / AD_Sum_Y;
        s_error_E = (float)(EM_AD[2] - EM_AD[4]) / AD_Sum_E;

        if (Round_flag != 0)
        {
        	//BEE_ON(&Bee_GPIO);

        	Kp = 0.01*data[data_identifier].dirkp_z;
        	Kd = 0.01*data[data_identifier].dirkd_z;

        	speed_expect_now = 0.1 * ((float)data[data_identifier].speed + (float)data[data_identifier].Round_Acc);

        	switch(Round_flag)
        	{
        	    case 1:
        	    	g_error = s_error_E;
        	    	break;
        	    case 2:
        	    	if (Round_dir == 0)
        	    	{
        	    		s_error_X = ((float)EM_AD[0] - (float)EM_AD[6]*(float)data[data_identifier].Round_TH/100) / ((float)EM_AD[0] + (float)EM_AD[6]*(float)data[data_identifier].Round_TH/100);
        	    		s_error_E = ((float)EM_AD[2] - (float)EM_AD[4]*(float)data[data_identifier].Round_TH/100) / ((float)EM_AD[2] + (float)EM_AD[4]*(float)data[data_identifier].Round_TH/100);
        	    		g_error = (s_error_X * Weight_x + s_error_E * Weight_e)/(Weight_x + Weight_e);
        	    	}
        	    	else
        	    	{
        	    		s_error_X = ((float)EM_AD[0]*(float)data[data_identifier].Round_TH/100 - (float)EM_AD[6]) / ((float)EM_AD[0]*(float)data[data_identifier].Round_TH/100 + (float)EM_AD[6]);
        	    		s_error_E = ((float)EM_AD[2]*(float)data[data_identifier].Round_TH/100 - (float)EM_AD[4]) / ((float)EM_AD[2]*(float)data[data_identifier].Round_TH/100 + (float)EM_AD[4]);
        	    		g_error = (s_error_X * Weight_x + s_error_E * Weight_e)/(Weight_x + Weight_e);
        	    	}
        	    	break;
        	    default:
					break;
        	}

        	g_error_motor = 0;

        	if (EM_AD[0] + EM_AD[3] + EM_AD[6] < 330/*300*/ /*&& (EM_AD[7] < 65 && EM_AD[8] < 65)*/)
        	{
        		Round_flag = 0;
        	}
        }
        else if (Cross_flag != 0)
        {
        	//BEE_ON(&Bee_GPIO);

          	speed_expect_now = 0.1 * ((float)data[data_identifier].speed + (float)data[data_identifier].Cross_Acc);

        	if (EM_AD[1] < 70 && EM_AD[5] < 70)
        	{
        		Cross_flag = 0;
        	}

        	Kp = 0.01*data[data_identifier].dirkp_s;
        	Kd = 0.01*data[data_identifier].dirkd_s;

        	Weight_x = data[data_identifier].Weight_x;
        	Weight_y = data[data_identifier].Weight_y;

        	//十字偏差获取有待商榷//
//        	g_error = (s_error_X * Weight_x + s_error_Y * Weight_y)/(Weight_x + Weight_y);
        	if(EM_AD[7]-EM_AD[8]>0)		g_error = 105 - EM_AD[3];
        	else						g_error = EM_AD[3] - 105;
        	g_error_motor = 0;
        }

        else
        {
        	//BEE_OFF(&Bee_GPIO);

        	Kp = 0.001*data[data_identifier].dirkp;
        	Kd = 0.001*data[data_identifier].dirkd;

        	Weight_x = data[data_identifier].Weight_x;
        	Weight_e = data[data_identifier].Weight_e;

//        	if (g_roll_sigma > 20 && g_roll_sigma < 40)
//        	{
//        		g_error = (s_error_X * Weight_x + s_error_E * Weight_e)/(Weight_x + Weight_e);
//        	}
//        	else
//        	{
//        		g_error = s_error_X;
//        	}

        	g_error = (s_error_X * Weight_x + s_error_E * Weight_e)/(Weight_x + Weight_e);

        	if (data[data_identifier].s_dir == 2)
        	{
            	if (g_error >= 0)					g_error = g_error*g_error;
            	else								g_error = -g_error*g_error;
        	}
        	else if (data[data_identifier].s_dir == 1)
        	{
        		/*Do Nothing*/
        	}

        	g_error_motor = s_error_E;
        }

        /*PID*/

    	if (State_Change_Flag!=0)
    	{
    		g_dir = (float)(Kp)*g_error;
    		g_dir_motor = (float)(Kp)*g_error_motor;
    	}
    	else
    	{
    		//g_dir = (float)(g_AD_nncu_Output[2])/(100*data[data_identifier].NNCU_NormalizeFactor);
    		g_dir = ((float)(Kp)*g_error+(float)(Kd)*(g_error-g_dir_error_1));
    		g_dir_motor = ((float)(Kp)*g_error_motor+(float)(Kd)*(g_error_motor-g_dir_error_motor_1));
    	}
        g_dir_error_1 = g_error;
        g_dir_error_motor_1 = g_error_motor;

        /*舵机限幅保护*/
        Servo_Protect(&g_dir);
        Servo_Protect(&g_dir_motor);

        /*温柔变速*/
        static float speed_change_rate = 0;
        if(speed_change_flag==0)
        {
          if((speed_expect_now - speed_expect) > 0.1 ||  (speed_expect_now - speed_expect) < -0.1)
          {
            speed_change_flag = 1;
            speed_change_rate = 0;
          }
        }
        if(speed_change_flag==1)
        {
          speed_expect_now = speed_expect_now * (1-speed_change_rate) + speed_expect * speed_change_rate;
          speed_change_rate += 20*0.001/speed_change_time;
    //      if((speed_expect_now - speed_expect) < 1 &&  (speed_expect_now - speed_expect) > -1)
          if((speed_expect_now - speed_expect) < 0.1 &&  (speed_expect_now - speed_expect) > -0.1)
          {
            speed_change_flag = 0;
            speed_expect_now = speed_expect;
          }
        }

        /*差速期望速度获取*/
        float rate;
        float L_rate = (float)data[data_identifier].speedkl;
        float R_rate = (float)data[data_identifier].speedkr;

        if(g_dir_motor > 0)//向左转
        {
          //差速
            rate =1 + 0.5*tan(g_dir_motor*0.0122173)*((float)L_rate);
            s_speed_aim_left = (float)speed_expect_now/rate;
            s_speed_aim_right = speed_expect_now;
        }
        else//向右转
        {
        	g_dir_motor = -g_dir_motor;
        	//差速
        	rate =1 + 0.45*tan(g_dir_motor*0.0122173)*((float)R_rate);
        	s_speed_aim_right = (float)speed_expect_now/rate;
        	s_speed_aim_left = speed_expect_now;
        }

        /*舵机输出占空比*/
        PWM_AC_SetServoDuty((uint16_t)(100*DIR_M + 100*g_dir));
        PWM_Dir_Motor = (uint16_t)(100*DIR_M + 100*g_dir_motor);

        //Send_Variable();

    }
}


void Speed_Control(void)
{
    //Running_Time();

    s_speed_left_now = (float)ENC_Positionget(ENC3) * FTM_CONSTANT;
    s_speed_right_now = -(float)ENC_Positionget(ENC2) * FTM_CONSTANT;

    /*老版本差速*/
//    s_speed_aim_left = s_speed_aim*(1-data[data_identifier].speedkl*Differencial);
//    s_speed_aim_right = s_speed_aim*(1+data[data_identifier].speedkr*Differencial);

    /*PID*/
    Motor_Kp_L = (float)data[data_identifier].speedkp_l;
    Motor_Kp_R = (float)data[data_identifier].speedkp_r;
    Motor_Ki_L = (float)data[data_identifier].speedki_l;
    Motor_Ki_R = (float)data[data_identifier].speedki_r;

    s_error_left = s_speed_aim_left - (float)s_speed_left_now;
    s_error_right = s_speed_aim_right - (float)s_speed_right_now;

    s_speed_left += ((float)Motor_Kp_L* 0.1 * (s_error_left - g_speed_error_left_1) + (float)Motor_Ki_L* 0.1 * s_error_left);
    s_speed_right += ((float)Motor_Kp_R* 0.1 * (s_error_right - g_speed_error_right_1) + (float)Motor_Ki_R* 0.1 * s_error_right);

    g_speed_error_left_1 = s_error_left;
    g_speed_error_right_1 = s_error_right;

    /*电机限幅*/
    Motor_Protect(&s_speed_left);
    Motor_Protect(&s_speed_right);

    Speed_Judge(s_speed_left, s_speed_right);

    ENC_Dateclear(ENC3);
    ENC_Dateclear(ENC2);
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
    static int motor_limit_flag = 1;
    static int stepcnt_motorlimit=0;

    if(motor_limit_flag==1) //刚起步的一段，电机限幅
    {
      if (*speed > MOTOR_START_LIMIT)       *speed = MOTOR_START_LIMIT;
      if (*speed < -MOTOR_START_LIMIT)       *speed = -MOTOR_START_LIMIT;
    }
    else//过了起步阶段，限幅解除
    {
      if (*speed > MAX_MOTOR_DUTY)         *speed = MAX_MOTOR_DUTY;
      if (*speed < -MAX_MOTOR_DUTY)		   *speed = -MAX_MOTOR_DUTY;
    }

    step_cnt(&stepcnt_motorlimit,motor_limit_flag);
    if(stepcnt_motorlimit > MOTOR_START_LIMIT_STEP && motor_limit_flag)    { motor_limit_flag = 0;}
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

void step_cnt(int *stepcnt,int stepcnt_flag)
{
  if(stepcnt_flag)
  {
    *stepcnt += s_speed_left_now;
    *stepcnt += s_speed_right_now;
  }
}
