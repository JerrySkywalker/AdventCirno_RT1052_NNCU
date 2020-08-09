/*** 
 * @Author: JerryW
 * @Date: 2020-06-24 14:59:24
 * @LastEditTime: 2020-06-24 15:15:17
 * @LastEditors: JerryW
 * @Description: 
 * @FilePath: /Cirno_MK66_Gamma/source/Command.h
 */

#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "MIMXRT1052.h"

#define BT_UART LPUART4
#define NUMBER_FORESIGHT 10

void COM_BT_Putchar(char temp);
void COM_BT_Upload();

#endif
