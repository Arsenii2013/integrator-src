#ifndef _SCR_H_
#define _SCR_H_
#include "main.h"

typedef struct
{
    uint32_t SR;
    uint32_t CR;
    uint32_t CR_S;
    uint32_t CR_C;
    uint32_t AFE_ERR;
    uint32_t LOG_ERR;
    uint32_t HP_ERROR;
    uint32_t B0;
    uint32_t B0_EV;
    uint32_t START_EV;
    uint32_t STOP_EV;
    uint32_t RESET_EV;
    uint32_t CALIBRATION_EV;
} statusControlRegisters;

#define SR_TIME       0
#define SR_OVERFLOW   1
#define SR_INVALID    2

#define CR_CLEAR      0

#define AFE_ERR_CALIBRATION 0
#define AFE_ERR_STARTSTOP   1
#define AFE_ERR_STOPSTOP    2
#define AFE_ERR_STARTSTART  3

#define LOG_ERR_SWITCH      0
#define LOG_ERR_STARTSTOP   1
#define LOG_ERR_STOPSTOP    2
#define LOG_ERR_STARTSTART  3
#define LOG_ERR_OVERFLOW    4

void statusNotEnoughtTime();
void statusOverflowEvents();
void statusInvalidEvents();

void statusAFEStartStop();
void statusAFEStopStop();
void statusAFEStartStart();
void statusAFECallibration();

void statusLogStartStop();
void statusLogStopStop();
void statusLogStartStart();
void statusLogSwitch();
void statusLogOverflow();

void controlDDS_SYNC();

uint32_t controlGetB0();
uint32_t controlB0();
uint32_t controlSTART();
uint32_t controlSTOP();
uint32_t controlRESET();
uint32_t controlCAL();

#ifdef TEST
void initSCR();
#endif

#endif // _SCR_H_