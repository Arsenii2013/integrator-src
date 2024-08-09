#ifndef _PSPL_H
#define _PSPL_H_
#include "main.h"

#define FIFO_SIZE       2024

typedef struct 
{
    uint32_t data[FIFO_SIZE];
    uint32_t start;
    uint32_t size;
} cyclicBuffer;

uint32_t findLastDDS_SYNC(cyclicBuffer * buff);
void clearDDS_SYNC(cyclicBuffer * buff);

void initPStoPL();
void waitDDS_SYNC(uint32_t timeout);
uint32_t readEvent();
uint32_t readEvents(cyclicBuffer * buff);
void clearEvents();

#endif // _PSPL_H_