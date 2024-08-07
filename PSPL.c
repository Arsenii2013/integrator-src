#include "PSPL.h"
#include "scr.h"

#define EMIO_0_PIN      54
#define DDS_SYNC_PIN    EMIO_0_PIN
#define BUSY_PIN        EMIO_0_PIN + 1
#define GP0_START       0x40000000

#define CR              0x1

#define EVENT_FIFO_OFFS 0x2400

#define SR              0x0
#define DATA            0x14

#ifndef TEST
XGpioPs bank2;
#endif

void initPStoPL(){
    #ifndef TEST
    XGpioPs_Config *conf2 = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
    XGpioPs_CfgInitialize(&bank2, conf2, conf2->BaseAddr);

    XGpioPs_SetDirectionPin(&bank2, EMIO_0_PIN, 0x0);
    XGpioPs_SetDirection(&bank2, XGPIOPS_BANK2, 0xffffffff);
    XGpioPs_SetOutputEnable(&bank2, XGPIOPS_BANK2, 0xffffffff);

    XGpioPs_Write(&bank2, XGPIOPS_BANK2, 0x00FF0000);
    #endif
}

void waitDDS_SYNC(uint32_t timeout){
    #ifndef TEST
    XGpioPs_WritePin(&bank2, BUSY_PIN, 0);
    if(timeout == 0){
        while(!XGpioPs_ReadPin(&bank2, DDS_SYNC_PIN)) {};
    }else{
        for(;timeout && !XGpioPs_ReadPin(&bank2, DDS_SYNC_PIN); timeout--) {};
    }
    XGpioPs_WritePin(&bank2, BUSY_PIN, 1);
    #endif
}

uint32_t readEvent(){
    if(!(*((volatile uint32_t *) (GP0_START + EVENT_FIFO_OFFS + SR)) & 0x1)){
        return *((volatile uint32_t *) (GP0_START + EVENT_FIFO_OFFS + DATA));
    }
    return 0;
}

uint32_t readEvents(cyclicBuffer * buff){
    uint32_t i = 0;
    while(!(*((volatile uint32_t *) (GP0_START + EVENT_FIFO_OFFS + SR)) & 0x1)){
        if(buff->size < FIFO_SIZE){
            buff->data[(buff->start + buff->size) % FIFO_SIZE] = *((volatile uint32_t *) (GP0_START + EVENT_FIFO_OFFS + DATA));
            buff->size++;
        }else{
            statusOverflowEvents();
            volatile uint32_t rd = *((volatile uint32_t *) (GP0_START + EVENT_FIFO_OFFS + DATA));
        }
        i++;
    }
    return i;
}

uint32_t findLastDDS_SYNC(cyclicBuffer * buff){
    uint32_t last = buff->start;
    uint32_t cnt = 0;
    for(uint32_t i = 0; i < buff->size; i++){
        if(buff->data[(buff->start + i) % FIFO_SIZE] & 0x100){
            last = buff->start + i;
            cnt++;
        }
    }
    if(cnt != 1){
        statusNotEnoughtTime();
    }
    #ifdef DEBUG
    if(cnt == 0){
        TM_PRINTF("DEBUG: zero DDS_SYNC events");
    }
    #endif
    return last % FIFO_SIZE;
}

void clearDDS_SYNC(cyclicBuffer * buff){
    for(uint32_t i = 0; i < buff->size; i++){
        buff->data[(buff->start + i) % FIFO_SIZE] &= 0xFF;
    }
}