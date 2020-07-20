/*
 * AC_Pit.h
 *
 *  Created on: 2020年7月4日
 *      Author: Jerry
 */

#ifndef AC_LIB_AC_PIT_H_
#define AC_LIB_AC_PIT_H_

#include "MIMXRT1052.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "FreeRTOS.h"
#include "task.h"


void AC_Pit(void *pv);

#endif /* AC_LIB_AC_PIT_H_ */
