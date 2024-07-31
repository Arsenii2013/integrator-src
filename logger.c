#include "logger.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

logRegs * REGS_BASE;

void * bankAddrs [BANK_NUM] = {0x100000, 0x200000};
uint32_t bankCnt [BANK_NUM] = {0, 0};

uint32_t logEntrySize(uint32_t desc){
    uint32_t count = 0;
    for (; desc; count++)
        desc &= (desc - 1);
    return count;
}

logRegs * loggerRegPtr(){
    return REGS_BASE;
}

void saveLog(){
    
}

uint32_t logRunning(){
    logRegs* regs = (logRegs*) REGS_BASE;
    return !(regs->SR & 1 << SR_IDLE);
}

void loggerInit(){
    #ifdef TEST
    bankAddrs[0] = malloc(sizeof(logEntry) * BANK_MAX_SIZE);
    bankAddrs[1] = malloc(sizeof(logEntry) * BANK_MAX_SIZE);
    REGS_BASE    = malloc(sizeof(logRegs));
    #endif

    /*const logRegs defaults = {.SR = 1, .CR = 0, .CR_S = 0, .CR_C = 0, .CFG = 0, .DCM = 0, .START = {0, 0}, .STOP = {0, 0},
         .bankRegs = {
            {.cfg = 0, .dcm = 0, .size = 0}, 
            {.cfg = 0, .dcm = 0, .size = 0}
        }
    };*/

    REGS_BASE->SR = 1 << SR_IDLE;
    REGS_BASE->CR = 0;
    REGS_BASE->CR_C = 0;
    REGS_BASE->CR_S = 0;
}

size_t writeEntry(logEntry e, void * addr){
    uint32_t * data = addr;

    uint32_t desc = e.desc;
    size_t   writed = 0;

    for(int i = 0; i < 32; i ++){
        if(desc & (1 << i)){
            *data = *((uint32_t*)(&e) + i + 1);
            data++;
            writed++;
        }
    }
    return writed;
}

void logg(logEntry e){
    logRegs* regs = (logRegs*) REGS_BASE;
    uint8_t activeBank = regs->SR & (1 << SR_BANK) ? 1 : 0;
    
    if(!logRunning()){
        #ifdef DEBUG
        TM_PRINTF("DEBUG: try write stopped log\n");
        #endif
        return;
    }
    if(bankCnt[activeBank] == 0){
        if(regs->bankRegs[activeBank].size == BANK_MAX_SIZE){
            #ifdef DEBUG
            TM_PRINTF("DEBUG: bank %d overflow\n", activeBank);
            #endif
            return;
        }
        e.desc = regs->bankRegs[activeBank].cfg;
        writeEntry(e, bankAddrs[activeBank] + regs->bankRegs[activeBank].size * logEntrySize(regs->bankRegs[activeBank].cfg) * 4);
        regs->bankRegs[activeBank].size ++;
        bankCnt[activeBank] = regs->bankRegs[activeBank].dcm;
    } else {
        bankCnt[activeBank] --;
    }
}

int loggerDDS_SYNC(void*){
    logRegs* regs = (logRegs*) REGS_BASE;

    uint32_t CRStart = (regs->CR & (1 << CR_START )) || (regs->CR_S & (1 << CR_START ));
    uint32_t CRStop  = (regs->CR & (1 << CR_STOP  )) || (regs->CR_S & (1 << CR_STOP  ));

    uint32_t running = !(regs->SR & (1 << SR_IDLE)); 

    if(running){
        if(CRStop){
            regs->SR |= (1 << SR_IDLE);
            regs->CR &= ~(1 << CR_STOP);
            if((regs->CR >> CR_MODE) & 0b01){
                regs->CR |= 1 << CR_SWITCH;
            }
        }
        #ifdef DEBUG
        if(CRStart){
            TM_PRINTF("DEBUG: try to start running log\n");
            return -1;
        }
        #endif
    }else{
        if(CRStart){
            regs->SR &= ~(1 << SR_IDLE);
            regs->CR &= ~(1 << CR_START);
            uint8_t activeBank = regs->SR & (1 << SR_BANK) ? 1 : 0;
            regs->bankRegs[activeBank].dcm = regs->DCM;
            regs->bankRegs[activeBank].cfg = regs->CFG;
            regs->bankRegs[activeBank].size= 0;
        }
        #ifdef DEBUG
        if(CRStop){
            TM_PRINTF("DEBUG: try to stop stopped log\n");
            return -1;
        }
        #endif
    }

    uint32_t CRSwitch = (regs->CR & (1 << CR_SWITCH)) || (regs->CR_S & (1 << CR_SWITCH));
    if(CRSwitch){
        regs->SR ^= (1 << SR_BANK);
    }

    return 0;
}

int loggerEvent(uint32_t ev, void*){
    logRegs* regs = (logRegs*) REGS_BASE;
    if(regs->SR & (1 << SR_IDLE)){ // лог остановлен
        for(int i = 0; i < BANK_NUM; i ++){
            if(ev == regs->START[i]){
                regs->CR |= 1 << CR_START;
            }
            #ifdef DEBUG
            if(ev == regs->STOP[i]){
                TM_PRINTF("DEBUG: stopped log recieved stop event\n");
                return -1;
            }
            #endif
        }
    }else{ // лог запущен
        for(int i = 0; i < BANK_NUM; i ++){
            if(ev == regs->STOP[i]){
                regs->CR |= 1 << CR_STOP;
            }
            #ifdef DEBUG
            if(ev == regs->START[i]){
                TM_PRINTF("DEBUG: running log recieved start event\n");
                return -1;
            }
            #endif
        }
    }
    return 0;
}


void printLog(){
    logRegs* regs = (logRegs*) REGS_BASE;
    uint32_t * log0 = bankAddrs[0], * log1 = bankAddrs[1];
    uint32_t desc0 = regs->bankRegs[0].cfg, desc1 = regs->bankRegs[1].cfg;
    uint32_t esize0 = logEntrySize(desc0), esize1 = logEntrySize(desc1);
    size_t size0 = regs->bankRegs[0].size * esize0, size1 = regs->bankRegs[1].size * esize1;

    TM_PRINTF("log0\n\r");
    for(size_t i = 0; i < size0; i ++){
        uint32_t data = log0[i];
        TM_PRINTF("%lud\t", data);
        if(i%esize0 == esize0-1){
            TM_PRINTF("\n\r");
        }
    }

    TM_PRINTF("log1\n\r");
    for(size_t i = 0; i < size1; i ++){
        uint32_t data = log1[i];
        TM_PRINTF("%lud\t", data);
        if(i%esize1 == esize1-1){
            TM_PRINTF("\n\r");
        }
    }

    return;
}