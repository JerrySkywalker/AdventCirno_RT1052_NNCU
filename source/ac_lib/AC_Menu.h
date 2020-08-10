/*** 
 * @Author: JerryW
 * @Date: 2020-06-19 14:53:01
 * @LastEditTime: 2020-06-21 10:55:52
 * @LastEditors: JerryW
 * @Description: 
 * @FilePath: /AdventCirno_RT1052/source/ac_lib/AC_Menu.h
 */
#ifndef _AC_MENU_H_
#define _AC_MENU_H_

#include "MIMXRT1052.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "smartcar/status.h"
#include "board.h"
#include "smartcar/sc_ac_delay.h"
#include "smartcar/sc_gpio.h"

/*Flash: Data Storage Start Address*/
#define FLASH_DATA_ADDR_START 3 * 1024 * 1024

#define DATA_IDENTIFIER_MIN 1
#define DATA_IDENTIFIER_MAX 9

/*TODO: DataSet definition here*/
typedef struct Data
{
    /**LED status*/
    int Led_CB_1;
    int Led_CB_2;
    int Led_CB_3;
    int Led_CB_4;

    /**Buzz status*/
    int Buzz;

    /**Car Speed*/
    int speed;

    /**Motor Differential*/
    int speedkp_l;
    int speedki_l;
    int speedkd_l;
    int speedkp_r;
    int speedki_r;
    int speedkd_r;
    int speedkl;        //左轮差速强度
    int speedkr;        //右轮差速强度

    /**Direction PID*/
    int dirkp;
    int dirki;
    int dirkd;
    int dirkp_z;
    int dirki_z;
    int dirkd_z;
    int dirkp_s;
    int dirki_s;
    int dirkd_s;
    /**Mode for Camera/EM Control*/
    int mode;

    int forward_view;
    int auto_threshold;
    int running_time;
    int Weight_x;
    int Weight_y;

    int yuzhi;
    int zhidao_yuzhi;
    int wandao_yuzhi;
    int shizi_yuzhi;
    int huandao_yuzhi;
    int jia_speed;
    int jian_speed;
    /**@brief NNCU NormalizeFactor
     *
     *  In the use of nncu selfinf code, we must use a normalize factor to justify the outcome. Usually the real outcome
     *  of the network is:
     *
     *      RealOutcome = NetworkOutcome/NormalizeFactor
     *
     *  To Easily use NNCU selfinf code ,this factor can be changed deliberately.
     * */
    int NNCU_NormalizeFactor;
} Data_t;

typedef enum
{
    CursorMove_UP,
    CursorMove_Down,
    CursorMove_Left,
    CursorMove_Right,
}CursorMove;

/**
 * @brief Defines the type of Menu node callback properties
 * @note    There are sereval different types:
 *
 *          MID:            MID node for Menu ,no callbacks
 *
 *          DATA_XXX:       DATA storaged in Menu, XXX is data type . Now only INT ins supported

 *          TASK            (FREERTOS)TASK run only once
 *
 *          SERVICE         (FREERTOS)TASK run in background
 *
 *
 * */
typedef enum
{
    MID,
    DATA_INT,
    TASK,
    SERVICE,
}MenuType;

typedef struct Menu
{
    uint8_t menuName[15];
    int size;
    MenuType type;
    int (*target_function)();

    struct Menu *father;
    struct Menu *firstson;
    struct Menu *lastson;
    struct Menu *pre;
    struct Menu *next;

} MenuNode_t;

/* Free RTOS Version MENU ENTRY*/

void AC_Menu(void *pvData);

/*
 * Menu API
 */

void Menu_BarShow(MenuNode_t *cursor_rendering);

void Menu_MenuNodeCreate(MenuNode_t **menu, char name[15], MenuType type, int (*target_function)(int (*action)(int *data,int modify)),MenuNode_t *father);
void Menu_DataEdit(MenuNode_t *targetMenu);

void Menu_MenuShow(MenuNode_t *menu, int page);
void Menu_CursorShow(int index);
void Menu_CursorMove(CursorMove move);
void Menu_CursorEnter(CursorMove move);

void Menu_TaskRun(MenuNode_t *targetMenu);
void Menu_ServiceRun(MenuNode_t *targetMenu);

/*TODO: Action List*/

int Show_int(int *data,int temp);
int Modify_int(int *data,int modify);

/*TODO: Callbacks for data modification*/

int Led_CB_1(int (*action)(int *data,int modify));
int Led_CB_2(int (*action)(int *data,int modify));
int Led_CB_3(int (*action)(int *data,int modify));
int Led_CB_4(int (*action)(int *data,int modify));

int Set_Buzz(int (*action)(int *data,int modify));
int Set_SpeedKp_L(int (*action)(int *data,int modify));
int Set_SpeedKi_L(int (*action)(int *data,int modify));
int Set_SpeedKd_L(int (*action)(int *data,int modify));
int Set_SpeedKp_R(int (*action)(int *data,int modify));
int Set_SpeedKi_R(int (*action)(int *data,int modify));
int Set_SpeedKd_R(int (*action)(int *data,int modify));
int Set_SpeedKL(int (*action)(int *data,int modify));
int Set_SpeedKR(int (*action)(int *data,int modify));
int Set_Speed(int (*action)(int *data,int modify));
int Set_Class(int (*action)(int *data,int modify));
int Set_DirKp(int (*action)(int *data,int modify));
int Set_DirKi(int (*action)(int *data,int modify));
int Set_DirKd(int (*action)(int *data,int modify));
int Set_DirKp_z(int (*action)(int *data,int modify));
int Set_DirKi_z(int (*action)(int *data,int modify));
int Set_DirKd_z(int (*action)(int *data,int modify));
int Set_DirKp_s(int (*action)(int *data,int modify));
int Set_DirKi_s(int (*action)(int *data,int modify));
int Set_DirKd_s(int (*action)(int *data,int modify));
int Set_Mode(int (*action)(int *data,int modify));
int Set_ForwardView(int (*action)(int *data,int modify));
int Set_AutoThreshold(int (*action)(int *data,int modify));
int Set_RunningTime(int (*action)(int *data,int modify));
int Set_Weight_x(int (*action)(int *data,int modify));
int Set_Weight_y(int (*action)(int *data,int modify));
int Set_jia_speed(int (*action)(int *data,int modify));
int Set_jian_speed(int (*action)(int *data,int modify));
int Set_yuzhi(int (*action)(int *data,int modify));
int Set_zhidao_yuzhi(int (*action)(int *data,int modify));
int Set_wandao_yuzhi(int (*action)(int *data,int modify));
int Set_shizi_yuzhi(int (*action)(int *data,int modify));
int Set_huandao_yuzhi(int (*action)(int *data,int modify));

int Set_NNCU_NormalizeFactor(int (*action)(int *data,int modify));

/**TODO: Task Declaration*/
int Task_SD_SaveMenu(int (*action)(int *data,int modify));
int Task_SD_LoadMenu(int (*action)(int *data,int modify));

/**TODO: Service Declaration*/
int Service_SD_SyncMenu(int (*action)(int *data,int modify));


#endif
