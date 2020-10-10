//
// Created by jerry on 2020/10/10.
//
#include "AC_Image_ECG.h"

/** THe Raw ECG Data**/
uint8_t ECG_RawData[128];

/** The Identifier for Loop Rendering**/
uint8_t ECG_LoopIdentifier;

/** Image Buffer for ECG Only**/
uint8_t ECG_BUffer[8][128];

status_t AC_Image_ECG_Show(void){
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

        /**Show ECG**/
        OLED_Fill(0);


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

        ECG_BUffer[col_group][i] = 1<<col_pos;
	}

    return kStatus_Success;
}

