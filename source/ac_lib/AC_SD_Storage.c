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
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "smartcar/status.h"
#include "smartcar/sc_ac_delay.h"
#include "smartcar/sc_gpio.h"
#include "diskio.h"
#include "ff.h"
#include "fsl_sd.h"
#include "fsl_debug_console.h"
#include "fsl_sd_disk.h"
#include "fsl_common.h"

#include "pin_mux.h"
#include "clock_config.h"

TaskHandle_t AC_SD_MenuSave_task_handle;
TaskHandle_t AC_SD_MenuLoad_task_handle;

static FATFS g_fileSystem; /* File system object */
static FIL g_fileObject;   /* File object */

int AC_SD_MenuSave()
{
	if (xSemaphoreTake(s_fileAccessSemaphore, s_taskSleepTicks) == pdTRUE)
	{

	}
}
