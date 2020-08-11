#ifndef _SC_UPLOAD_H
#define _SC_UPLOAD_H

#include "smartcar/sc_uart.h"
#include "ac_lib/AC_Control.h"

#define BYTE0(Temp)       (*(char *)(&Temp))
#define BYTE1(Temp)       (*((char *)(&Temp) + 1))
#define BYTE2(Temp)       (*((char *)(&Temp) + 2))
#define BYTE3(Temp)       (*((char *)(&Temp) + 3))

void Send_Begin(void);
void  Send_Variable(void);
//void Img_Upload(void);
//void Img_Upload_wxj(uint8_t* upload_img);
#endif
