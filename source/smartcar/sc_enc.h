#ifndef SC_ENC_H_
#define SC_ENC_H_

#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "fsl_enc.h"
#include "fsl_xbara.h"

void ENC_Init_t(ENC_Type *base);
void ENC_Dateclear(ENC_Type *base);
int16_t ENC_Dateget(ENC_Type *base);
int16_t ENC_Positionget(ENC_Type *base);
int16_t ENC_Revolutionget(ENC_Type *base);
#endif /* SC_ENC_H_ */
