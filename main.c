#include "main.h"
#include "PSPL.h"
#include "logger.h"
#include "scheduler.h"
#include "sin_integral_emulator.h"
#include "AFE.h"

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

#ifdef TEST
void PCIELoggerSetup(){
    logRegs * reg = loggerRegPtr();
    reg->CFG  = 0x7;
    reg->DCM  = 0;
    reg->START[0] = 0x12;
    reg->STOP[0] = 0x18;
}
#endif

int main()
{
    cyclicBuffer ev_buff = {.size = 0, .start = 0, .data = {0}};
    #ifdef TEST
    uint32_t repeat = 0;

    uint32_t eventsN   = 5;
    uint32_t events [] = {0x12, 0x13, 0x17, 0x18, 0x20};
    uint64_t cycles [] = {0,    0,    1,    1,    2};
    testGenInit(events, cycles, eventsN, repeat);
    initSCR();
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
        {.name="graph", .DDS_SYNCCallback=emulatorDDS_SYNC, .eventCallback=emulatorEvent, .appData=NULL}, 
        //{.name="printLog", .DDS_SYNCCallback=NULL, .eventCallback=eventPrintLog, .appData=NULL}, 
        {.name="",     .DDS_SYNCCallback=NULL,          .eventCallback=NULL,       .appData=NULL}, 
    };

    schedulerSet(apps);

    #ifdef TEST
    PCIELoggerSetup();
    #endif

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

        //logIntegrator entry = {.integralDigital=ev, .integralAnalog=ev, .B=ev};
        //if(ev && logRunning())
           //logg(*((logEntry*)&entry));
        //TM_PRINTF("0x%x\n", ev);
    }
    
    #ifndef TEST
    cleanup_platform();

    #ifdef DEBUG
    TM_PRINTF("start\n\r");
    #endif
    #endif
    return 0;
}

