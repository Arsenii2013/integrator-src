#ifndef _PSPL_H
#define _PSPL_H_
#include "main.h"
#include "cycle_buff.h"

uint32_t findLastDDS_SYNC(cyclicBuffer * buff);
void clearDDS_SYNC(cyclicBuffer * buff);

void initPStoPL();
void waitDDS_SYNC(uint32_t timeout);
uint32_t readEvent();
uint32_t readEvents(cyclicBuffer * buff);
void clearEvents();
int AFEPwrOn();
int AFEPwrOff();

uint32_t externalStart();

#endif // _PSPL_H_