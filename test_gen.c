
#ifdef TEST
#include "test_gen.h"
#include <time.h>

uint64_t c0;
uint32_t repeat;

#define  MAX_EVENTS 10
uint32_t eventsN   = 4;
uint32_t events [MAX_EVENTS] = {0x12, 0x13, 0x17, 0x18};
uint64_t delays [MAX_EVENTS] = {0,    115,  200,   376};

uint64_t micros()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    uint64_t us = SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
    return us;
}

void testGenInit(uint32_t * newEvents, uint64_t * newDelays, uint32_t cnt, uint32_t newRepeat){
    eventsN = cnt;
    for(uint32_t i = 0; i < eventsN; i++){
        events[i] = newEvents[i];
        delays[i] = newDelays[i];
    }
    repeat = newRepeat;
    c0 = micros();
}

uint32_t testGenDDS_SYNC(){
    uint64_t c1 = micros();
    return (c1-c0) % 20 == 0;
}


uint32_t testGenEvent(){
    const uint64_t prd = 1000; // in mks

    static int activs [2][MAX_EVENTS];
    static int activsNotRepeat [MAX_EVENTS];

    uint64_t c1  = micros();
    uint64_t div = ((c1 - c0) / prd) % 2;
    uint64_t dt  = (c1 - c0) % prd;

    if(repeat){
        for(int i = 0; i < eventsN; i ++){
            if(dt > delays[i] && !activs[div][i]){
                activs[(div+1) % 2][i] = 0;
                activs[div][i] = 1;
                return events[i];
            }
        }
    } else {
        for(int i = 0; i < eventsN; i ++){
            if(dt > delays[i] && !activsNotRepeat[i]){
                activsNotRepeat[i] = 1;
                return events[i];
            }
        }
    }
    return 0;
}
#endif