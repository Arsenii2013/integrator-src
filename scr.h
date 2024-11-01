#ifndef _SCR_H_
#define _SCR_H_
#include "main.h"

#define SCR_EVENTS_N 4
typedef struct
{
    uint32_t SR;
    uint32_t CR;
    uint32_t CR_S;
    uint32_t CR_C;
    uint32_t AFE_ERR;
    uint32_t LOG_ERR;
    uint32_t HP_ERR;
    uint32_t MODE;
    uint32_t EXT;
    uint32_t B0;
    uint32_t K_ANALOG_TO_B;
    uint32_t BSER_IN;
    uint32_t K_B_TO_ANALOG;
    uint32_t BSER_OUT;
    uint32_t START_EV[SCR_EVENTS_N];
    uint32_t STOP_EV[SCR_EVENTS_N];
    uint32_t ZERO_EV;
    uint32_t CALIBRATION_EV;
} statusControlRegisters;

#define SR_TIME       0
#define SR_OVERFLOW   1
#define SR_INVALID    2

#define CR_CLEAR      0
#define CR_AFE_PWR    1

#define EXT_EXT       0
#define EXT_CYCLE_CAL 1
#define EXT_CAL       2
#define EXT_SOFT      3

#define AFE_ERR_CALIBRATION 0
#define AFE_ERR_STARTSTOP   1
#define AFE_ERR_STOPSTOP    2
#define AFE_ERR_STARTSTART  3
#define AFE_ERR_STATE       4

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
void statusAFEState(uint32_t AFEState);

void statusLogStartStop();
void statusLogStopStop();
void statusLogStartStart();
void statusLogSwitch();
void statusLogOverflow();

int controlDDS_SYNC(void*);

uint32_t controlStartEv(uint32_t i);
uint32_t controlStopEv(uint32_t i);
uint32_t controlZeroEv();
uint32_t controlCalEv();
uint32_t controlAFEPwr();

float controlB0();
float controlCoeffAB();
float controlBserIn();
float controlCoeffBA();
float controlBserOut();

uint32_t controlMode();

uint32_t controlExtTrig();
uint32_t controlExtTrigCycCal();
uint32_t controlExtTrigCal();
uint32_t controlExtTrigSoft();
void statusExtTrigSoft();
void statusExtTrigCal();

void initSCR();
volatile statusControlRegisters * SCRegPtr();

#endif // _SCR_H_