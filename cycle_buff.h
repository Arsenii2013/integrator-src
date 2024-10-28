#ifndef _CYCLE_BUFF_H_
#define _CYCLE_BUFF_H_
#include "main.h"

#define FIFO_SIZE       2024
typedef struct 
{
    uint32_t data[FIFO_SIZE];
    uint32_t start;
    uint32_t size;
} cyclicBuffer;

int cyclicBufferInsert(cyclicBuffer * buff, uint32_t data);

#endif// _CYCLE_BUFF_H_