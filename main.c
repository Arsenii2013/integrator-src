#include "main.h"
#include "PSPL.h"
#include "logger.h"
#include "scheduler.h"
//#include "sin_integral_emulator.h"
#include "AFE.h"
#include "scr.h"
#include "PSPL.h"
#include "AFE_emulator.h"
#include "ext_trig.h"
#include "ev_seq.h"

#ifdef TEST
#include "test_gen.h"
#endif

#ifndef TEST
#include "xil_cache.h"    
#endif

int DDS_SYNCPrint(void *){
    //TM_PRINTF("DDS_SYNC\n\r");
    return 0;
}
int eventPrint(uint32_t ev, void *){
    //TM_PRINTF("Event %d\n\r", ev);
    return 0;
}
int eventPrintNZero(uint32_t ev, void *){
    if(ev != 0)
        TM_PRINTF("Event %d\n\r", ev);
    return 0;
}

uint32_t appRunning = 0;

int eventApp(uint32_t ev, void*){
    if(ev == 0){
        return 0;
    }
    uint32_t trig_mode = trigEvSource();

    if(trig_mode == TRIG_EVENT){
        for(int i = 0; i < 4; i++){
            if(ev == controlStartEv(i)){
                appRunning = 1;
            }
            if(ev == controlStopEv(i)){
                appRunning = 0;
            }
        }
    } else if(trig_mode == TRIG_EXTERNAL){
        if(ev == EV_INT_START){
            appRunning = 1;
        }
        if(ev == EV_INT_STOP){
            appRunning = 0;
        }
    }
    return 0;
}

int DDS_SYNCApp(void*){
    if(appRunning){
        float B = MFMGetB();
        int64_t intergral = MFMGetIntegral();
        int32_t ADC = MFMGetADC();
        uint32_t DAC = MFMGetDAC();

        logIntegrator e = {.B = *(uint32_t *)&B, .ADC = ADC, .DAC = DAC, .integral_low = intergral, .integral_high = intergral >> 32};
        //TM_PRINTF("%x, %x, %x, %x\n\r", e.B, e.ADC, e.integral_low, e.integral_high);
        logg(*(logEntry *) &e);
    }
    return 0;
}

#ifndef TEST
int DDS_SYNCCacheInvalidate(void*){
    //Xil_DCacheFlush();
    Xil_DCacheInvalidateRange((intptr_t)loggerRegPtr(), sizeof(logRegs));
    Xil_DCacheInvalidateRange((intptr_t)SCRegPtr(), sizeof(statusControlRegisters));
    return 0;
}

int DDS_SYNCCacheFlush(void*){
    //Xil_DCacheFlush();
    Xil_DCacheFlushRange((intptr_t)loggerRegPtr(), sizeof(logRegs));
    Xil_DCacheFlushRange((intptr_t)SCRegPtr(), sizeof(statusControlRegisters));
    return 0;
}
#endif

#ifdef TEST
void PCIELoggerSetup(){
    logRegs * regLog = loggerRegPtr();
    regLog->CFG  = 0x6;
    regLog->DCM  = 0;
    regLog->START[0] = 0x12;
    regLog->STOP[0] = 0x18;

    statusControlRegisters * regSC = SCRegPtr(); 
    regSC->B0 = 10;
    regSC->START_EV = 0x13;
    regSC->STOP_EV = 0x17;
    regSC->K_ANALOG_TO_B = 1; 
    regSC->K_B_TO_ANALOG = 1;
    regSC->MODE = MFM_MODE_ANALOG_TO_ANALOG;
}
#endif

int main()
{
    cyclicBuffer ev_buff = {.size = 0, .start = 0, .data = {0}};
    uint32_t     ev_buff_flat [FIFO_SIZE];
    #ifdef TEST
    uint32_t repeat = 0;

    uint32_t eventsN   = 5;
    uint32_t events [] = {0x12, 0x13, 0x17, 0x18, 0x20};
    uint64_t cycles [] = {0,    0,    100,  100,  120};
    testGenInit(events, cycles, eventsN, repeat);
    AFEEmulinit();
    #else
    
    init_platform();
    initPStoPL();
    TM_PRINTF("start\n\r");
    #ifdef DEBUG
    volatile uint32_t tmp = readEvent();
    TM_PRINTF("%d\n\r", tmp);
    #endif
    #endif

    initSCR();
    loggerInit();
    #ifndef TEST
    //DDS_SYNCCacheFlush(NULL);
    #endif

    schedulerRecord apps[] = {
        #ifndef DEBUG
        {.name="print", .DDS_SYNCCallback=DDS_SYNCPrint, .eventCallback=eventPrintNZero, .appData=NULL}, 
        #endif
        #ifndef TEST
        //{.name="cacheInv", .DDS_SYNCCallback=DDS_SYNCCacheInvalidate, .eventCallback=NULL, .appData=NULL}, 
        #endif
        {.name="control", .DDS_SYNCCallback=controlDDS_SYNC, .eventCallback=NULL, .appData=NULL}, 
        //{.name="print", .DDS_SYNCCallback=NULL, .eventCallback=eventPrintNZero, .appData=NULL}, 
        
        {.name="logger", .DDS_SYNCCallback=loggerDDS_SYNC, .eventCallback=loggerEvent, .appData=NULL}, 
        //{.name="AFEEmul", .DDS_SYNCCallback=AFEEmulDDS_SYNC, .eventCallback=NULL, .appData=NULL}, 
        {.name="app", .DDS_SYNCCallback=DDS_SYNCApp, .eventCallback=eventApp, .appData=NULL}, 
        {.name="AFE", .DDS_SYNCCallback=AFEDDS_SYNC, .eventCallback=AFEEvent, .appData=NULL}, 
        {.name="ext", .DDS_SYNCCallback=trigDDS_SYNC, .eventCallback=NULL, .appData=NULL}, 
        #ifndef TEST
        //{.name="stop", .DDS_SYNCCallback=stopDDS_SYNC, .eventCallback=NULL, .appData=NULL}, 
        //{.name="cacheFlush", .DDS_SYNCCallback=DDS_SYNCCacheFlush, .eventCallback=NULL, .appData=NULL}, 
        #endif
        {.name="",     .DDS_SYNCCallback=NULL,          .eventCallback=NULL,       .appData=NULL}, 
    };

    schedulerSet(apps);

    #ifdef TEST
    PCIELoggerSetup();
    #endif
    #ifndef TEST
    clearEvents();
    #endif

    //DDS_SYNCCacheInvalidate(NULL);
    while (1) {
        #ifdef TEST
        testWaitDDS_SYNC();
        testReadEvents(&ev_buff);
        #else
        waitDDS_SYNC(0);
        
        uint32_t src = trigEvSource();
        if(src == TRIG_EVENT){
            readEvents(&ev_buff);
        } else if(src == TRIG_EXTERNAL) {
            seqReadEvents(&ev_buff);
            clearEvents();
        }
        #endif

        schedulerDDS_SYNC();

        uint32_t events_untill_sync = cyclicBufferReadUntillLast(&ev_buff, ev_buff_flat, 0x100);
        uint32_t sync_cnt = 0;
        //TM_PRINTF("%d\n\r", events_untill_sync);
        for(uint32_t i = 0; i < events_untill_sync; i++){
            schedulerEvent(ev_buff_flat[i] & 0xff);
            if(ev_buff_flat[i] & 0x100){
                sync_cnt++;
            }
        }   
        if(sync_cnt > 1){
            statusNotEnoughtTime();
        }

    }
    printLog();
    
    #ifndef TEST
    cleanup_platform();
    #endif
    return 0;
}
