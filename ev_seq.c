#include "ev_seq.h"

#define  MAX_SEQ_EVENTS 32
static struct {
    uint32_t N;
    uint32_t events [MAX_SEQ_EVENTS];
    uint32_t delays [MAX_SEQ_EVENTS];
    uint32_t running;
} iternalSeqData = {0, {0}, {0}, 0};

void seqStart(){
    #ifdef DEBUG
    TM_PRINTF("seq start\n\r");
    #endif
    iternalSeqData.running = 1;
}

void seqStop(){
    #ifdef DEBUG
    TM_PRINTF("seq stop\n\r");
    #endif
    iternalSeqData.running = 0;
}

uint32_t seqRunning(){
    return iternalSeqData.running;
}

void seqSetEvents(uint32_t * newEvents, uint32_t * newDelays, uint32_t cnt){
    iternalSeqData.running = 0;
    iternalSeqData.N = cnt;
    for(uint32_t i = 0; i < iternalSeqData.N; i++){
        iternalSeqData.events[i] = newEvents[i];
        iternalSeqData.delays[i] = newDelays[i];
    }
}

uint32_t seqReadEvent(cyclicBuffer * buff){
    static uint32_t currentDelay = 0;
    static uint32_t currentPtr   = 0;
    
    if(seqRunning()){
        if(currentDelay >= iternalSeqData.delays[currentPtr]){
            uint32_t ev = iternalSeqData.events[currentPtr];
            cyclicBufferInsert(buff, ev);
            currentPtr ++;
            currentDelay = 0;
            if(currentPtr >= iternalSeqData.N){
                currentPtr = 0;
                seqStop();
            }
            return 1;
        }else{
            currentDelay ++;
        }
    }
    cyclicBufferInsert(buff, 0x100);
    return 0;
}

uint32_t seqReadEvents(cyclicBuffer * buff){
    uint32_t i = 0;
    while(seqReadEvent(buff)){
        i++;
    }
    if(i > 1){
        TM_PRINTF("CRIT ERROR: more thah one event in external trigger cycle\n\r");
    }
    return i;
}