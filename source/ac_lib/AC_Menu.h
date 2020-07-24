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
    int Led_CB_1;
    int Led_CB_2;
    int Led_CB_3;
    int Led_CB_4;

    //int Preset_Class;
    int Buzz;

    int speed;
    int speedkp_l;
    int speedki_l;
    int speedkd_l;
    int speedkp_r;
    int speedki_r;
    int speedkd_r;
    int speedkl;        //左轮差速强度
    int speedkr;        //右轮差速强度

    int dirkp;
    int dirki;
    int dirkd;
    int mode;
    int forward_view;
    int auto_threshold;
    int running_time;
    int Weight_x;
    int Weight_y;
} Data_t;

typedef enum
{
    CursorMove_UP,
    CursorMove_Down,
    CursorMove_Left,
    CursorMove_Right,
}CursorMove;

typedef enum
{
    MID,
    FUNC,
    GO,
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
int Set_Mode(int (*action)(int *data,int modify));
int Set_ForwardView(int (*action)(int *data,int modify));
int Set_AutoThreshold(int (*action)(int *data,int modify));
int Set_RunningTime(int (*action)(int *data,int modify));
int Set_Weight_x(int (*action)(int *data,int modify));
int Set_Weight_y(int (*action)(int *data,int modify));

#endif
