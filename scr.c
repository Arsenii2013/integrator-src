#include "scr.h"
#ifndef TEST
#include "xil_cache.h"    
#endif

//statusControlRegisters * REGS_BASE_SCR = 0x40000000 + 0x2000;
volatile statusControlRegisters * REGS_BASE_SCR = (statusControlRegisters *)0x3A001000;

void initSCR(){
    #ifdef TEST
        REGS_BASE_SCR = malloc(sizeof(statusControlRegisters));
        statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    #endif
    REGS_BASE_SCR->SR = 0;
    REGS_BASE_SCR->CR = 1 << CR_AFE_PWR;
    REGS_BASE_SCR->CR_S = 0;
    REGS_BASE_SCR->CR_C = 0;
    REGS_BASE_SCR->AFE_ERR = 0;
    REGS_BASE_SCR->LOG_ERR = 0;
    REGS_BASE_SCR->HP_ERR = 0;
    REGS_BASE_SCR->B0 = 0;
    REGS_BASE_SCR->START_EV[0] = 0;
    REGS_BASE_SCR->START_EV[1] = 0;
    REGS_BASE_SCR->START_EV[2] = 0;
    REGS_BASE_SCR->START_EV[3] = 0;
    REGS_BASE_SCR->STOP_EV[0] = 0;
    REGS_BASE_SCR->STOP_EV[1] = 0;
    REGS_BASE_SCR->STOP_EV[2] = 0;
    REGS_BASE_SCR->STOP_EV[3] = 0;
    REGS_BASE_SCR->ZERO_EV = 0;
    REGS_BASE_SCR->CALIBRATION_EV = 0;
    REGS_BASE_SCR->MODE = 0;
    REGS_BASE_SCR->EXT = 1 << EXT_CYCLE_CAL;
    REGS_BASE_SCR->K_ANALOG_TO_B = 0;
    REGS_BASE_SCR->BSER_IN = 0;
    REGS_BASE_SCR->K_B_TO_ANALOG = 0;
    REGS_BASE_SCR->BSER_OUT = 0;
}

volatile statusControlRegisters * SCRegPtr(){
    return REGS_BASE_SCR;
}

void flushIfnTEST(){
    #ifndef TEST
    Xil_DCacheFlushRange((intptr_t)SCRegPtr(), sizeof(statusControlRegisters));
    #endif
}

void statusNotEnoughtTime(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->SR |= 1 << SR_TIME;
    flushIfnTEST();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: events were not processed in one DDS_SYNC cycle\n\r");
    #endif
}

void statusOverflowEvents(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->SR |= 1 << SR_OVERFLOW;
    flushIfnTEST();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: event fifo overflow\n\r");
    #endif
}

void statusInvalidEvents(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->SR |= 1 << SR_INVALID;
    flushIfnTEST();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: invalid events sequnce\n\r");
    #endif
}


int controlDDS_SYNC(void*){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    
    regs->CR |= regs->CR_S;
    regs->CR &= ~regs->CR_C;
    regs->CR_S = 0;
    regs->CR_C = 0;

    if(regs->CR & 0x1){
        regs->CR &= ~0x1;
        regs->SR = 0;
        regs->AFE_ERR = 0;
        regs->LOG_ERR = 0;
        regs->HP_ERR = 0;
    }
    return 0;
}

void statusAFEStartStop(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR |= 1 << AFE_ERR_STARTSTOP;
    flushIfnTEST();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: AFE: start and stop in one DDS_SYNC cycle\n\r");
    #endif
}
void statusAFEStopStop(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR |= 1 << AFE_ERR_STOPSTOP;
    flushIfnTEST();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: AFE: try to stop stopped integral\n\r");
    #endif
}
void statusAFEStartStart(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR |= 1 << AFE_ERR_STARTSTART;
    flushIfnTEST();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: AFE: try to start running integral\n\r");
    #endif
}
void statusAFECallibration(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR |= 1 << AFE_ERR_CALIBRATION;
    flushIfnTEST();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: AFE: try to handle while callibration\n\r");
    #endif
}

void statusAFEState(uint32_t AFEState){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR = (regs->AFE_ERR & ~(0b111 << AFE_ERR_STATE)) | AFEState << AFE_ERR_STATE;
    flushIfnTEST();
    #ifdef DEBUG
    if(AFEState != 0){
        TM_PRINTF("DEBUG: AFE: state = %x\n\r", AFEState);
    }
    #endif
}

void statusLogStartStop(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->LOG_ERR |= 1 << LOG_ERR_STARTSTOP;
    flushIfnTEST();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: LOG: start and stop in one DDS_SYNC cycle\n\r");
    #endif
}
void statusLogStopStop(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->LOG_ERR |= 1 << LOG_ERR_STOPSTOP;
    flushIfnTEST();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: LOG: try to stop stopped log\n\r");
    #endif
}
void statusLogStartStart(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->LOG_ERR |= 1 << LOG_ERR_STARTSTART;
    flushIfnTEST();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: LOG: try to start running log\n\r");
    #endif
}
void statusLogSwitch(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->LOG_ERR |= 1 << LOG_ERR_SWITCH;
    flushIfnTEST();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: LOG: try to switch bank while running\n\r");
    #endif
}


void statusLogOverflow(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->LOG_ERR |= 1 << LOG_ERR_OVERFLOW;
    flushIfnTEST();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: LOG: bank overflow\n\r");
    #endif
}

uint32_t controlAFEPwr(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->CR & (1 << CR_AFE_PWR);
}

uint32_t controlExtTrig(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    uint32_t res = regs->EXT & (1 << EXT_EXT);
    return res;
}

uint32_t controlExtTrigCycCal(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->EXT & (1 << EXT_CYCLE_CAL);
}

uint32_t controlExtTrigCal(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->EXT & (1 << EXT_CAL);
}

uint32_t controlExtTrigSoft(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    uint32_t res = regs->EXT & (1 << EXT_SOFT);
    if(res) 
        regs->EXT &= ~(1 << EXT_SOFT);
    return res;
}

uint32_t controlStartEv(uint32_t i){
    if(i > SCR_EVENTS_N){
        #ifdef DEBUG
        TM_PRINTF("DEBUG: control registers read err\n\r");
        #endif
        return 0;
    }
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->START_EV[i];
}

uint32_t controlStopEv(uint32_t i){
    if(i > SCR_EVENTS_N){
        #ifdef DEBUG
        TM_PRINTF("DEBUG: control registers read err\n\r");
        #endif
        return 0;
    }
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->STOP_EV[i];
}

uint32_t controlZeroEv(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->ZERO_EV;
}

uint32_t controlCalEv(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->CALIBRATION_EV;
}

float controlB0(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return *(float*)&regs->B0;
}

float controlCoeffAB(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    uint32_t K_sens = regs->K_ANALOG_TO_B;
    float K = 2.5 / 2.487951 * (float)DDS_SYNC_PRD * powf(10., -6) / powf(2., 17) / *(float*)&K_sens;
    return K;
}

float controlBserIn(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return *(float*)&regs->BSER_IN;
}

float controlCoeffBA(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    float K = *(float*)&regs->K_B_TO_ANALOG * powf(2., 14) / 10.75;
    return K;
}

float controlBserOut(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return *(float*)&regs->BSER_OUT;
}


uint32_t controlMode(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->MODE;
}