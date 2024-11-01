#include "ext_trig.h"
#include "scr.h"
#include "logger.h"
#include "AFE.h"
#include "PSPL.h"
#include "ev_seq.h"

struct {
    uint32_t mode;
    uint32_t triggered;
    uint32_t calibration_in_process;
    uint32_t switch_to_ext;
    uint32_t switch_to_ev;
} iternalTrigData = {0, 0, 0};

uint32_t trigEvSource(){
    return iternalTrigData.mode == TRIG_EXTERNAL || iternalTrigData.calibration_in_process;
}

void trigSetEvSource(uint32_t mode){
    iternalTrigData.mode = mode;
}

uint32_t integralAnalog(){
    return controlMode() == MFM_MODE_ANALOG_TO_ANALOG || controlMode() == MFM_MODE_ANALOG_TO_DIGITAL;
}

void setRunSequence(){
    #ifdef DEBUG
    TM_PRINTF("run\n\r");
    #endif
    uint32_t ev[32] = {0}, 
             dl[32] = {0}, 
             cnt    = 0;
    if(logRunning()){
        ev[cnt] = EV_LOG_STOP;
        dl[cnt] = 0;
        cnt++;
    }
    if(AFERunning()){
        ev[cnt] = EV_INT_STOP;
        dl[cnt] = DL_LOG_STOP;
        cnt++;
    }
    ev[cnt] = EV_INT_ZERO;
    dl[cnt] = DL_INT_STOP;
    cnt++;
    if(integralAnalog() && controlExtTrigCycCal()){ // if need calibration
        ev[cnt] = EV_INT_CAL;
        dl[cnt] = DL_INT_ZERO;
        cnt++;
        ev[cnt] = EV_INT_START;
        dl[cnt] = DL_INT_CAL;
        cnt++;
    } else {
        ev[cnt] = EV_INT_START;
        dl[cnt] = DL_INT_ZERO;
        cnt++;
    }
    ev[cnt] = EV_LOG_START;
    dl[cnt] = DL_INT_START;
    cnt++;
    seqSetEvents(ev, dl, cnt);
}

void setSwitchSequence(uint32_t mode_from, uint32_t mode_to){
    #ifdef DEBUG
    TM_PRINTF("trig sw\n\r");
    #endif
    uint32_t ev[32] = {0}, 
             dl[32] = {0}, 
             cnt    = 0;
    ev[cnt] = 0; // delay for switch
    dl[cnt] = 0;
    cnt++;
    if(logRunning()){
        ev[cnt] = EV_LOG_STOP;
        dl[cnt] = 1;
        cnt++;
    }
    if(AFERunning()){
        ev[cnt] = EV_INT_STOP;
        dl[cnt] = DL_LOG_STOP;
        cnt++;
    }
    ev[cnt] = 0;  // delay for switch
    dl[cnt] = 1;
    cnt++;
    seqSetEvents(ev, dl, cnt);
}

void setCalibrationSequence(){
    #ifdef DEBUG
    TM_PRINTF("cal\n\r");
    #endif
    uint32_t ev[32] = {0}, 
             dl[32] = {0}, 
             cnt    = 0;
    if(logRunning()){
        ev[cnt] = EV_LOG_STOP;
        dl[cnt] = 0;
        cnt++;
    }
    if(AFERunning()){
        ev[cnt] = EV_INT_STOP;
        dl[cnt] = DL_LOG_STOP;
        cnt++;
    }
    ev[cnt] = EV_INT_CAL;
    dl[cnt] = DL_INT_STOP;
    cnt++;
    seqSetEvents(ev, dl, cnt);
}

int trigDDS_SYNC(void*){
    if(!seqRunning()){
        if(iternalTrigData.calibration_in_process){ // if calibration is ended
            statusExtTrigCal();
            iternalTrigData.calibration_in_process = 0;
        }
        if(iternalTrigData.triggered){ // if trigger sequence is ended
            iternalTrigData.triggered = 0;
        }

        if(iternalTrigData.switch_to_ev){ // if switch sequence is ended
            trigSetEvSource(TRIG_EVENT);
            iternalTrigData.switch_to_ev = 0;
        }

        uint32_t curr_source = trigEvSource();
        uint32_t next_source = controlExtTrig();
        if(next_source != curr_source){ // if source changes
            setSwitchSequence(curr_source, next_source);
            seqStart();
            if(next_source == TRIG_EVENT)
                iternalTrigData.switch_to_ev = 1;
            else if(next_source == TRIG_EXTERNAL){
                iternalTrigData.switch_to_ext = 1;
                trigSetEvSource(TRIG_EXTERNAL);
                iternalTrigData.switch_to_ext = 0;
            }

        } else if(curr_source == TRIG_EVENT && controlExtTrigCal()){ // if source dont changes and it is events
            setCalibrationSequence();
            seqStart();
            iternalTrigData.calibration_in_process = 1;
        } else if(curr_source == TRIG_EXTERNAL && (externalStart() || controlExtTrigSoft())){
            setRunSequence();
            seqStart();
            statusExtTrigSoft();
            iternalTrigData.triggered = 1;
        }
    }
    return 0;
}