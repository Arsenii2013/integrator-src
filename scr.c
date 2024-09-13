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
    REGS_BASE_SCR->CR = 0;
    REGS_BASE_SCR->CR_S = 0;
    REGS_BASE_SCR->CR_C = 0;
    REGS_BASE_SCR->AFE_ERR = 0;
    REGS_BASE_SCR->LOG_ERR = 0;
    REGS_BASE_SCR->HP_ERR = 0;
    REGS_BASE_SCR->B0 = 0;
    REGS_BASE_SCR->B0_EV = 0;
    REGS_BASE_SCR->START_EV = 0;
    REGS_BASE_SCR->STOP_EV = 0;
    REGS_BASE_SCR->RESET_EV = 0;
    REGS_BASE_SCR->CALIBRATION_EV = 0;
    REGS_BASE_SCR->MODE = 0;
    REGS_BASE_SCR->K_ANALOG_TO_B = 0;
    REGS_BASE_SCR->K_DIGITAL_TO_B = 0;
    REGS_BASE_SCR->K_B_TO_ANALOG = 0;
    REGS_BASE_SCR->K_B_SERIES = 0;
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

uint32_t controlB0(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->B0;
}
uint32_t controlB0Ev(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->B0_EV;
}

uint32_t controlStartEv(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->START_EV;
}

uint32_t controlStopEv(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->STOP_EV;
}

uint32_t controlResetEv(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->RESET_EV;
}

uint32_t controlCalEv(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->CALIBRATION_EV;
}

uint32_t controlKoeffAB(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    uint32_t K_sens = regs->K_ANALOG_TO_B;
    //float K = 2.5 * DDS_SYNC_PRD * powf(10., -6) / 2.487951 / powf(2., 17) / (float)K_sens;
    return K_sens;
}

uint32_t controlKoeffDB(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->K_DIGITAL_TO_B;
}

uint32_t controlKoeffBA(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->K_B_TO_ANALOG;
}

uint32_t controlBSeries(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->K_B_SERIES;
}


uint32_t controlMode(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    return regs->MODE;
}