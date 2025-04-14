#ifndef _EXTERNAL_TRIG_H_
#define _EXTERNAL_TRIG_H_
#include "main.h"

#define TRIG_EVENT    0
#define TRIG_EXTERNAL 1

uint32_t trigEvSource();
int trigDDS_SYNC(void*);

#endif //_EXTERNAL_START_H_