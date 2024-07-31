#ifdef TEST
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "main.h"
#include "test_gen.h"
#include "scheduler.h"
#include "logger.h"

int DDS_SYNCPrint(void *){
    printf("DDS_SYNC!!!\n\r");
    return 0;
}
int eventPrint(uint32_t ev, void *){
    printf("Event %d!!!\n\r", ev);
    return 0;
}


int eventReturn(uint32_t ev, void *){
    printf("Return %d!!!\n\r", ev);
    return ev;
}
int DDS_SYNCReturn(void *){
    printf("DDS_SYNC -1!!!\n\r");
    return -1;
}


int eventAppData(uint32_t ev, void * appData){
    printf("AppData %d!!!\n\r", *(uint32_t*)appData);
    *(uint32_t*)appData = ev;
    return 0;
}
int DDS_SYNCAppData(void * appData){
    printf("AppData %d!!!\n\r", *(uint32_t*)appData);
    return 0;
}

int eventPrintLog(uint32_t ev, void *){
    if(ev == 0x20){
        printLog();
    }
    return 0;
}

void PCIELoggerSetup(){
    #ifdef TEST
    logRegs * reg = loggerRegPtr();
    reg->CFG  = 0x7;
    reg->DCM  = 0;
    reg->START[0] = 0x12;
    reg->STOP[0] = 0x18;
    #endif
}

int main()
{
    uint32_t repeat = 0;


    uint32_t ev;
    uint32_t sync;
    uint32_t eventsN   = 5;
    uint32_t events [] = {0x12, 0x13, 0x17, 0x18, 0x20};
    uint64_t delays [] = {0,    115,  200,  376,  500};
    testGenInit(events, delays, eventsN, repeat);

    uint32_t lev = 0;
    loggerInit();

    schedulerRecord apps[] = {
        //{.name="print", .DDS_SYNCCallback=DDS_SYNCPrint, .eventCallback=eventPrint, .appData=NULL}, 
        //{.name="return", .DDS_SYNCCallback=DDS_SYNCReturn, .eventCallback=eventReturn, .appData=NULL}, 
        //{.name="data", .DDS_SYNCCallback=DDS_SYNCAppData, .eventCallback=eventAppData, .appData=&lev}, 
        {.name="logger", .DDS_SYNCCallback=loggerDDS_SYNC, .eventCallback=loggerEvent, .appData=NULL}, 
        {.name="printLog", .DDS_SYNCCallback=NULL, .eventCallback=eventPrintLog, .appData=NULL}, 
        {.name="",     .DDS_SYNCCallback=NULL,          .eventCallback=NULL,       .appData=NULL}, 
    };

    schedulerSet(apps);
    PCIELoggerSetup();

    while (ev != 0x20) {
        sync = testGenDDS_SYNC();
        ev = testGenEvent();
        if(sync){
            schedulerDDS_SYNC();
        }
        if(ev){
            schedulerEvent(ev);
        }
        logIntegrator entry = {.integralDigital=ev, .integralAnalog=ev, .B=ev};
        if(ev && logRunning())
            logg(*((logEntry*)&entry));
        //printf("0x%x\n", ev);
    }
    
    return 0;
}

#endif