#include "scheduler.h"
#include "main.h"
#ifdef PROFILING
#include "xtime_l.h"
#endif

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
    #ifdef PROFILING
    uint64_t time_eval;
    XTime before_test;
    XTime *p_before_test = &before_test;
    XTime after_test;
    XTime *p_after_test = &after_test;
    TM_PRINTF("DDS_SYNC time: ");
    #endif
    for(size_t i = 0; i < schedulerCnt; i ++){
        if(schedulerInfo[i].DDS_SYNCCallback != NULL) {
            #ifdef PROFILING
            XTime_GetTime(p_before_test);
            #endif

            int exitCode = schedulerInfo[i].DDS_SYNCCallback(schedulerInfo[i].appData);

            #ifdef PROFILING
            XTime_GetTime(p_after_test);
            time_eval = (u64) after_test - (u64) before_test;
            TM_PRINTF("%s:%lu ", schedulerInfo[i].name, time_eval);
            #endif

            #ifdef DEBUG
            if(exitCode != 0){
                TM_PRINTF("%s DDS_SYNC callback exit with code %d\n\r", schedulerInfo[i].name, exitCode);
            }
            #else
            (void)exitCode;
            #endif
        }
    }
    #ifdef PROFILING
    TM_PRINTF("\n\r");
    #endif
}

void schedulerEvent(uint32_t ev){
    #ifdef PROFILING
    uint32_t time_eval;
    XTime before_test;
    XTime *p_before_test = &before_test;
    XTime after_test;
    XTime *p_after_test = &after_test;
    TM_PRINTF("EVENT 0x%x time: ", ev);
    #endif
    for(size_t i = 0; i < schedulerCnt; i ++){
        #ifdef PROFILING
        XTime_GetTime(p_before_test);
        #endif

        if(schedulerInfo[i].eventCallback != NULL){
            int exitCode = schedulerInfo[i].eventCallback(ev, schedulerInfo[i].appData);
            #ifdef DEBUG
            if(exitCode != 0){
                TM_PRINTF("%s callback on event %d exit with code %d\n\r", schedulerInfo[i].name, ev, exitCode);
            }
            #else
            (void)exitCode;
            #endif
        }

        #ifdef PROFILING
        XTime_GetTime(p_after_test);
        time_eval = (u64) after_test - (u64) before_test;
        TM_PRINTF("%s:%lu ", schedulerInfo[i].name, time_eval);
        #endif
    }
    #ifdef PROFILING
    TM_PRINTF("\n\r");
    #endif
}