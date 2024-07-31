#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "main.h"

typedef struct{
    char name[80];
    int (*DDS_SYNCCallback)(void *);
    int (*eventCallback)(uint32_t, void *);
    void * appData;
} schedulerRecord;

void schedulerSet(schedulerRecord * app);
void schedulerDDS_SYNC();
void schedulerEvent(uint32_t ev);

#endif // _SCHEDULER_H_