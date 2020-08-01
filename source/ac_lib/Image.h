#ifndef _IMAGE_H
#define _IMAGE_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "cv.h"
//#include "highgui.h"
#include "ac_lib/AC_Menu.h"
#include <stdint.h>
#include "smartcar/sc_ac_delay.h"
#include "smartcar/sc_oled.h"
#include "smartcar/sc_camera_zzf.h"
#include "smartcar/sc_camera_common.h"
#include "ac_lib/Image.h"

/******************************************************************
*  Parameter declaration for IMG proc
******************************************************************/
#define MISS 255     // I don't know what this is...
#define CAMERA_H 120 //Picture Height
#define CAMERA_W 184 //Picture Width

#define STRATEGY_CURRENT_UPPER_LIMITS 1   //Current view for IMG proc
#define STRATEGY_CURRENT_LOWER_LIMITS 119 //Current view for IMG proc
#define STRATEGY_CURRENT_LEFT_LIMITS 0    //Current view for IMG proc
#define STRATEGY_CURRENT_RIGHT_LIMITS 187 //Current view for IMG proc
#define STRATEGY_CURRENT_MID 94           //Current view for IMG proc

#define STRATEGY_FUTURE_UPPER_LIMITS 0 //Future view for prediction
#define STRATEGY_FUTURE_LOWER_LIMITS 0 //Future view for prediction
#define STRATEGY_FUTURE_LEFT_LIMITS 0  //Future view for prediction
#define STRATEGY_FUTURE_RIGHT_LIMITS 0 //Future view for prediction

#define CAR_HEAD_FAR_LIMITS 83 //Car head limits, specially design for original img
#define CAR_HEAD_NEAR_LIMITS 120
#define CAR_HEAD_LEFT_LIMITS 40
#define CAR_HEAD_RIGHT_LIMITS 136
#define white_num_MAX 80 //每行最多允许白条数
/******************************************************************
* Preset colors
******************************************************************/
#define COLOR_BLACK 0
#define COLOR_WHITE 1
#define COLOR_BLUE 2
#define COLOR_GREEN 3
#define COLOR_RED 4
#define COLOR_GRAY 5
#define COLOR_PURPLE 6

///////////////////
typedef struct {
	uint8_t direction;
	uint8_t Lbxstart;
	uint8_t Rbxstart;
}circle_information;

typedef struct {
	uint8_t circle;
	uint8_t cross;
	uint8_t straight;
	uint8_t acceleration;
}MOOD;
#define gap 5
#define pi 3.14
#define cross_road_flag 0
#define circle_flag_stage_one 1
#define curve_flag 2
#define straight_way_flag 3
#define near_cross_road_flag 4
#define circle_flag_stage_two 5
#define circle_flag_stage_three 6
#define circle_flag_stage_four 7
#define circle_flag_stage_five 8
#define circle_flag_stage_six 9
#define circle_flag_stage_seven 10
#define circle_flag_stage_eight 11
#define circle_flag_stage_nine 12
#define FAR_LINE 1//图像处理上边界
#define NEAR_LINE 113//图像处理下边界
//////////////////////

//extern CvScalar col_white, col_black, col_blue, col_red, col_green, col_gray, col_purple;

//void IMAGE_Image2Array(IplImage *(*img), uint8_t array[CAMERA_H][CAMERA_W]);
//void IMAGE_Array2Image(uint8_t array[CAMERA_H][CAMERA_W], IplImage *(*img));
void IMAGE_Array2Image_OLED(uint8_t src[CAMERA_H][CAMERA_W], uint8_t dst[CAMERA_H][CAMERA_W]);
void IMAGE_COLOR_Gray2Binary(uint8_t src[CAMERA_H][CAMERA_W], uint8_t dst[CAMERA_H][CAMERA_W]);

void IMAGE_ADD_TopBoundary(uint8_t array[CAMERA_H][CAMERA_W]);

void IMAGE_RemoveCarHead(uint8_t array[CAMERA_H][CAMERA_W]);

void IMAGE_FIND_Mid(uint8_t array[CAMERA_H][CAMERA_W]);

void IMAGE_ROI_Cutting(uint8_t img[CAMERA_H][CAMERA_W], uint8_t roi_bg[CAMERA_H][CAMERA_W]);

/***internal use only***/
void col_init(void);

int IMAGE_Find_FatherNode(int a);
void IMAGE_SearchWhiteRange(uint8_t img[CAMERA_H][CAMERA_W]);
void IMAGE_FIND_AllConnect();
void IMAGE_FIND_ROAD();
uint8_t IMAGE_FIND_NextLineWhitePieces(uint8_t i_start, uint8_t j_start);
void IMAGE_FIND_Lines_LeftAndRight(uint8_t img[CAMERA_H][CAMERA_W]);
void IMAGE_FIND_Lines_Mid(void);
void IMAGE_FIND_CornerFirstPair(uint8_t img[CAMERA_H][CAMERA_W]);

void IMAGE_PROC_Main(uint8_t img[CAMERA_H][CAMERA_W]);

void IMAGE_COUNT_OverallWhite(uint8_t img[CAMERA_H][CAMERA_W]);

void IMAGE_FIX_MidLineCross(uint8_t img[CAMERA_H][CAMERA_W]);
void Stop_Car(void);
uint8_t otsuThreshold(uint8_t img[CAMERA_H][CAMERA_W]);
void Find_Zebra_Crossing(void);
void Image_Copy(img_t* src);
void Image_Main(img_t* src);

extern uint8_t mid_line[CAMERA_H];
extern int data_identifier;
extern Data_t data[10];
extern uint8_t threshold;
extern int Stop_Car_flag;
extern int Control_timenow;
extern int Flag_Find;

//////////////////
void element_init(void);//定义赛道元素
uint8_t define_my_way(uint8_t line);
uint8_t define_straight_line(uint8_t* line,uint8_t start_line,uint8_t end_line);//计算是否为直道
void find_x_right_left(void);//找左右赛道边
uint8_t define_road_type(void);
void THRE(uint8_t src[CAMERA_H][CAMERA_W], uint8_t dst[CAMERA_H][CAMERA_W]);//二值化
void map(void);//逆透视变换
extern uint8_t img_original[CAMERA_H][CAMERA_W];



bool tell_circle_stage_one(void);//环岛识别部分
bool tell_circle_stage_two(void);
bool tell_circle_stage_three(void);
bool tell_circle_stage_four(void);
bool tell_circle_stage_five(void);
bool tell_circle_stage_six(void);
bool tell_circle_stage_seven(void);
bool tell_circle_stage_eight(void);
bool tell_circle_stage_nine(void);


extern uint8_t circle_stage_one_flag;//赛道环岛部分标志
extern uint8_t circle_stage_two_flag;
extern uint8_t circle_stage_three_flag;
extern uint8_t circle_stage_four_flag;
extern uint8_t circle_stage_five_flag;
extern uint8_t circle_stage_six_flag;
extern uint8_t circle_stage_seven_flag;
extern uint8_t circle_stage_eight_flag;
extern uint8_t circle_stage_nine_flag;
extern bool if_stage_3;
extern circle_information circle_inf;
extern MOOD mood;
bool tell_cross(void);
bool tell_near_cross(void);
void roof(void);

#endif

// #ifndef _IMAGE_H
// #define _IMAGE_H
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdint.h>
// //#include "cv.h"
// //#include "highgui.h"
// #include <math.h>
// #include "image.h"

// #define MISS 255
// #define CAMERA_H  120                            //图片高度
// #define CAMERA_W  188                            //图片宽度
// #define FAR_LINE 1//图像处理上边界
// #define NEAR_LINE 113//图像处理下边界
// #define LEFT_SIDE 0//图像处理左边界
// #define RIGHT_SIDE 187//图像处理右边界
// #define MISS 255
// #define white_num_MAX 80//每行最多允许白条数

// /////////////////////////////
// #define black 0
// #define white 1
// #define blue  2
// #define green 3
// #define red   4
// #define gray  5
// #define purple 6
// ///////////////////////////

// extern uint8_t IMG[CAMERA_H][CAMERA_W];//二值化后图像数组
// extern uint8_t image_Buffer_0[CAMERA_H][CAMERA_W];
// extern uint8_t mid_line[CAMERA_H];

// void THRE();
// int find_f(int a);
// void search_white_range();
// void find_all_connect();
// void find_road();
// uint8_t find_continue(uint8_t i_start, uint8_t j_start);
// void ordinary_two_line(void);
// void image_main();
// void get_mid_line(void);
// #endif
