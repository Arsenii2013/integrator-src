#include "scr.h"

statusControlRegisters * REGS_BASE_SCR = 0x40000000 + 0x2000;

#ifdef TEST
void initSCR(){
    REGS_BASE_SCR = malloc(sizeof(statusControlRegisters));
}
#endif

void statusNotEnoughtTime(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->SR |= 1 << SR_TIME;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: events were not processed in one DDS_SYNC cycle\n\r");
    #endif
}

void statusOverflowEvents(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->SR |= 1 << SR_OVERFLOW;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: event fifo overflow\n\r");
    #endif
}

void statusInvalidEvents(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->SR |= 1 << SR_INVALID;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: invalid events sequnce\n\r");
    #endif
}


void controlDDS_SYNC(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    if(regs->CR & 0x1){
        regs->SR = 0;
        regs->AFE_ERR = 0;
        regs->LOG_ERR = 0;
    }
}

void statusAFEStartStop(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR |= 1 << AFE_ERR_STARTSTOP;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: AFE: start and stop in one DDS_SYNC cycle\n\r");
    #endif
}
void statusAFEStopStop(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR |= 1 << AFE_ERR_STOPSTOP;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: AFE: try to stop stopped integral\n\r");
    #endif
}
void statusAFEStartStart(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR |= 1 << AFE_ERR_STARTSTART;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: AFE: try to start running integral\n\r");
    #endif
}
void statusAFECallibration(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR |= 1 << AFE_ERR_CALIBRATION;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: AFE: try to handle while callibration\n\r");
    #endif
}

void statusLogStartStop(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR |= 1 << LOG_ERR_STARTSTOP;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: LOG: start and stop in one DDS_SYNC cycle\n\r");
    #endif
}
void statusLogStopStop(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR |= 1 << LOG_ERR_STOPSTOP;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: LOG: try to stop stopped log\n\r");
    #endif
}
void statusLogStartStart(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR |= 1 << LOG_ERR_STARTSTART;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: LOG: try to start running log\n\r");
    #endif
}
void statusLogSwitch(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR |= 1 << LOG_ERR_SWITCH;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: LOG: try to switch bank while running\n\r");
    #endif
}


void statusLogOverflow(){
    statusControlRegisters* regs = (statusControlRegisters*) REGS_BASE_SCR;
    regs->AFE_ERR |= 1 << LOG_ERR_OVERFLOW;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: LOG: bank overflow\n\r");
    #endif
}