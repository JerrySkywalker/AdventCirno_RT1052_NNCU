/*
 * AC_SD_Storage.c
 *
 *  Created on: 2020年7月28日
 *      Author: jerry
 */

#include "AC_SD_Storage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MIMXRT1052.h"
#include "board.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "fsl_sdmmc_host.h"
#include "fsl_sd.h"
#include "fsl_debug_console.h"
#include "fsl_sd_disk.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "ff.h"

#include "smartcar/sc_oled.h"
#include "smartcar/status.h"
#include "smartcar/sc_ac_delay.h"
#include "smartcar/sc_gpio.h"
#include "smartcar/sc_sd.h"
#include "smartcar/sc_ac_key_5D.h"

#include "ac_lib/AC_Menu.h"


extern Data_t data[10];

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* buffer size (in byte) for read/write operations */
#define BUFFER_SIZE_MENU sizeof(data)+2U

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*A multiplicator for data modification */
extern int multiplicator;

/*A operator marker for data modification, 1 is plus ,-1 is minus*/
extern int pm;

static const int PRINT_START_DATA = 90, PRINT_START_MULTIPLICATOR = 84, PRINT_LENGTH_DATA = 5;

/* some location mark related to CURSOR rendering on 128x64 OLED*/
static const int CURSOR_SUP = 1, CURSOR_INF = 7;

static int g_Flag_FileNameDefault;
static int g_Name_MenuFileName;

static FATFS g_fileSystem; /* File system object */
static FIL g_fileObject_Menu;   /* File object */

/* @brief decription about the read/write buffer
 * The size of the read/write buffer should be a multiple of 512, since SDHC/SDXC card uses 512-byte fixed
 * block length and this driver example is enabled with a SDHC/SDXC card.If you are using a SDSC card, you
 * can define the block length by yourself if the card supports partial access.
 * The address of the read/write buffer should align to the specific DMA data buffer address align value if
 * DMA transfer is used, otherwise the buffer address is not important.
 * At the same time buffer address/size should be aligned to the cache line size if cache is supported.
 */
SDK_ALIGN(uint8_t g_bufferWrite_Menu[SDK_SIZEALIGN(BUFFER_SIZE_MENU, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
SDK_ALIGN(uint8_t g_bufferRead_Menu[SDK_SIZEALIGN(BUFFER_SIZE_MENU, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));

status_t AC_SD_MenuSave(StorageMode_t mode)
{
    FRESULT error;
    DIR directory; /* Directory object */
    FILINFO fileInformation;
    UINT bytesWritten;
    UINT bytesRead;
    const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
    volatile bool failedFlag           = false;
    char ch                            = '0';
    BYTE work[FF_MAX_SS];


    /** For file name change **/
    multiplicator = 1;          /*due to the limitation of 5D-key, file Name can only be numbers.Therefore we can change numbers for file names*/
    g_Flag_FileNameDefault = 0;
    g_Name_MenuFileName = 0;

    char *MenuDir = "/menu/";
    char *MenuNameDefault = "default";
    char *FullPath = (char *) pvPortMalloc(20);

    /** Start **/

    PRINTF("[O K] AC: SD: Start task: Save Menu\r\n");

    if(mode!=ModeBoot)
    {
        OLED_P6x8Str(0,0,(uint8_t*)"SD:Load Menu");
        OLED_P6x8Str(0,1,(uint8_t*)"Mode");

        if(mode == ModeDefault)
        {
            OLED_P6x8Str(60,1,(uint8_t*)"Default");
            PRINTF("[O K] AC: SD: Mode-Default\r\n");
        }
        else if(mode  == ModeSync)
        {
            OLED_P6x8Str(60,1,(uint8_t*)"Sync");
            PRINTF("[O K] AC: SD: Mode-Sync\r\n");
        }
    }
    else{
        PRINTF("[O K] AC: SD: Mode-Boot\r\n");
    }

    PRINTF("[O K] AC: SD: Set names\r\n");

    if(mode == ModeDefault) {
        OLED_P6x8Str(0, 2, (uint8_t *) "Use default name?");
        while (true) {
            if ((KEY_P_DOWN == Key_Check(KEY_ENTER)) || (KEY_P_DOWN == Key_Check(KEY_RIGHT))) {
                delay_ms(10);
                if ((KEY_P_DOWN == Key_Check(KEY_ENTER)) || (KEY_P_DOWN == Key_Check(KEY_RIGHT))) {
                    g_Flag_FileNameDefault = 0;
                    break;
                }
            }
            if (KEY_P_DOWN == Key_Check(KEY_LEFT)) {
                delay_ms(10);
                if (KEY_P_DOWN == Key_Check(KEY_LEFT)) {
                    g_Flag_FileNameDefault = 1;
                    break;
                }
            }
        }

        Str_Clr(0, 2, 21);
    }
    else
    {
        g_Flag_FileNameDefault = -1;
    }

    if(1 == g_Flag_FileNameDefault)
    {
        OLED_P6x8Str(0,2,(uint8_t*)"Dst Name?");

        Str_Clr(PRINT_START_MULTIPLICATOR, 0, PRINT_LENGTH_DATA + 1);
        OLED_P6x8Str(PRINT_START_MULTIPLICATOR, 0, (uint8_t*)"X");
        OLED_Print_Num(PRINT_START_DATA, 0, multiplicator);

        OLED_Print_Num(PRINT_START_DATA,2,g_Name_MenuFileName);

        while (Key_Check(KEY_ENTER) != KEY_P_DOWN) {
            delay_ms(10);
            if (Key_Check(KEY_ENTER) != KEY_P_DOWN) {
                if (KEY_P_DOWN == Key_Check(KEY_DOWN)) {
                    Str_Clr(PRINT_START_DATA, 2, 6);
                    pm = -1;
                    g_Name_MenuFileName += pm * multiplicator;

                    if(g_Name_MenuFileName<0)
                        g_Name_MenuFileName = 0;

                    OLED_Print_Num(PRINT_START_DATA, 2, g_Name_MenuFileName);
                }
                if (KEY_P_DOWN == Key_Check(KEY_UP)) {
                    Str_Clr(PRINT_START_DATA, 2, 6);
                    pm = 1;
                    g_Name_MenuFileName += pm * multiplicator;

                    if(g_Name_MenuFileName>30000)
                        g_Name_MenuFileName = 30000;

                    OLED_Print_Num(PRINT_START_DATA, 2, g_Name_MenuFileName);
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
    }

    Str_Clr(0,2,23);

    SD_EnterCritical();

    error = f_mkdir(_T("/menu"));
    if (error)
    {
        if (error == FR_EXIST)
        {
            PRINTF("[O K] AC: SD: Directory exists.\r\n");
            if(mode!=ModeBoot){
                OLED_P6x8Str(0,2,(uint8_t*)"O K - Dir exist");
            }
        }
        else
        {
            PRINTF("[Err] AC: SD: Error Code %d\r\n",error);
            PRINTF("[Err] AC: SD: Make directory failed.\r\n");
            if(mode!=ModeBoot){
                OLED_P6x8Str(0,2,(uint8_t*)"Err - Mk Dir");
            }
            SD_ExitCritical();
            return kStatus_Fail;
        }
    }
    else
    {
        PRINTF("[O K] AC: SD: Successfully make dir /data.\r\n");
        if(mode!=ModeBoot){
            OLED_P6x8Str(0,2,(uint8_t*)"O K - Mk Dir");
        }
    }

    if(-1==g_Flag_FileNameDefault)
    {
    	PRINTF("[O K] AC: SD: Save to Sync.\r\n");
    	error = f_open(&g_fileObject_Menu, _T("/menu/sync"), (FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
    }
    else if(0==g_Flag_FileNameDefault)
    {
       error = f_open(&g_fileObject_Menu, _T("/menu/default"), (FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
       PRINTF("[O K] AC: SD: Save to default.\r\n");
    }
    else
    {
       sprintf(FullPath,"%s%d",MenuDir,g_Name_MenuFileName);
       error = f_open(&g_fileObject_Menu, _T(FullPath), (FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
       PRINTF("[O K] AC: SD: Save to %s.\r\n", _T(FullPath));
    }

    if (error)
    {
        if (error == FR_EXIST)
        {
            PRINTF("[O K] AC: SD: File exists.\r\n");
            if(mode!=ModeBoot){
                OLED_P6x8Str(0,3,(uint8_t*)"O K - File exist");
            }
        }
        else
        {
            PRINTF("[Err] AC: SD: Error Code %d\r\n",error);
            PRINTF("[Err] AC: SD: Open file failed.\r\n");
            if(mode!=ModeBoot){
                OLED_P6x8Str(0,3,(uint8_t*)"Err - Open File");
            }
            SD_ExitCritical();
            return kStatus_Fail;
        }
    }
    else
    {
        PRINTF("[O K] AC: SD: Successfully open file \r\n");
        if(mode!=ModeBoot){
            OLED_P6x8Str(0,3,(uint8_t*)"O K - File opened");
        }
    }



    memcpy(g_bufferWrite_Menu,data,sizeof(data));
    g_bufferWrite_Menu[BUFFER_SIZE_MENU - 2U] = '\r';
    g_bufferWrite_Menu[BUFFER_SIZE_MENU - 1U] = '\n';

    PRINTF("[O K] AC: SD: Write/read file until encounters error......\r\n");

    PRINTF("[O K] AC: SD: Write to above created file.\r\n");
    error = f_write(&g_fileObject_Menu, g_bufferWrite_Menu, sizeof(g_bufferWrite_Menu), &bytesWritten);
    if ((error) || (bytesWritten != sizeof(g_bufferWrite_Menu)))
    {
        PRINTF("[Err] AC: SD: Error Code %d\r\n",error);
        PRINTF("[Err] AC: SD: Write file failed. \r\n");
        failedFlag = true;
        if(mode!=ModeBoot){
            OLED_P6x8Str(0,4,(uint8_t*)"Err - Write File");
        }
        SD_ExitCritical();
        return kStatus_Fail;
    }

    /* Move the file pointer */
    if (f_lseek(&g_fileObject_Menu, 0U))
    {
        PRINTF("[Err] AC: SD: Error Code %d\r\n",error);
        PRINTF("[Err] AC: SD: Set file pointer position failed. \r\n");
        if(mode!=ModeBoot){
            OLED_P6x8Str(0,4,(uint8_t*)"Err - Mv pointer");
        }
        failedFlag = true;

        SD_ExitCritical();
        return kStatus_Fail;
    }

    PRINTF("[O K] AC: SD: Read from above created file.\r\n");
    memset(g_bufferRead_Menu, 0U, sizeof(g_bufferRead_Menu));
    error = f_read(&g_fileObject_Menu, g_bufferRead_Menu, sizeof(g_bufferRead_Menu), &bytesRead);
    if ((error) || (bytesRead != sizeof(g_bufferRead_Menu)))
    {
        PRINTF("[Err] AC: SD: Error Code %d\r\n",error);
        PRINTF("[Err] AC: SD: Read file failed. \r\n");
        if(mode!=ModeBoot){
            OLED_P6x8Str(0,4,(uint8_t*)"Err - Read file");
        }
        failedFlag = true;

        SD_ExitCritical();
        return kStatus_Fail;
    }

    PRINTF("[O K] AC: SD: Compare the read/write content......\r\n");
    if (memcmp(g_bufferWrite_Menu, g_bufferRead_Menu, sizeof(g_bufferWrite_Menu)))
    {
        PRINTF("[Err] AC: SD: Compare read/write content isn't consistent.\r\n");
        if(mode!=ModeBoot){
            OLED_P6x8Str(0,5,(uint8_t*)"Err - R/W not same");
        }
        failedFlag = true;

        SD_ExitCritical();
        return kStatus_Fail;
    }
    PRINTF("[O K] AC: SD: The read/write content is consistent.\r\n");
    if(mode!=ModeBoot){
        OLED_P6x8Str(0,4,(uint8_t*)"O K - Write file");
    }

    if (f_close(&g_fileObject_Menu))
    {
        PRINTF("[Err] AC: SD: Close file failed.\r\n");
        if(mode!=ModeBoot){
            OLED_P6x8Str(0,5,(uint8_t*)"Err - Close file");
        }
        SD_ExitCritical();
        return kStatus_Fail;
    }

    SD_ExitCritical();
    return kStatus_Success;
}


status_t AC_SD_MenuLoad(StorageMode_t mode)
{
    FRESULT error;
    DIR directory; /* Directory object */
    FILINFO fileInformation;
    UINT bytesWritten;
    UINT bytesRead;
    const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
    volatile bool failedFlag           = false;
    char ch                            = '0';
    BYTE work[FF_MAX_SS];


    /** For file name change **/
    multiplicator = 1;          /*due to the limitation of 5D-key, file Name can only be numbers.Therefore we can change numbers for file names*/
    g_Flag_FileNameDefault = 0;
    g_Name_MenuFileName = 0;

    char *MenuDir = "/menu/";
    char *MenuNameDefault = "default";
    char *FullPath = (char *) pvPortMalloc(20);

    /** Start **/

    PRINTF("[O K] AC: SD: Start task: Load Menu\r\n");

    f_mount(&g_fileSystem, driverNumberBuffer, 1U);

    if(mode!=ModeBoot)
    {
        OLED_P6x8Str(0,0,(uint8_t*)"SD:Load Menu");
        OLED_P6x8Str(0,1,(uint8_t*)"Mode");

        if(mode == ModeDefault)
        {
            OLED_P6x8Str(60,1,(uint8_t*)"Default");
            PRINTF("[O K] AC: SD: Mode-Default\r\n");
        }
        else if(mode  == ModeSync)
        {
            OLED_P6x8Str(60,1,(uint8_t*)"Sync");
            PRINTF("[O K] AC: SD: Mode-Sync\r\n");
        }
    }
    else{
        PRINTF("[O K] AC: SD: Mode-Boot\r\n");
    }



    if(mode==ModeDefault) {

        PRINTF("[O K] AC: SD: Set names\r\n");
        OLED_P6x8Str(0, 2, (uint8_t *) "Load default?");
        while (true) {
            if ((KEY_P_DOWN == Key_Check(KEY_ENTER)) || (KEY_P_DOWN == Key_Check(KEY_RIGHT))) {
                delay_ms(10);
                if ((KEY_P_DOWN == Key_Check(KEY_ENTER)) || (KEY_P_DOWN == Key_Check(KEY_RIGHT))) {
                    g_Flag_FileNameDefault = 0;
                    break;
                }
            }
            if (KEY_P_DOWN == Key_Check(KEY_LEFT)) {
                delay_ms(10);
                if (KEY_P_DOWN == Key_Check(KEY_LEFT)) {
                    g_Flag_FileNameDefault = 1;
                    break;
                }
            }
        }

        Str_Clr(0, 2, 21);
    }
    else if(mode == ModeBoot)
    {
        g_Flag_FileNameDefault = -1;
    }

    SD_EnterCritical();

    if(1 == g_Flag_FileNameDefault) {
        OLED_P6x8Str(0, 2, (uint8_t *) "Src Name?");

        int temp_Flag_DirReadLoop = 1;

        PRINTF("[O K] AC: SD: List the file in that directory......\r\n");
        if (f_opendir(&directory, "/MENU")) {
            PRINTF("[Err] AC: SD: Open directory failed.\r\n");
            OLED_Print_Num(0, 2, (uint8_t*)"Err - Open Dir");
            SD_ExitCritical();
            return kStatus_Fail;
        }


        while (temp_Flag_DirReadLoop) {
            error = f_readdir(&directory, &fileInformation);

            /* To the end. */
            if ((error != FR_OK) || (fileInformation.fname[0U] == 0U)) {
                OLED_P6x8Str(0, 3, (uint8_t*)"Err - Cancelled");
                PRINTF("[Err] AC: SD: Cancelled by User.\r\n");

                SD_ExitCritical();
                return kStatus_Fail;      // 读取失败或者读取完所有条目,取消任务
            }
            if (fileInformation.fname[0] == '.')    // 隐藏文件
            {
                continue;
            }
            if (fileInformation.fattrib & AM_DIR)   // 是文件夹
            {
                continue;
            } else {
                Str_Clr(72, 2, 9);
                OLED_P6x8Str(72, 2, (uint8_t*)fileInformation.fname);

                int temp_Flag_DirFilterWaiting = 1;

                while (temp_Flag_DirFilterWaiting) {
                    if (Key_Check(KEY_ENTER) == KEY_P_DOWN) {
                        delay_ms(10);
                        if (Key_Check(KEY_ENTER) == KEY_P_DOWN) {
                            sprintf(FullPath, "%s%s", MenuDir, fileInformation.fname);
                            temp_Flag_DirReadLoop = 0;
                            break;
                        }
                    } else if (Key_Check(KEY_DOWN) == KEY_P_DOWN) {
                        delay_ms(10);
                        if (Key_Check(KEY_DOWN) == KEY_P_DOWN) {
                            temp_Flag_DirFilterWaiting = 0;
                        }
                    }
                }
            }
        }
    }
    if(mode!=ModeBoot){
    	Str_Clr(0,2,23);
    }

    error = f_mkdir(_T("/menu"));
    if (error)
    {
        if (error == FR_EXIST)
        {
            PRINTF("[O K] AC: SD: Directory exists.\r\n");
            if(mode!=ModeBoot){
                OLED_P6x8Str(0,2,(uint8_t*)"O K - Dir exist");
            }
        }
        else if(error == FR_NO_FILESYSTEM){
            PRINTF("[Err] AC: SD: Error Code %d\r\n",error);
            PRINTF("[ERR] AC: SD: No valid file system for FatFs.Ready to make  a New Filesystem...\r\n");

        	PRINTF("[O K] AC: SD: Press WAKEUP key to Start Reformat...\r\n");
			gpio_t wakeUp = {XSNVS_WAKEUP_GPIO, XSNVS_WAKEUP_PIN, 0};
			while (GPIO_Read(&wakeUp))
			{
				delay_ms(10);
			}

			PRINTF("[O K] AC: SD: Make file system......The time may be long if the card capacity is big.\r\n");
			if (f_mkfs(driverNumberBuffer, FM_ANY, 0U, work, sizeof work))
			{
				PRINTF("[Err] AC: SD: Make filesystem failed.\r\n");
                SD_ExitCritical();
				return kStatus_Fail;
			}
        }
        else
        {
            PRINTF("[Err] AC: SD: Error Code %d\r\n",error);
            PRINTF("[Err] AC: SD: Make directory failed.\r\n");
            if(mode!=ModeBoot){
                OLED_P6x8Str(0,2,(uint8_t*)"Err - Mk Dir");
            }

            SD_ExitCritical();
            return kStatus_Fail;
        }
    }
    else
    {
        PRINTF("[O K] AC: SD: Successfully make dir /data.\r\n");
        if(mode!=ModeBoot){
            OLED_P6x8Str(0,2,(uint8_t*)"O K - Mk Dir");
        }
    }


    if(-1 == g_Flag_FileNameDefault)
    {
    	PRINTF("[O K] AC: SD: Load from SYNC.\r\n");
    	error = f_open(&g_fileObject_Menu, _T("/MENU/SYNC"), (FA_READ));
    }
    else if(0==g_Flag_FileNameDefault)
    {
    	PRINTF("[O K] AC: SD: Load from DEFAULT.\r\n");
    	error = f_open(&g_fileObject_Menu, _T("/MENU/DEFAULT"), (FA_READ));
    }
    else
    {
    	PRINTF("[O K] AC: SD: Load from %s.\r\n", _T(FullPath));
    	error = f_open(&g_fileObject_Menu, _T(FullPath), ( FA_READ));
    }

    if (error)
    {
        if (error == FR_EXIST)
        {
            PRINTF("[O K] AC: SD: File exists.\r\n");
            if(mode!=ModeBoot){
                OLED_P6x8Str(0,3,(uint8_t*)"O K - File exist");
            }
        }
        else
        {
            PRINTF("[Err] AC: SD: Error Code %d\r\n",error);
            PRINTF("[Err] AC: SD: Open file failed.\r\n");
            if(mode!=ModeBoot){
                OLED_P6x8Str(0,3,(uint8_t*)"Err - Open File");
            }
            SD_ExitCritical();
            return kStatus_Fail;
        }
    }
    else
    {
        PRINTF("[O K] AC: SD: Successfully open file \r\n");
        if(mode!=ModeBoot){
            OLED_P6x8Str(0,3,(uint8_t*)"O K - File opened");
        }
    }



    PRINTF("[O K] AC: SD: Read from above created file.\r\n");
    memset(g_bufferRead_Menu, 0U, sizeof(g_bufferRead_Menu));


    error = f_read(&g_fileObject_Menu, g_bufferRead_Menu, sizeof(g_bufferRead_Menu), &bytesRead);
    if ((error) || (bytesRead != sizeof(g_bufferRead_Menu)))
    {
        PRINTF("[Err] AC: SD: Error Code %d\r\n",error);
        PRINTF("[Err] AC: SD: Read file failed. \r\n");
        if(mode!=ModeBoot){
            OLED_P6x8Str(0,4,(uint8_t*)"Err - Read file");
        }
        failedFlag = true;
        SD_ExitCritical();
        return kStatus_Fail;
    }

    memcpy(data,g_bufferRead_Menu,sizeof(data));
    if(mode!=ModeBoot){
        OLED_P6x8Str(0,4,(uint8_t*)"O K - Read file");
    }

    if (f_close(&g_fileObject_Menu))
    {
        PRINTF("[Err] AC: SD: Close file failed.\r\n");
        if(mode!=ModeBoot){
            OLED_P6x8Str(0,5,(uint8_t*)"Err - Close file");
        }
        SD_ExitCritical();
        return kStatus_Fail;
    }

    SD_ExitCritical();
    return kStatus_Success;
}
