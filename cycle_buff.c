#include "cycle_buff.h"
#include "scr.h"

int cyclicBufferInsert(cyclicBuffer * buff, uint32_t data){
    if(buff->size < FIFO_SIZE){
        buff->data[(buff->start + buff->size) % FIFO_SIZE] = data;
        buff->size++;
    }else{
        statusOverflowEvents();
    }
    return 0;
}

int cyclicBufferEmpty(cyclicBuffer * buff){
    return buff->size == 0;
}

int cyclicBufferFull(cyclicBuffer * buff){
    return buff->size == FIFO_SIZE;
}

uint32_t cyclicBufferRead(cyclicBuffer * buff){
    if(cyclicBufferEmpty(buff))
        return -1;
    uint32_t ret = buff->data[buff->start]; 
    buff->start = (buff->start + 1) % FIFO_SIZE;
    buff->size --;
    return ret;
}


uint32_t cyclicBufferFindLast(cyclicBuffer * buff, uint32_t word){
    uint32_t search_ptr = buff->size;
    while(search_ptr != 0 && (((buff->data[(buff->start + search_ptr-1) % FIFO_SIZE]) & word) == 0)){
        search_ptr --;
    }
    return search_ptr-1;


}

uint32_t cyclicBufferReadUntillLast(cyclicBuffer * c_buff, uint32_t buff [FIFO_SIZE], uint32_t word){
    uint32_t last = cyclicBufferFindLast(c_buff, word);
    for(uint32_t i = 0; i < last+1; i ++){
        buff[i] = cyclicBufferRead(c_buff);
    }
    return last+1;
}