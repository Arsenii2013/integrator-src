#include "cycle_buff.h"

int cyclicBufferInsert(cyclicBuffer * buff, uint32_t data){
    if(buff->size < FIFO_SIZE){
        buff->data[(buff->start + buff->size) % FIFO_SIZE] = data;
        buff->size++;
    }else{
        statusOverflowEvents();
    }
}