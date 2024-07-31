#ifndef _TEST_GEN_
#define _TEST_GEN_
#ifdef TEST

#include "main.h"

#define SEC_TO_US(sec) ((sec)*1000000)
#define NS_TO_US(ns)    ((ns)/1000)

void testGenInit(uint32_t * newEvents, uint64_t * newDelays, uint32_t cnt, uint32_t newRepeat);

uint32_t testGenDDS_SYNC();
uint32_t testGenEvent();
#endif
#endif // _TEST_GEN_