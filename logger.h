#ifndef _LOGGER_H_
#define _LOGGER_H_
#include "main.h"

#define BANK_NUM 2
#define BANK_MAX_SIZE 100000

typedef struct
{
    uint32_t cfg;
    uint32_t dcm;
    uint32_t size;
} bankRegs;


typedef struct
{
    uint32_t SR;
    uint32_t CR;
    uint32_t CR_S;
    uint32_t CR_C;
    uint32_t CFG;
    uint32_t DCM;
    uint32_t START [BANK_NUM];
    uint32_t STOP  [BANK_NUM];
    bankRegs bankRegs [BANK_NUM];
} logRegs;

#define SR_IDLE   0
#define SR_BANK   1

#define CR_MODE   0
#define CR_STOP   2
#define CR_START  3
#define CR_SWITCH 4

typedef struct
{
    uint32_t desc;
    uint32_t data0;
    uint32_t data1;
    uint32_t data2;
    uint32_t data3;
    uint32_t data4;
    uint32_t data5;
    uint32_t data6;
    uint32_t data7;
} logEntry;

typedef struct
{
    uint32_t desc;
    uint32_t sigmaX;
    uint32_t deltaX;
    uint32_t sigmaY;
    uint32_t deltaY;
    uint32_t df;
    uint32_t f;
} logFB;


typedef struct
{
    uint32_t desc;
    uint32_t integralDigital_low;
    uint32_t integralDigital_high;
    uint32_t integralAnalog_low;
    uint32_t integralAnalog_high;
    uint32_t B_low;
    uint32_t B_high;
} logIntegrator;

volatile logRegs * loggerRegPtr();
void loggerInit();
uint32_t logRunning();

void logg(logEntry e);
int loggerDDS_SYNC(void*);
int loggerEvent(uint32_t ev, void*);

void printLog();

#endif // _LOGGER_H_