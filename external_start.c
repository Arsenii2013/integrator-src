#include "external_start.h"

volatile externalStartRegs * REGS_BASE_EXT = (externalStartRegs *)0x40002c00;

void writeEvent(uint32_t delay, uint32_t ev){
    externalStartRegs* regs = (externalStartRegs*) REGS_BASE_EXT;
    regs->EV = ev;
    regs->DELAY = delay;
    regs->CR |= 1 << CR_WRITE;
}

void extClearEvents(){
    externalStartRegs* regs = (externalStartRegs*) REGS_BASE_EXT;
    regs->CR |= 1 << CR_CLEAR;
}

int checkExternalStartRunning(){
    externalStartRegs* regs = (externalStartRegs*) REGS_BASE_EXT;
    if(regs->SR & (1 << SR_IDLE)){
        return 0;
    }
    if(regs->CR & (1 << CR_START) || regs->CR & (1 << CR_REPEAT)){
        return -1;
    }
    return -2;
}

void setExternalStartSequency(uint32_t * events, uint32_t * delays, int cnt){
    extClearEvents();
    for(int i = 0; i < cnt; i++){
        writeEvent(delays[i], events[i]);
    }
}