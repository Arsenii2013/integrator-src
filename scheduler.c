#include "scheduler.h"

#include "main.h"


schedulerRecord * schedulerInfo = NULL;
size_t schedulerCnt = 0;

void schedulerSet(schedulerRecord * app){
    size_t i = 0;
    while(!(app[i].name[0] == '\0'        && app[i].DDS_SYNCCallback == NULL && 
            app[i].eventCallback == NULL  && app[i].appData == NULL)){
        i++;
    }
    schedulerCnt  = i;
    schedulerInfo = app;
}

void schedulerDDS_SYNC(){
    for(size_t i = 0; i < schedulerCnt; i ++){
        if(schedulerInfo[i].DDS_SYNCCallback != NULL) {
            int exitCode = schedulerInfo[i].DDS_SYNCCallback(schedulerInfo[i].appData);
            #ifdef DEBUG
            if(exitCode != 0){
                TM_PRINTF("%s DDS_SYNC callback exit with code %d\n\r", schedulerInfo[i].name, exitCode);
            }
        }
        #endif
    }
}

void schedulerEvent(uint32_t ev){
    for(size_t i = 0; i < schedulerCnt; i ++){
        if(schedulerInfo[i].eventCallback != NULL){
            int exitCode = schedulerInfo[i].eventCallback(ev, schedulerInfo[i].appData);
            #ifdef DEBUG
            if(exitCode != 0){
                TM_PRINTF("%s callback on event %d exit with code %d\n\r", schedulerInfo[i].name, ev, exitCode);
            }
            #endif
        }
    }
}