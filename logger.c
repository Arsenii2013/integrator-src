#include "logger.h"
#include "scr.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef TEST
#include "xil_cache.h"
#endif

//logRegs * REGS_BASE_LOG = 0x40000000 + 0x1C00;
volatile logRegs * REGS_BASE_LOG = (logRegs *)0x3A000000;

uint8_t * bankAddrs [BANK_NUM] = {(uint8_t *)0x00000000, (uint8_t *)0x10000000};
uint32_t bankCnt [BANK_NUM] = {0, 0};

uint32_t startLog = 0;
uint32_t stopLog = 0;
uint32_t switchLog = 0;

uint32_t logEntrySize(uint32_t desc){
    uint32_t count = 0;
    for (; desc; count++)
        desc &= (desc - 1);
    return count;
}

volatile logRegs * loggerRegPtr(){
    return REGS_BASE_LOG;
}

void saveLog(){
    
}

uint32_t logRunning(){
    logRegs* regs = (logRegs*) REGS_BASE_LOG;
    return !(regs->SR & (1 << SR_IDLE));
}

void loggerInit(){
    #ifdef TEST
    bankAddrs[0] = malloc(sizeof(logEntry) * BANK_MAX_SIZE);
    bankAddrs[1] = malloc(sizeof(logEntry) * BANK_MAX_SIZE);
    REGS_BASE_LOG    = malloc(sizeof(logRegs));
    #endif

    const logRegs defaults = {.SR = 1 << SR_IDLE, .CR = 0, .CR_S = 0, .CR_C = 0, .CFG = 7, .DCM = 0, .START = {15, 0}, .STOP = {0, 0},
         .bankRegs[0] = {.cfg = 0, .dcm = 0, .size = 0}, .bankRegs[1] = {.cfg = 0, .dcm = 0, .size = 0},
    };

    *REGS_BASE_LOG = defaults;
    Xil_DCacheFlushRange(loggerRegPtr(), sizeof(logRegs));
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
    Xil_DCacheFlushRange(addr, writed*4);
    return writed;
}

uint32_t integratorCfgConvert(uint32_t cfg){
    uint32_t bits = 0b0001; // показывает битность каждого параметра
    uint32_t res = 0;
    for(int i = 0, j = 0; (i < 32) && (j < 32); i++){
        if(cfg & 1 << i) {
            res |= 1 << j;
        }
        j ++;
        if(bits & 1 << i){ // 64
            if(cfg & 1 << i) {
                res |= 1 << j;
            }
            j ++;
        }
    }
    return res;
}

void logg(logEntry e){
    logRegs* regs = (logRegs*) REGS_BASE_LOG;
    uint8_t activeBank = regs->SR & (1 << SR_BANK) ? 1 : 0;
    
    if(!logRunning()){
        #ifdef DEBUG
        TM_PRINTF("ERROR: try write stopped log\n");
        #endif
        return;
    }
    if(bankCnt[activeBank] == 0){
        if(regs->bankRegs[activeBank].size == BANK_MAX_SIZE){
            statusLogOverflow();
            return;
        }
        e.desc = integratorCfgConvert(regs->bankRegs[activeBank].cfg);
        writeEntry(e, bankAddrs[activeBank] + regs->bankRegs[activeBank].size * logEntrySize(e.desc) * 4);
        regs->bankRegs[activeBank].size ++;
        bankCnt[activeBank] = regs->bankRegs[activeBank].dcm;
    } else {
        bankCnt[activeBank] --;
    }
}

int loggerDDS_SYNC(void*){
    logRegs* regs = (logRegs*) REGS_BASE_LOG;

    regs->CR |= regs->CR_S;
    regs->CR &= ~regs->CR_C;
    regs->CR_S = 0;
    regs->CR_C = 0;

    uint32_t CRStart  = (regs->CR & (1 << CR_START )) | startLog;
    uint32_t CRStop   = (regs->CR & (1 << CR_STOP  )) | stopLog;
    uint32_t CRSwitch = (regs->CR & (1 << CR_SWITCH)) | switchLog;

    uint32_t running = !(regs->SR & (1 << SR_IDLE)); 

    if(CRStart && CRStop){
        statusLogStartStop();
        return -1;
    }

    if(running){
        if(CRStop){
            TM_PRINTF("CRStop\n\r");
            regs->SR |= (1 << SR_IDLE);
            if(CRSwitch){
                regs->SR   ^= (1 << SR_BANK);
            }
        }
        if(CRStart){
            statusLogStartStart();
        }
    }else{
        if(CRStart){
            TM_PRINTF("CRStart\n\r");
            regs->SR &= ~(1 << SR_IDLE);
            uint8_t activeBank = regs->SR & (1 << SR_BANK) ? 1 : 0;
            regs->bankRegs[activeBank].dcm = regs->DCM;
            regs->bankRegs[activeBank].cfg = regs->CFG;
            regs->bankRegs[activeBank].size= 0;
        }
        if(CRStop){
            statusLogStopStop();
        }
    }


    regs->CR   &= ~((1 << CR_START) | (1 << CR_STOP) | (1 << CR_SWITCH));
    startLog = 0;
    stopLog  = 0;
    switchLog= 0; 
    return 0;
}

int loggerEvent(uint32_t ev, void*){
    logRegs* regs = (logRegs*) REGS_BASE_LOG;
    if(ev == 0){
        return 0;
    }
    uint32_t running = !(regs->SR & (1 << SR_IDLE)); 

    if(running){
        for(int i = 0; i < BANK_NUM; i ++){
            if(ev == regs->STOP[i]){
                //regs->CR |= 1 << CR_STOP;
                stopLog = 1; 
                if((regs->CR >> CR_MODE) & 0b01){
                    //regs->CR |= 1 << CR_SWITCH;
                    switchLog = 1;
                }
            }
        }
    }else{
        for(int i = 0; i < BANK_NUM; i ++){
            if(ev == regs->START[i]){
                //regs->CR |= 1 << CR_START;
                startLog = 1;
            }
        }
    }
    return 0;
}


void printLog(){
    logRegs* regs = (logRegs*) REGS_BASE_LOG;
    uint32_t * log0 = bankAddrs[0], * log1 = bankAddrs[1];
    uint32_t desc0 = integratorCfgConvert(regs->bankRegs[0].cfg), desc1 = integratorCfgConvert(regs->bankRegs[1].cfg);
    uint32_t esize0 = logEntrySize(desc0), esize1 = logEntrySize(desc1);
    size_t size0 = regs->bankRegs[0].size * esize0, size1 = regs->bankRegs[1].size * esize1;

    TM_PRINTF("log0\n\r");
    for(size_t i = 0; i < size0; i ++){
        if(i%2 == 0){
            uint32_t data = log0[i];
            TM_PRINTF("%lu\t", data);
        }else {
            int32_t data = ((int32_t *)log0)[i];
            TM_PRINTF("%d\t", data);
        }
        if(i%esize0 == esize0-1){
            TM_PRINTF("\n\r");
        }
    }

    TM_PRINTF("log0x64\n\r");
    for(size_t i = 0; i < size0 / 2; i ++){
        int64_t data = ((int64_t *)log0)[i];
        TM_PRINTF("%lld\t", data);
        if(i%(esize0/2) == (esize0/2)-1){
            TM_PRINTF("\n\r");
        }
    }

    TM_PRINTF("log1\n\r");
    for(size_t i = 0; i < size1; i ++){
        uint32_t data = log1[i];
        TM_PRINTF("%lu\t", data);
        if(i%esize1 == esize1-1){
            TM_PRINTF("\n\r");
        }
    }

    return;
}