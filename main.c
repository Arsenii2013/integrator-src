#include "main.h"
#include "logger.h"
#include "scheduler.h"

#ifdef TEST
#include "test_gen.h"
#endif

#ifndef TEST
XGpioPs bank2;

#define EMIO_0_PIN      54
#define DDS_SYNC_PIN    EMIO_0_PIN
#define GP0_START       0x40000000

#define PS_MEM_OFFS     0x900
#define CR              0x1

#define EVENT_FIFO_OFFS 0xA00

#define SR              0x0
#define DATA            0x5
#endif

int DDS_SYNCPrint(void *){
    TM_PRINTF("DDS_SYNC!!!\n\r");
    return 0;
}
int eventPrint(uint32_t ev, void *){
    TM_PRINTF("Event %d!!!\n\r", ev);
    return 0;
}

int eventReturn(uint32_t ev, void *){
    TM_PRINTF("Return %d!!!\n\r", ev);
    return ev;
}
int DDS_SYNCReturn(void *){
    TM_PRINTF("DDS_SYNC -1!!!\n\r");
    return -1;
}

int eventAppData(uint32_t ev, void * appData){
    TM_PRINTF("AppData %d!!!\n\r", *(uint32_t*)appData);
    *(uint32_t*)appData = ev;
    return 0;
}
int DDS_SYNCAppData(void * appData){
    TM_PRINTF("AppData %d!!!\n\r", *(uint32_t*)appData);
    return 0;
}

int eventPrintLog(uint32_t ev, void *){
    if(ev == 0x20){
        printLog();
    }
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

#ifndef TEST
void init(){
    XGpioPs_Config *conf2 = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
    XGpioPs_CfgInitialize(&bank2, conf2, conf2->BaseAddr);

    XGpioPs_SetDirectionPin(&bank2, EMIO_0_PIN, 0x0);
    XGpioPs_SetDirection(&bank2, XGPIOPS_BANK2, 0xffffffff);
    XGpioPs_SetOutputEnable(&bank2, XGPIOPS_BANK2, 0xffffffff);

    XGpioPs_Write(&bank2, XGPIOPS_BANK2, 0x00FF0000);
}

uint32_t readDDS_SYNC(){
    return XGpioPs_ReadPin(&bank2, DDS_SYNC_PIN);
}
uint32_t readEvent(){
    if(!(*((volatile uint32_t *) GP0_START + EVENT_FIFO_OFFS + SR) & 0x2)){
        return *((volatile uint32_t *) GP0_START + EVENT_FIFO_OFFS + DATA);
    }
    return 0;
}
#endif

int main()
{

    uint32_t ev = 0;
    uint32_t sync = 0;
    
    #ifdef TEST
    uint32_t repeat = 0;

    uint32_t eventsN   = 5;
    uint32_t events [] = {0x12, 0x13, 0x17, 0x18, 0x20};
    uint64_t delays [] = {0,    115,  200,  376,  500};
    testGenInit(events, delays, eventsN, repeat);
    #else
    #ifdef DEBUG
    TM_PRINTF("start\n\r");
    #endif
    init_platform();
    init();
    #endif

    uint32_t lev = 0;
    loggerInit();

    schedulerRecord apps[] = {
        {.name="print", .DDS_SYNCCallback=DDS_SYNCPrint, .eventCallback=eventPrint, .appData=NULL}, 
        //{.name="return", .DDS_SYNCCallback=DDS_SYNCReturn, .eventCallback=eventReturn, .appData=NULL}, 
        //{.name="data", .DDS_SYNCCallback=DDS_SYNCAppData, .eventCallback=eventAppData, .appData=&lev}, 
        //{.name="logger", .DDS_SYNCCallback=loggerDDS_SYNC, .eventCallback=loggerEvent, .appData=NULL}, 
        //{.name="printLog", .DDS_SYNCCallback=NULL, .eventCallback=eventPrintLog, .appData=NULL}, 
        {.name="",     .DDS_SYNCCallback=NULL,          .eventCallback=NULL,       .appData=NULL}, 
    };

    schedulerSet(apps);

    #ifdef TEST
    PCIELoggerSetup();
    #endif

    while (ev != 0x20) {
        #ifdef TEST
        sync = testGenDDS_SYNC();
        ev = testGenEvent();
        #else
        sync = readDDS_SYNC();
        ev = readEvent();
        #endif

        if(sync){
            schedulerDDS_SYNC();
        }
        if(ev){
            schedulerEvent(ev);
        }
        logIntegrator entry = {.integralDigital=ev, .integralAnalog=ev, .B=ev};
        if(ev && logRunning())
            logg(*((logEntry*)&entry));
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

