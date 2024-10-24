#ifndef _EXTERNAL_START_H_
#define _EXTERNAL_START_H_
#include "main.h"

typedef struct{
    uint32_t SR;
    uint32_t CR;
    uint32_t CR_S;
    uint32_t CR_C;
    uint32_t EV;
    uint32_t DELAY;
} externalStartRegs;

#define SR_IDLE   0

#define CR_CLEAR  0
#define CR_WRITE  1
#define CR_START  2
#define CR_REPEAT 3

int checkExternalStartRunning();
void setExternalStartSequency(uint32_t * events, uint32_t * delays, int cnt);

#endif //_EXTERNAL_START_H_