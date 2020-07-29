/*
 * AC_SD_Storage.c
 *
 *  Created on: 2020年7月28日
 *      Author: jerry
 */

#include "AC_SD_Storage.h"

#include <stdio.h>
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

#include "smartcar/status.h"
#include "smartcar/sc_ac_delay.h"
#include "smartcar/sc_gpio.h"
#include "smartcar/sc_sd.h"

#include "ac_lib/AC_Menu.h"


extern Data_t data[10];

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* buffer size (in byte) for read/write operations */
#define BUFFER_SIZE_MENU sizeof(data)+2U

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

status_t AC_SD_MenuSave()
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

    error = f_mkdir(_T("/data"));
    if (error)
    {
        if (error == FR_EXIST)
        {
            PRINTF("[O K] AC: SD: Directory exists.\r\n");
        }
        else
        {
            PRINTF("[Err] AC: SD: Make directory failed.\r\n");
            return kStatus_Fail;
        }
    }
    PRINTF("[O K] AC: SD: Successfully make dir /data.\r\n");

    error = f_open(&g_fileObject_Menu, _T("/data/menu.txt"), (FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
    if (error)
    {
        if (error == FR_EXIST)
        {
            PRINTF("[O K] AC: SD: File exists.\r\n");
        }
        else
        {
            PRINTF("[Err] AC: SD: Open file failed.\r\n");
            return kStatus_Fail;
        }
    }
    PRINTF("[O K] AC: SD: Successfully open menu.txt \r\n");

    memcpy(g_bufferWrite_Menu,data,sizeof(data));
    g_bufferWrite_Menu[BUFFER_SIZE_MENU - 2U] = '\r';
    g_bufferWrite_Menu[BUFFER_SIZE_MENU - 1U] = '\n';

    PRINTF("[O K] AC: SD: Write/read file until encounters error......\r\n");

    PRINTF("[O K] AC: SD: Write to above created file.\r\n");
    error = f_write(&g_fileObject_Menu, g_bufferWrite_Menu, sizeof(g_bufferWrite_Menu), &bytesWritten);
    if ((error) || (bytesWritten != sizeof(g_bufferWrite_Menu)))
    {
        PRINTF("[Err] AC: SD: Write file failed. \r\n");
        failedFlag = true;
        return kStatus_Fail;
    }

    /* Move the file pointer */
    if (f_lseek(&g_fileObject_Menu, 0U))
    {
        PRINTF("[Err] AC: SD: Set file pointer position failed. \r\n");
        failedFlag = true;
        return kStatus_Fail;
    }

    PRINTF("[O K] AC: SD: Read from above created file.\r\n");
    memset(g_bufferRead_Menu, 0U, sizeof(g_bufferRead_Menu));
    error = f_read(&g_fileObject_Menu, g_bufferRead_Menu, sizeof(g_bufferRead_Menu), &bytesRead);
    if ((error) || (bytesRead != sizeof(g_bufferRead_Menu)))
    {
        PRINTF("[Err] AC: SD: Read file failed. \r\n");
        failedFlag = true;
        return kStatus_Fail;
    }

    PRINTF("[O K] AC: SD: Compare the read/write content......\r\n");
    if (memcmp(g_bufferWrite_Menu, g_bufferRead_Menu, sizeof(g_bufferWrite_Menu)))
    {
        PRINTF("[Err] AC: SD: Compare read/write content isn't consistent.\r\n");
        failedFlag = true;
        return kStatus_Fail;
    }
    PRINTF("[O K] AC: SD: The read/write content is consistent.\r\n");

    if (f_close(&g_fileObject_Menu))
    {
        PRINTF("[Err] AC: SD: Close file failed.\r\n");
        return kStatus_Fail;
    }


    return kStatus_Success;
}


status_t AC_SD_MenuLoad()
{
    int error;

    error = f_mkdir(_T("/data"));
    if (error)
    {
        if (error == FR_EXIST)
        {
            PRINTF("[O K] AC: SD: Directory exists.\r\n");
        }
        else
        {
            PRINTF("[Err] AC: SD: Make directory failed.\r\n");
            return kStatus_Fail;
        }
    }

    error = f_open(&g_fileObject_Menu, _T("/data/menu.txt"), (FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
    if (error)
    {
        if (error == FR_EXIST)
        {
            PRINTF("[O K] AC: SD: File exists.\r\n");
        }
        else
        {
            PRINTF("[Err] AC: SD: Open file failed.\r\n");
            return kStatus_Fail;
        }
    }



    return kStatus_Success;
}