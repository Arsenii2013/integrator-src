#ifndef _MAIN_H_
#define _MAIN_H_


#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define TEST
#define DEBUG


#ifndef TEST
    #include "platform.h"
    #include "xil_printf.h"
    #include "xparameters.h"
    #include "xgpiops.h"
#endif


#ifdef TEST
    #include <stdio.h>
    #define TM_PRINTF(f_, ...) printf((f_), ##__VA_ARGS__)
#else
    #define TM_PRINTF(f_, ...) xil_printf((f_), ##__VA_ARGS__)
#endif

#define DDS_SYNC_PRD 20

#endif // _MAIN_H_