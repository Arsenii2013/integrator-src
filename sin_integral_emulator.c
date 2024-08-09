#include "sin_integral_emulator.h"
#include "scr.h"
#include "logger.h"
#include "math.h"

uint32_t emulRunning = 0;
uint64_t timestamp = 0;
uint32_t calibration = 0;

int emulatorDDS_SYNC(void*){
    static uint32_t cal_cnt = 0;
    if(!emulRunning){
        return 0;
    }
    if(calibration){
        if(cal_cnt > 3){
            cal_cnt = 0;
            calibration = 0;
        } else {
            cal_cnt++;
        } 
    }

    float arg = (float)timestamp / 100.* 2 * M_PI;
    float sinus = sin(arg);
    float cosinus = cos(arg); 

    logIntegrator l = {.B=timestamp, .integralDigital=(*(uint32_t *) &sinus), .integralAnalog=(*(uint32_t *) &cosinus)};

    logg(*(logEntry *)&l);
    timestamp++;
    return 0;
}


int emulatorEvent(uint32_t ev, void *){
    if(ev == controlSTART()){
        emulRunning = 1;
    }
    if(ev == controlSTOP()){
        emulRunning = 0;
    }
    if(ev == controlRESET()){
        timestamp = 0;
    }
    if(ev == controlB0()){
        timestamp = controlGetB0();
    }
    if(ev == controlCAL()){
        calibration = 1;
    }
    return 0;
}