#ifndef _EVENT_SEQ_H_
#define _EVENT_SEQ_H_

#include "cycle_buff.h"

#define EV_LOG_STOP  1
#define EV_INT_STOP  EV_LOG_STOP  + 1
#define EV_INT_ZERO  EV_INT_STOP  + 1
#define EV_INT_CAL   EV_INT_ZERO  + 1
#define EV_INT_START EV_INT_CAL   + 1
#define EV_LOG_START EV_INT_START + 1
#define EV_CNT       EV_LOG_START + 1

#define DL_LOG_STOP  1
#define DL_INT_STOP  1
#define DL_INT_ZERO  1
#define DL_INT_CAL   5000
#define DL_INT_START 1

void seqSetEvents(uint32_t * newEvents, uint32_t * newDelays, uint32_t cnt);
uint32_t seqReadEvents(cyclicBuffer * buff);

void seqStart();
uint32_t seqRunning();

#endif //_EVENT_SEQ_H_