#ifndef _TEST_GEN_
#define _TEST_GEN_
#include "main.h"

#ifdef TEST
#define SEC_TO_US(sec)  ((sec)*1000000)
#define NS_TO_US(ns)    ((ns)/1000)
#define DDS_SYNC_PRD_US 20000

void testGenInit(uint32_t * newEvents, uint64_t * newCycles, uint32_t cnt, uint32_t newRepeat);

void testWaitDDS_SYNC();
uint32_t testReadEvents(cyclicBuffer * buff);
#endif
#endif // _TEST_GEN_