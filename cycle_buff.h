#ifndef _CYCLE_BUFF_H_
#define _CYCLE_BUFF_H_
#include "main.h"

#define FIFO_SIZE       32
typedef struct 
{
    uint32_t data[FIFO_SIZE];
    uint32_t start;
    uint32_t size;
} cyclicBuffer;

int cyclicBufferInsert(cyclicBuffer * buff, uint32_t data);
int cyclicBufferEmpty(cyclicBuffer * buff);
int cyclicBufferFull(cyclicBuffer * buff);
uint32_t cyclicBufferRead(cyclicBuffer * buff);
uint32_t cyclicBufferReadUntillLast(cyclicBuffer * c_buff, uint32_t buff [FIFO_SIZE], uint32_t word);

#endif// _CYCLE_BUFF_H_