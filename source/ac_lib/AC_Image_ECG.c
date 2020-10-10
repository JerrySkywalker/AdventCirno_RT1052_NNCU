//
// Created by jerry on 2020/10/10.
//
#include "AC_Image_ECG.h"

#define ECG_TEST (1U)

/** THe Raw ECG Data**/
uint8_t ECG_RawData[128];

/** The Identifier for Loop Rendering**/
uint8_t ECG_LoopIdentifier;

/** Image Buffer for ECG Only**/
uint8_t ECG_Buffer[8][128];


extern const uint8_t gImage_100Logo[8][128];

status_t AC_Image_ECG_Show(void){

	ECG_LoopIdentifier = 0;

	/**Init Benchmark**/
#if defined(ECG_TEST) && (ECG_TEST > 0)
	for(int i = 0;i<=63;i++)
	{
		ECG_RawData[i] = i;
		ECG_RawData[127-i] = i;
	}

#endif

    while(true)
    {
        if ((KEY_P_DOWN == Key_Check(KEY_ENTER)) || (KEY_P_DOWN == Key_Check(KEY_RIGHT))) {
            delay_ms(10);
            if ((KEY_P_DOWN == Key_Check(KEY_ENTER)) || (KEY_P_DOWN == Key_Check(KEY_RIGHT))) {

            	/** Exit **/
                break;
            }
        }

        AC_Image_ECG_ConvertBuffer();

        ECG_LoopIdentifier+=10;
        if (ECG_LoopIdentifier >127)
        {
        	ECG_LoopIdentifier = 0;
        }

        /**Show ECG**/
        OLED_Fill(0);
        OLED_Image_ShowLoop_128x64(ECG_LoopIdentifier);
        delay_ms(500);

        //OLED_Image_ShowLoop_128x64(&ECG_Buffer,ECG_LoopIdentifier);


    }


    return kStatus_Success;
}

/**
 * @brief Convert Raw ECG data into a image
 * **/
status_t AC_Image_ECG_ConvertBuffer(void){

	for(int i = 0;i<128;i++)
	{
        uint8_t relativePos = ECG_RawData[i]>>2;

        uint8_t col_group = relativePos>>3;
        uint8_t col_pos = relativePos%8;

        ECG_Buffer[col_group][i] = 1<<col_pos;
	}

    return kStatus_Success;
}


/** 电赛专用 OLED接口 ,暂时写在这里，稍后移植至OLED模块**/

/**********************************************************************************************************************
*  @brief      OLED循环刷新ECG
*  @param      loopIdentifier      	循环开始时的y坐标。本函数不自动刷新，需要使用者手动更新LoopIdentifier
*  @return     void
*  @since      v1.0
*  Sample usage:
**********************************************************************************************************************/
void OLED_Image_ShowLoop_128x64(uint8_t loopIdentifier)
{
	uint8_t y, x;
	for (y = 1; y < 8; y++)
	{
		OLED_WrCmd(0xb0 + y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for (x = 0; x < 128-loopIdentifier; x++)
			OLED_WrDat(gImage_100Logo[y][x+loopIdentifier]);
		for (x = 0; x < loopIdentifier; x++)
			OLED_WrDat(gImage_100Logo[y][x]);
	}
}

