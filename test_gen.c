#include "PSPL.h"
#include "test_gen.h"

#ifdef TEST
#include <time.h>

static uint64_t cp;
static uint64_t lostDDS_SYNC;
static uint32_t repeat;

#define  MAX_EVENTS 10
static uint32_t eventsN   = 1;
static uint32_t events [MAX_EVENTS] = {0x12};
static uint64_t cycles [MAX_EVENTS] = {0};

uint64_t micros()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    uint64_t us = SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
    return us;
}

void testGenInit(uint32_t * newEvents, uint64_t * newCycles, uint32_t cnt, uint32_t newRepeat){
    eventsN = cnt;
    for(uint32_t i = 0; i < eventsN; i++){
        events[i] = newEvents[i];
        cycles[i] = newCycles[i];
    }
    repeat = newRepeat;
    cp = micros();
    lostDDS_SYNC = 0;

    int cycle = 0;
    for(int i = 0; i < cnt; i ++){
        if(newCycles[i] < cycle){
            TM_PRINTF("ERROR: testGenEvents events must go in non-decreasing order\n");
        }
        cycle = newCycles[i];
    }
}

void testWaitDDS_SYNC(){
    uint64_t cc = micros();
    uint64_t betveenDDS_SYNC = cc - cp;    
    while(betveenDDS_SYNC == 0 || betveenDDS_SYNC % DDS_SYNC_PRD_US != 0){ 
        cc = micros();
        betveenDDS_SYNC = cc - cp;
    }
    lostDDS_SYNC = betveenDDS_SYNC / DDS_SYNC_PRD_US;
    lostDDS_SYNC = 1;
    cp = cc;
}


uint32_t testReadEvents(cyclicBuffer * buff){
    int prd = cycles[eventsN-1] + 1;

    static uint32_t cycle = 0;
    uint32_t l = lostDDS_SYNC;
    for(; lostDDS_SYNC; lostDDS_SYNC --){
        for(uint32_t i = 0; i < eventsN; i ++){
            if(cycles[i] == cycle){
                buff->data[(buff->start + buff->size) % FIFO_SIZE] = events[i];
                buff->size++;
            }
        }
        buff->data[(buff->start + buff->size) % FIFO_SIZE] = 0x100;
        buff->size++;
        cycle = (cycle + 1) % prd;
    }

    return 0;
}
#endif