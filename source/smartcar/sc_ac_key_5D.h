/*** 
 * @Author: JerryW
 * @Date: 2020-06-18 15:39:59
 * @LastEditTime: 2020-06-19 17:32:31
 * @LastEditors: JerryW
 * @Description: 
 * @FilePath: \IMRT10XX\source\smartcar\sc_ac_key_5D.h
 */
#ifndef _SC_AC_KEY_5D_H_
#define _SC_AC_KEY_5D_H_


#include "sc_gpio.h"

/*5D Keypad for RT1052_ControlBoard 2nd Edition*/
typedef enum
{
    KEY_ENTER,//0 	中心键
    KEY_UP,   //1	上键

    KEY_DOWN, //2 	下键
    KEY_LEFT, //3	左键

    KEY_RIGHT,//4	右键

    KEY_MAX,  //5   初始化用
}KEY_e;

/*Key Status for RT1052_ControlBoard 2nd Edition*/
typedef enum
{
    KEY_P_DOWN,
    KEY_P_UP,
}KEY_STATUS;

void KEY_5D_Init();
KEY_STATUS Key_Check(KEY_e key);



#endif  //SC_KEY_5D_H_
