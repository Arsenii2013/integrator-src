#include "main.h"
#include "PSPL.h"
#include "logger.h"
#include "scheduler.h"
//#include "sin_integral_emulator.h"
#include "AFE.h"
#include "AFE_emulator.h"

#ifdef TEST
#include "test_gen.h"
#endif

int DDS_SYNCPrint(void *){
    TM_PRINTF("DDS_SYNC\n\r");
    return 0;
}
int eventPrint(uint32_t ev, void *){
    TM_PRINTF("Event %d\n\r", ev);
    return 0;
}

uint32_t appRunning = 0;

int eventApp(uint32_t ev, void*){
    if(ev == 0){
        return 0;
    }
    if(ev == controlStartEv()){
        appRunning = 1;
    }
    if(ev == controlStopEv()){
        appRunning = 0;
    }
    return 0;
}

int DDS_SYNCApp(void*){
    if(appRunning){
        uint64_t B = MFMGetB();
        uint64_t intergral = MFMGetIntegral();
        logIntegrator e = {.B_low = B, .B_high = B >> 32, .integralAnalog_low = intergral, .integralAnalog_high = intergral >> 32};
        TM_PRINTF("%x, %x, %x, %x\n\r", e.B_low, e.B_high, e.integralAnalog_low, e.integralAnalog_high);
        logg(*(logEntry *) &e);
    }
    return 0;
}

#ifdef TEST
void PCIELoggerSetup(){
    logRegs * regLog = loggerRegPtr();
    regLog->CFG  = 0x3c;
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
    #ifdef TEST
    uint32_t repeat = 0;

    uint32_t eventsN   = 5;
    uint32_t events [] = {0x12, 0x13, 0x17, 0x18, 0x20};
    uint64_t cycles [] = {0,    0,    100,  100,  120};
    testGenInit(events, cycles, eventsN, repeat);
    initSCR();
    AFEEmulinit();
    #else
    init_platform();
    initPStoPL();
    clearEvents();

    #ifdef DEBUG
    TM_PRINTF("start\n\r");

    volatile uint32_t tmp = readEvent();
    TM_PRINTF("%d\n\r", tmp);
    #endif
    #endif

    uint32_t lev = 0;
    loggerInit();

    schedulerRecord apps[] = {
        {.name="print", .DDS_SYNCCallback=DDS_SYNCPrint, .eventCallback=eventPrint, .appData=NULL}, 
        //{.name="return", .DDS_SYNCCallback=DDS_SYNCReturn, .eventCallback=eventReturn, .appData=NULL}, 
        //{.name="data", .DDS_SYNCCallback=DDS_SYNCAppData, .eventCallback=eventAppData, .appData=&lev}, 
        {.name="logger", .DDS_SYNCCallback=loggerDDS_SYNC, .eventCallback=loggerEvent, .appData=NULL}, 
        {.name="AFEEmul", .DDS_SYNCCallback=AFEEmulDDS_SYNC, .eventCallback=NULL, .appData=NULL}, 
        {.name="log", .DDS_SYNCCallback=DDS_SYNCApp, .eventCallback=eventApp, .appData=NULL}, 
        {.name="AFE", .DDS_SYNCCallback=AFEDDS_SYNC, .eventCallback=AFEEvent, .appData=NULL}, 
        //{.name="printLog", .DDS_SYNCCallback=NULL, .eventCallback=eventPrintLog, .appData=NULL}, 
        {.name="",     .DDS_SYNCCallback=NULL,          .eventCallback=NULL,       .appData=NULL}, 
    };

    schedulerSet(apps);

    #ifdef TEST
    PCIELoggerSetup();
    #endif
    uint32_t flag = 1;
    while (flag) {
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
            if(ev_buff.data[ev_buff.start % FIFO_SIZE] == 0x20){
                flag = 0;
            }
            ev_buff.start ++;
            ev_buff.size --;
        }
        ev_buff.start = ev_buff.start % FIFO_SIZE;

        //logIntegrator entry = {.integralDigital=ev, .integralAnalog=ev, .B=ev};
        //if(ev && logRunning())
           //logg(*((logEntry*)&entry));
        //TM_PRINTF("0x%x\n", ev);
    }
    printLog();
    
    #ifndef TEST
    cleanup_platform();

    #ifdef DEBUG
    TM_PRINTF("start\n\r");
    #endif
    #endif
    return 0;
}

