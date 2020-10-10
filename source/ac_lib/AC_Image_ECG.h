//
// Created by jerry on 2020/10/10.
//

#ifndef ADVENTCIRNO_RT1052_NNCU_AC_IMAGE_ECG_H
#define ADVENTCIRNO_RT1052_NNCU_AC_IMAGE_ECG_H

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

status_t AC_Image_ECG_Show(void);
status_t AC_Image_ECG_ConvertBuffer(void);


#endif //ADVENTCIRNO_RT1052_NNCU_AC_IMAGE_ECG_H
