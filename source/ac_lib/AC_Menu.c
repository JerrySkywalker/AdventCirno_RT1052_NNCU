/*
 * @Author: JerryW
 * @Date: 2020-06-19 14:52:43
 * @LastEditTime: 2020-06-23 00:37:59
 * @LastEditors: JerryW
 * @Description:
 * @FilePath: /AdventCirno_RT1052/source/ac_lib/AC_Menu.c
 */
#include "AC_Menu.h"
#include "smartcar/sc_ac_key_5D.h"
#include "smartcar/sc_flash.h"
#include "smartcar/sc_oled.h"

TaskHandle_t AC_Menu_task_handle;

/*
 * CONST definition for OLED display
 * */

/* some location mark related to DATA rendering on 128x64 OLED*/
const int PRINT_START_DATA = 90, PRINT_START_MULTIPLICATOR = 84, PRINT_LENGTH_DATA = 5;

/* some location mark related to CURSOR rendering on 128x64 OLED*/
const int CURSOR_SUP = 1, CURSOR_INF = 7;

/* TODO: ADD your declearation of menu pointer */

/*Menu Root: The main root of the menu tree*/
MenuNode_t *root;

MenuNode_t *Menu_Led_CoreBoard, *Menu_Speed, *Menu_Direction, *Menu_SetMode;

MenuNode_t *Menu_CB_LED1, *Menu_CB_LED2, *Menu_CB_LED3, *Menu_CB_LED4;
MenuNode_t *Menu_SetClass, *Menu_SetBuzz, *Menu_SetSpeed, *Menu_SetBoardLed;
MenuNode_t *Menu_SetSpeedKp_L, *Menu_SetSpeedKi_L, *Menu_SetSpeedKd_L, *Menu_SetSpeedKp_R, *Menu_SetSpeedKi_R, *Menu_SetSpeedKd_R, *Menu_SetSpeedKL, *Menu_SetSpeedKR;
MenuNode_t *Menu_SetDirKp, *Menu_SetDirKi, *Menu_SetDirKd;
MenuNode_t *Menu_SetForwardView;
MenuNode_t *Menu_SetAutoThreshold;
MenuNode_t *Menu_SetRunningTime;
MenuNode_t *Menu_SetSetupTime;
MenuNode_t *Menu_Weight_x;
MenuNode_t *Menu_Weight_y;

/**NNCU Data*/
MenuNode_t *Menu_NNCU, *Menu_NNCU_NormalizeFactor;

/**
 * @brief OLED display variables
 */

/*A tracker for the rendering menu*/
MenuNode_t *cursor_rendering;

/*Cursor location variable*/
int cursor_index = 1, cursor_page = 1;

/*
 * DATA related var
 */

/*Menu data buffer. Read from flash when booted, always update*/
volatile Data_t data[10];
/*A marker for dataset identification.Use with data[]*/
volatile int data_identifier = 1;

/*A multiplicator for data modification */
int multiplicator = 1;

/*A operator marker for data modification, 1 is plus ,-1 is minus*/
int pm = 1;


/**
 * @brief Render your menu bar
 * @param cursor_rendering the current rendering menu root
 * @example Menu_BarShow(cursor_rendering)
 * @since      v1.0
 */
void Menu_BarShow(MenuNode_t *cursor_rendering) {
    OLED_P6x8Str(0, 0, cursor_rendering->menuName);
    OLED_Print_Num(60, 0, cursor_page);
    OLED_P6x8Str(66, 0, "/");
    OLED_Print_Num(72, 0, (cursor_rendering->size) / CURSOR_INF + 1);
}


/**
 * @brief Create Menu Node ,and fill in data.....IN ONE LINE,ANYWHERE IN THE CODE!
 * @param menu the address of menu pointer
 * @param name the name of menu ,will be shown on the oled
 * @param type the type of menu node, MID is the jumping node, FUNC is the function node
 * @param target_function the target function for function node ,NULL for MID
 * @param father the father of this node
 * @example Menu_MenuNodeCreate(&Menu_Led_CoreBoard, "Led_Board", MID, NULL, root);
 * @since v1.0
 */
void Menu_MenuNodeCreate(MenuNode_t *(*menu),
                         char name[15],
                         MenuType type,
                         int (*target_function)(int (*action)(int *data,
                                                              int modify)),
                         MenuNode_t *father) {
    //(*menu) = (MenuNode_t *)malloc(sizeof(MenuNode_t));
    (*menu) = (MenuNode_t *) pvPortMalloc(sizeof(MenuNode_t));

    memcpy((*menu)->menuName, name, sizeof((*menu)->menuName));

    (*menu)->type = type;
    (*menu)->target_function = target_function;
    (*menu)->size = 0;

    (*menu)->father = father;
    (*menu)->firstson = NULL;
    (*menu)->lastson = NULL;
    (*menu)->pre = NULL;
    (*menu)->next = NULL;

    if (father->firstson == NULL) {
        father->firstson = (*menu);
        father->lastson = (*menu);
    } else {
        (*menu)->pre = father->lastson;
        (father->lastson)->next = (*menu);
        father->lastson = (*menu);
    }
    father->size++;
}

/**
 * @brief Show Menu Automatically
 * @param cursor_rendering the current rendering root
 * @param page the current rendering page
 * @example	Menu_MenuShow(root,1);
 * @since v1.0
*/
void Menu_MenuShow(MenuNode_t *menu, int page) {
    cursor_rendering = menu;

    Menu_BarShow(cursor_rendering);

    int temp_showIndex = cursor_rendering->size;
    int temp_pagelimit = (cursor_rendering->size) / CURSOR_INF + 1;
    MenuNode_t *rendering = cursor_rendering->firstson;

    int i;
    if (page != 1) {
        for (i = 1; i <= 7 * (page - 1); i++) {
            rendering = rendering->next;
            temp_showIndex--;
        }
    }

    if (temp_showIndex > CURSOR_INF) {
        temp_showIndex = CURSOR_INF;
    }

    int range = temp_showIndex + 1;
    while (temp_showIndex > 0) {
        OLED_P6x8Str(12, range - temp_showIndex, rendering->menuName);
        if (rendering->type == FUNC) {
            Str_Clr(PRINT_START_DATA, range - temp_showIndex,
                    PRINT_LENGTH_DATA);
            OLED_Print_Num(PRINT_START_DATA, range - temp_showIndex,
                           rendering->target_function(Show_int));
        }
        rendering = rendering->next;
        temp_showIndex--;
    }
    while (range < 7) {
        range++;
        Str_Clr(0, range, 22);
    }
}

/**
 *  @brief      Show Cursor on the left side of the screen
 *  @param      index    the current Y of cursor (on the screen)
 *  @Sample 	Menu_CursorShow(cursor_index)
 *  @since      v1.0
 */
void Menu_CursorShow(int index) {
    OLED_P6x8Str(0, index, "*");
}

/**
 *  @brief     Cursor : React to Move Action (& rendering a new page if needed)
 *  @param      move     your action, 1 means 'go down the menu list', -1 'means go up
 *  @example 	Menu_CursorMove(CursorMove_Down);
 *  @since      v1.0
*/
void Menu_CursorMove(CursorMove move) {
    int pagelimit = (cursor_rendering->size) / 7 + 1;

    Str_Clr(0, cursor_index, 2);

    if (move == CursorMove_Down) {
        if (cursor_index < CURSOR_INF &&
            ((cursor_index + 7 * (cursor_page - 1)) < cursor_rendering->size)) {
            cursor_index++;
        } else if (cursor_page <= pagelimit && cursor_index == 7) {
            cursor_index = CURSOR_SUP;
            cursor_page++;
            OLED_Fill(0);
            Menu_MenuShow(cursor_rendering, cursor_page);
        }
    } else {
        if (cursor_index > CURSOR_SUP) {
            cursor_index--;
        } else if (cursor_page > 1) {
            cursor_index = CURSOR_INF;
            cursor_page--;
            OLED_Fill(0);
            Menu_MenuShow(cursor_rendering, cursor_page);
        }
    }

    Menu_CursorShow(cursor_index);
}

/**
 *  @brief      Cursor : React to Enter Action
 *  @param      move     your action,1 means 'in(or edit)', -1 means 'out'
 *  @example	Menu_CursorEnter(CursorMove_Right);
 *  @since      v1.0
*/
void Menu_CursorEnter(CursorMove move) {
    MenuNode_t *targetMenu;
    int targetDistance = cursor_index;
    // targetMenu = cursor_pointing;

    targetMenu = cursor_rendering->firstson;
    targetMenu->father = cursor_rendering;

    int i;
    for (i = 1; i <= 7 * (cursor_page - 1); i++) {
        targetMenu = targetMenu->next;
    }
    for (i = 1; i < cursor_index; i++) {
        targetMenu = targetMenu->next;
    }

    if (move == CursorMove_Right && targetMenu->type == FUNC) {
        Menu_DataEdit(targetMenu);
    } else if (move == CursorMove_Right && targetMenu->type == MID) {
        cursor_rendering = targetMenu->firstson;
        OLED_Fill(0);
        cursor_page = 1;
        cursor_index = 1;
        Menu_MenuShow(targetMenu, 1);
    } else if (move == CursorMove_Left && (targetMenu->father) != NULL &&
               (targetMenu->father) != root) {
        cursor_rendering = cursor_rendering->father;
        OLED_Fill(0);
        cursor_page = 1;
        cursor_index = 1;
        Menu_MenuShow(cursor_rendering, 1);
    }
}

/**
 *  @brief      Data edit
 *  @param      targetMenu   the node you want to edit.
 *  @example	Menu_DataEdit(targetMenu);
 *  @Tips       you can change the multiplicator's range in this function
 *  @since      v1.0
 */
void Menu_DataEdit(MenuNode_t *targetMenu) {
    multiplicator = 1;
    Str_Clr(PRINT_START_MULTIPLICATOR, 0, PRINT_LENGTH_DATA + 1);
    OLED_P6x8Str(PRINT_START_MULTIPLICATOR, 0, "X");

    OLED_Print_Num(PRINT_START_DATA, 0, multiplicator);

    while (Key_Check(KEY_ENTER) != KEY_P_DOWN) {
        vTaskDelay(10);
        if (Key_Check(KEY_ENTER) != KEY_P_DOWN) {
            if (KEY_P_DOWN == Key_Check(KEY_DOWN)) {
                Str_Clr(PRINT_START_DATA, cursor_index, 6);
                pm = -1;
                OLED_Print_Num(PRINT_START_DATA, cursor_index,
                               targetMenu->target_function(Modify_int));
            }
            if (KEY_P_DOWN == Key_Check(KEY_UP)) {
                Str_Clr(PRINT_START_DATA, cursor_index, 6);
                pm = 1;
                int ans = targetMenu->target_function(Modify_int);
                OLED_Print_Num(PRINT_START_DATA, cursor_index, ans);
            }
            if (KEY_P_DOWN == Key_Check(KEY_RIGHT)) {
                multiplicator *= 10;
                if (multiplicator > 10000) {
                    multiplicator = 10000;
                }
                Str_Clr(PRINT_START_DATA, 0, PRINT_LENGTH_DATA + 1);
                OLED_Print_Num(PRINT_START_DATA, 0, multiplicator);
            }
            if (KEY_P_DOWN == Key_Check(KEY_LEFT)) {
                multiplicator /= 10;
                if (multiplicator < 1) {
                    multiplicator = 1;
                }
                Str_Clr(PRINT_START_DATA, 0, PRINT_LENGTH_DATA + 1);
                OLED_Print_Num(PRINT_START_DATA, 0, multiplicator);
            }
        }
    }
    /*K66 Version*/
    //Error = ram_to_flash((2 * sizeof(int) + data_identifier * sizeof(Data_t)),
    //                     &data[data_identifier], sizeof(Data_t));

    Str_Clr(PRINT_START_MULTIPLICATOR, 0, PRINT_LENGTH_DATA + 3);

    Menu_MenuShow(cursor_rendering, cursor_page);
}

/**
 * @brief Menu initialization sequence. Menu can be managed in the order bellow
 * @since v2.0
 * @note: To create a new menu node ,you only need to do 3 steps
 *
 *          1.  Declare a menu pointer
 *
 *          2. Create menu node and fill in data here(in one line,yeh!!!)
 *
 *          3. If it is a FUNC, DIY your func in the end of this code
 *
 * @tips: you don't need to care about the order of init lines,only make sure
 *          the menu of same level is under you wish
 * */
void Menu_Init() {

    /* TODO: init your menu here! */

    Menu_MenuNodeCreate(&Menu_SetClass, "SetClass", FUNC, Set_Class, root);
    Menu_MenuNodeCreate(&Menu_SetMode, "SetMode", FUNC, Set_Mode, root);

    /**@brief Settings for board LED only*/
//    Menu_MenuNodeCreate(&Menu_Led_CoreBoard, "Led_Board", MID, NULL, root);
//    {
//        Menu_MenuNodeCreate(&Menu_CB_LED1, "SetLed1", FUNC, Led_CB_1, Menu_Led_CoreBoard);
//        Menu_MenuNodeCreate(&Menu_CB_LED2, "SetLed2", FUNC, Led_CB_2, Menu_Led_CoreBoard);
//        Menu_MenuNodeCreate(&Menu_CB_LED3, "SetLed3", FUNC, Led_CB_3, Menu_Led_CoreBoard);
//        Menu_MenuNodeCreate(&Menu_CB_LED4, "SetLed4", FUNC, Led_CB_4, Menu_Led_CoreBoard);
//    }

    /**@brief Settings for Differential */
    Menu_MenuNodeCreate(&Menu_Speed, "Speed", MID, NULL, root);
    {
        Menu_MenuNodeCreate(&Menu_SetSpeedKp_L, "SetSpeedKp_L", FUNC, Set_SpeedKp_L, Menu_Speed);
        Menu_MenuNodeCreate(&Menu_SetSpeedKi_L, "SetSpeedKi_L", FUNC, Set_SpeedKi_L, Menu_Speed);
        Menu_MenuNodeCreate(&Menu_SetSpeedKd_L, "SetSpeedKd_L", FUNC, Set_SpeedKd_L, Menu_Speed);
        Menu_MenuNodeCreate(&Menu_SetSpeedKp_R, "SetSpeedKp_R", FUNC, Set_SpeedKp_R, Menu_Speed);
        Menu_MenuNodeCreate(&Menu_SetSpeedKi_R, "SetSpeedKi_R", FUNC, Set_SpeedKi_R, Menu_Speed);
        Menu_MenuNodeCreate(&Menu_SetSpeedKd_R, "SetSpeedKd_R", FUNC, Set_SpeedKd_R, Menu_Speed);
        Menu_MenuNodeCreate(&Menu_SetSpeedKL, "SetSpeedKL", FUNC, Set_SpeedKL, Menu_Speed);
        Menu_MenuNodeCreate(&Menu_SetSpeedKR, "SetSpeedKR", FUNC, Set_SpeedKR, Menu_Speed);
    }

    /**@brief Settings for Direction PID */
    Menu_MenuNodeCreate(&Menu_Direction, "Direction", MID, NULL, root);
    {
        Menu_MenuNodeCreate(&Menu_SetDirKp, "SetDirKp", FUNC, Set_DirKp, Menu_Direction);
        Menu_MenuNodeCreate(&Menu_SetDirKi, "SetDirKi", FUNC, Set_DirKi, Menu_Direction);
        Menu_MenuNodeCreate(&Menu_SetDirKd, "SetDirKd", FUNC, Set_DirKd, Menu_Direction);
    }

    /**@brief Settings for NNCU */
    Menu_MenuNodeCreate(&Menu_NNCU,"NNCU",MID, NULL, root);
    {
        Menu_MenuNodeCreate(&Menu_NNCU_NormalizeFactor, "NormFactor", FUNC, Set_NNCU_NormalizeFactor, Menu_NNCU);
    }

    /** Level 1:Quick Set*/
    Menu_MenuNodeCreate(&Menu_SetBuzz, "SetBuzz", FUNC, Set_Buzz, root);
    Menu_MenuNodeCreate(&Menu_SetSpeed, "SetSpeed", FUNC, Set_Speed, root);

    /** Level 1: else*/
    Menu_MenuNodeCreate(&Menu_SetForwardView, "SetFW", FUNC, Set_ForwardView, root);
    Menu_MenuNodeCreate(&Menu_SetAutoThreshold, "SetAutoTH", FUNC, Set_AutoThreshold, root);
    Menu_MenuNodeCreate(&Menu_SetRunningTime, "SetRunTime", FUNC, Set_RunningTime, root);
    // Menu_MenuNodeCreate(&Menu_SetSetupTime, "SetSetupTime", FUNC, Set_SetupTime, root);
    Menu_MenuNodeCreate(&Menu_Weight_x, "Weight_x", FUNC, Set_Weight_x, root);
    Menu_MenuNodeCreate(&Menu_Weight_y, "Weight_y", FUNC, Set_Weight_y, root);
}

/**
 * @description: AC_Menu Main Entry.
 * @param   void
 * @return  NULL
 */
void AC_Menu(void *pvData) {
    PRINTF("[O K] AC: Menu Created\r\n");

    OLED_Fill(0x00);

    OLED_Logo();
    vTaskDelay(500);
    OLED_Fill(0x00);

    /*Init Menu root*/
    //root = (MenuNode_t *)malloc(sizeof(MenuNode_t));
    root = (MenuNode_t *) pvPortMalloc(sizeof(MenuNode_t));

    root->size = 0;
    strcpy(root->menuName, "root");
    root->father = NULL;
    root->firstson = NULL;
    root->lastson = NULL;
    root->pre = NULL;
    root->next = NULL;

    //cursor_rendering = (MenuNode_t *)malloc(sizeof(MenuNode_t));
    cursor_rendering = (MenuNode_t *) pvPortMalloc(sizeof(MenuNode_t));

    Menu_Init();

    Menu_MenuShow(root, 1);
    Menu_CursorShow(cursor_index);


    while (1) {
        if ((KEY_P_DOWN == Key_Check(KEY_DOWN)) || (KEY_P_DOWN == Key_Check(KEY_UP)) ||
            (KEY_P_DOWN == Key_Check(KEY_LEFT)) || (KEY_P_DOWN == Key_Check(KEY_RIGHT)) ||
            (KEY_P_DOWN == Key_Check(KEY_ENTER))) {
            if (KEY_P_DOWN == Key_Check(KEY_DOWN)) {
                Menu_CursorMove(CursorMove_Down);
            }
            if (KEY_P_DOWN == Key_Check(KEY_UP)) {
                Menu_CursorMove(CursorMove_UP);
            }
            if (KEY_P_DOWN == Key_Check(KEY_LEFT)) {
                Menu_CursorEnter(CursorMove_Left);
            }
            if ((KEY_P_DOWN == Key_Check(KEY_RIGHT)) || (KEY_P_DOWN == Key_Check(KEY_ENTER))) {
                Menu_CursorEnter(CursorMove_Right);
            }
        }
    }

}

/**
*  @brief      simply return int var
*  @param      data     the adreess of data
*  @param      temp     here we don't use it
*  @since      v1.0
*/
int Show_int(int *data, int temp) {
    return *data;
}

/**
*  @brief      return int var with modify
*  @param      data     the adreess of data
*  @param      modify   the increase value for each action 
*  @since      v1.0
*/
int Modify_int(int *data, int modify) {
    *data += modify * pm;
    return *data;
}

/*
* Tips: To create a FUNC ,you only need to do 4 steps
* 1. declear your FUNC
* 2. do the range check(which is VERY SERIOUS),and save value
* 3. if this node manage an I/O, apply the I/O change (Led,Buzz,etc)
* 4. return you ans,so the menu can show the change
*/

/*TODO:Your menu::function here!*/

/**
*  @brief      Led_CB_1 Callback
*  @since      v1.0
*/
int Led_CB_1(int (*action)(int *data, int modify)) {
    int ans = action(&data[data_identifier].Led_CB_1, multiplicator) % 2;
    ans < 0 ? 1 : ans;
    data[data_identifier].Led_CB_1 = ans;
    if (ans == 1) {
        //LED_On(GPIOA, 17);
    } else {
        //LED_Off(GPIOA, 17);
    }

    return ans;
}

int Led_CB_2(int (*action)(int *data, int modify)) {
    int ans = action(&data[data_identifier].Led_CB_2, multiplicator) % 2;

    ans < 0 ? 1 : ans;
    data[data_identifier].Led_CB_2 = ans;

    if (ans == 1) {
        //LED_On(GPIOC, 0);
    } else {
        //LED_Off(GPIOC, 0);
    }

    return ans;
}

int Led_CB_3(int (*action)(int *data, int modify)) {
    int ans = action(&data[data_identifier].Led_CB_3, multiplicator) % 2;

    ans < 0 ? 1 : ans;
    data[data_identifier].Led_CB_3 = ans;
    if (ans == 1) {
        //LED_On(GPIOD, 15);
    } else {
        //LED_Off(GPIOD, 15);
    }

    return ans;
}

int Led_CB_4(int (*action)(int *data, int modify)) {
    int ans = action(&data[data_identifier].Led_CB_4, multiplicator) % 2;
    ans < 0 ? 1 : ans;
    data[data_identifier].Led_CB_4 = ans;
    if (ans == 1) {
        //LED_On(GPIOE, 25);
    } else {
        //LED_Off(GPIOE, 25);
    }

    return ans;
}

int Set_Buzz(int (*action)(int *data, int modify)) {
    int ans = action(&data[data_identifier].Buzz, multiplicator) % 2;
    ans < 0 ? 1 : ans;
    data[data_identifier].Buzz = ans;
    if (ans == 1) {
        //Beep_On(GPIOA, 11);
    } else {
        //Beep_Off(GPIOA, 11);
    }

    return ans;
}

int Set_Speed(int (*action)(int *data, int modify)) {
    int ans = action(&data[data_identifier].speed, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 100 ? ans : 100;
    data[data_identifier].speed = ans;
    return ans;
}

int Set_Class(int (*action)(int *data, int modify)) {
    int *p = &data_identifier;
    int ans = action(p, multiplicator);
    ans = ans >= 1 ? ans : 1;
    ans = ans <= 9 ? ans : 9;
    data_identifier = ans;

    /*K66 Version*/
    //ram_to_flash(0,&data_identifier, sizeof(int));
    //flash_to_ram(&data[data_identifier], (2*sizeof(int) + data_identifier * sizeof(Data_t)), sizeof(Data_t));

    /*RT1052 Version*/
    /*Reload Buffer*/
//    FLASH_Read(FLASH_DATA_ADDR_START, buff_page_r, sizeof(FLASH_PAGE_SIZE));
//    memcpy(buff_page_w,buff_page_r,sizeof(FLASH_PAGE_SIZE));
//    memcpy(buff_page_w,&data_identifier,sizeof(data_identifier));
//    FLASH_Prog(FLASH_DATA_ADDR_START, buff_page_w);
//    FLASH_Read(FLASH_DATA_ADDR_START, &data_identifier, sizeof(data_identifier));

    return ans;
}
int Set_SpeedKp_L(int (*action)(int *data, int modify))
{
    int ans = action(&data[data_identifier].speedkp_l, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 30000 ? ans : 30000;
    data[data_identifier].speedkp_l = ans;
    return ans;
}
int Set_SpeedKi_L(int (*action)(int *data, int modify))
{
    int ans = action(&data[data_identifier].speedki_l, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 30000 ? ans : 30000;
    data[data_identifier].speedki_l = ans;
    return ans;
}
int Set_SpeedKd_L(int (*action)(int *data, int modify))
{
    int ans = action(&data[data_identifier].speedkd_l, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 30000 ? ans : 30000;
    data[data_identifier].speedkd_l = ans;
    return ans;
}
int Set_SpeedKp_R(int (*action)(int *data, int modify))
{
    int ans = action(&data[data_identifier].speedkp_r, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 30000 ? ans : 30000;
    data[data_identifier].speedkp_r = ans;
    return ans;
}
int Set_SpeedKi_R(int (*action)(int *data, int modify))
{
    int ans = action(&data[data_identifier].speedki_r, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 30000 ? ans : 30000;
    data[data_identifier].speedki_r = ans;
    return ans;
}
int Set_SpeedKd_R(int (*action)(int *data, int modify))
{
    int ans = action(&data[data_identifier].speedkd_r, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 30000 ? ans : 30000;
    data[data_identifier].speedkd_r = ans;
    return ans;
}
int Set_SpeedKL(int (*action)(int *data, int modify))
{
    int ans = action(&data[data_identifier].speedkl, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 30000 ? ans : 30000;
    data[data_identifier].speedkl = ans;
    return ans;
}
int Set_SpeedKR(int (*action)(int *data, int modify))
{
    int ans = action(&data[data_identifier].speedkr, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 30000 ? ans : 30000;
    data[data_identifier].speedkr = ans;
    return ans;
}

int Set_DirKp(int (*action)(int *data, int modify))
{
    int ans = action(&data[data_identifier].dirkp, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 30000 ? ans : 30000;
    data[data_identifier].dirkp = ans;
    return ans;
}
int Set_DirKi(int (*action)(int *data, int modify))
{
    int ans = action(&data[data_identifier].dirki, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 30000 ? ans : 30000;
    data[data_identifier].dirki = ans;
    return ans;
}
int Set_DirKd(int (*action)(int *data, int modify))
{
    int ans = action(&data[data_identifier].dirkd, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 30000 ? ans : 30000;
    data[data_identifier].dirkd = ans;
    return ans;
}
int Set_Mode(int (*action)(int *data, int modify))
{
    int ans = action(&data[data_identifier].mode, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 2 ? ans : 2;
    data[data_identifier].mode = ans;
    return ans;
}
int Set_ForwardView(int (*action)(int *data,int modify))
{
    int ans = action(&data[data_identifier].forward_view, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 119 ? ans : 119;
    data[data_identifier].forward_view = ans;
    return ans;
}
int Set_AutoThreshold(int (*action)(int *data,int modify))
{
    int ans = action(&data[data_identifier].auto_threshold, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 1 ? ans : 1;
    data[data_identifier].auto_threshold = ans;
    return ans;
}
int Set_RunningTime(int (*action)(int *data,int modify))
{
    int ans = action(&data[data_identifier].running_time, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 60000 ? ans : 60000;
    data[data_identifier].running_time = ans;
    return ans;
}
int Set_Weight_x(int (*action)(int *data,int modify))
{
    int ans = action(&data[data_identifier].Weight_x, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 60000 ? ans : 60000;
    data[data_identifier].Weight_x = ans;
    return ans;
}
int Set_Weight_y(int (*action)(int *data,int modify))
{
    int ans = action(&data[data_identifier].Weight_y, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 60000 ? ans : 60000;
    data[data_identifier].Weight_y = ans;
    return ans;
}


/**
 * @brief NNCU_NormalizeFactor Callback
 * @since v1.0
 * */
int Set_NNCU_NormalizeFactor(int (*action)(int *data,int modify))
{
    int ans = action(&data[data_identifier].NNCU_NormalizeFactor, multiplicator);
    ans = ans >= 0 ? ans : 0;
    ans = ans <= 60000 ? ans : 60000;
    data[data_identifier].NNCU_NormalizeFactor = ans;
    return ans;
}
