#include "main.h"
#include "PSPL.h"
#include "logger.h"
#include "scheduler.h"
//#include "sin_integral_emulator.h"
#include "AFE.h"
#include "scr.h"
#include "AFE_emulator.h"

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
    for(int i = 0; i < 4; i++){
        if(ev == controlStartEv(i)){
            appRunning = 1;
        }
        if(ev == controlStopEv(i)){
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

        logIntegrator e = {.B = *(uint32_t *)&B, .ADC = ADC, .integral_low = intergral, .integral_high = intergral >> 32};
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

int rstDDS_SYNC(void*){
    static uint32_t prev = 1;
    uint32_t atm = controlAFEPwr();
    if(prev != atm){
        if(atm){
            TM_PRINTF("AFE off\n\r");
            AFEPwrOff();
        }else{
            TM_PRINTF("AFE on\n\r");
            AFEPwrOn();
        }
        prev = atm;
    }
    return 0;
}

int main()
{
    cyclicBuffer ev_buff = {.size = 0, .start = 0, .data = {0}};
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
    int afeRes = AFEPwrOn();
    AFEInit();
    TM_PRINTF("AFE pwr good: %d\n\r", afeRes);
    #ifdef DEBUG
    volatile uint32_t tmp = readEvent();
    TM_PRINTF("%d\n\r", tmp);
    #endif
    #endif

    initSCR();
    loggerInit();
    #ifndef TEST
    DDS_SYNCCacheFlush(NULL);
    #endif

    schedulerRecord apps[] = {
        #ifdef DEBUG
        {.name="print", .DDS_SYNCCallback=DDS_SYNCPrint, .eventCallback=eventPrint, .appData=NULL}, 
        #endif
        #ifndef TEST
        {.name="cacheInv", .DDS_SYNCCallback=DDS_SYNCCacheInvalidate, .eventCallback=NULL, .appData=NULL}, 
        #endif
        {.name="control", .DDS_SYNCCallback=controlDDS_SYNC, .eventCallback=NULL, .appData=NULL}, 
        //{.name="print", .DDS_SYNCCallback=NULL, .eventCallback=eventPrintNZero, .appData=NULL}, 
        
        //{.name="rst", .DDS_SYNCCallback=rstDDS_SYNC, .eventCallback=NULL, .appData=NULL}, 
        {.name="logger", .DDS_SYNCCallback=loggerDDS_SYNC, .eventCallback=loggerEvent, .appData=NULL}, 
        //{.name="AFEEmul", .DDS_SYNCCallback=AFEEmulDDS_SYNC, .eventCallback=NULL, .appData=NULL}, 
        {.name="app", .DDS_SYNCCallback=DDS_SYNCApp, .eventCallback=eventApp, .appData=NULL}, 
        {.name="AFE", .DDS_SYNCCallback=AFEDDS_SYNC, .eventCallback=AFEEvent, .appData=NULL}, 
        #ifndef TEST
        //{.name="stop", .DDS_SYNCCallback=stopDDS_SYNC, .eventCallback=NULL, .appData=NULL}, 
        {.name="cacheFlush", .DDS_SYNCCallback=DDS_SYNCCacheFlush, .eventCallback=NULL, .appData=NULL}, 
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
        readEvents(&ev_buff);
        #endif

        uint32_t lastDDS_SYNC = findLastDDS_SYNC(&ev_buff);
        clearDDS_SYNC(&ev_buff);

        schedulerDDS_SYNC(); // применить установки с прошлого цикла
        
        while(ev_buff.start <= lastDDS_SYNC){
            if(ev_buff.size == 0)
                break;
            schedulerEvent(ev_buff.data[ev_buff.start % FIFO_SIZE]);
            ev_buff.start ++;
            ev_buff.size --;
        }
        ev_buff.start = ev_buff.start % FIFO_SIZE;
    }
    printLog();
    
    #ifndef TEST
    cleanup_platform();
    #endif
    return 0;
}
