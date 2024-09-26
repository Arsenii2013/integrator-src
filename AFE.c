#include "AFE.h"

volatile AFERegs * REGS_BASE_AFE = (AFERegs *)0x40001800;
//volatile AFERegs * REGS_BASE_AFE = (AFERegs *)0x10000000;
uint32_t reset = 0;
uint32_t calibration_flag = 0;
uint32_t calibration_cnt  = 0;
uint32_t koeffAB = 0;
uint32_t koeffDB = 0;
uint32_t mode = MFM_MODE_ANALOG_TO_ANALOG;

volatile AFERegs * AFERegPtr(){
    return  REGS_BASE_AFE;
}

void MFMStartIntegral(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->MFM.ctrl_reg |= 1 << MFM_CTRL_OPERATION;

}

void MFMStopIntegral(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->MFM.ctrl_reg &= ~(1 << MFM_CTRL_OPERATION);
}

void MFMResetIntegral(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->MFM.ctrl_reg |= 1 << MFM_CTRL_ZERO;
    reset = 1;
}

void MFMStartCalibration(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->MFM.ctrl_reg |= 1 << MFM_CTRL_CALIBRATON;
    calibration_flag = 1;
    calibration_cnt  = 0;
}

void MFMStopCalibration(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->MFM.ctrl_reg &= ~(1 << MFM_CTRL_CALIBRATON);
    calibration_flag = 0;
}

void MFMSetB0(uint32_t B0){
    //AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    //TODO regs->MFM.b0 = B0;
}

void MFMSetATOA(uint32_t coeff){
    //AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    //TODO regs->MFM.analog_to_analog = coeff;
}

void MFMSetATOD(uint32_t coeff){
    //AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    //TODO regs->MFM.b_series_analog = coeff;
}

void MFMSetDTOA(uint32_t coeff){
    //AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    //TODO regs->MFM.digital_to_analog = coeff;
}

int AFEDDS_SYNC(void*){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    if(reset && ( regs->MFM.ctrl_reg & (1 << MFM_CTRL_ZERO))){
        reset = 0;
        regs->MFM.ctrl_reg &= ~(1 << MFM_CTRL_ZERO);
    }

    if(calibration_flag && ( regs->MFM.ctrl_reg & (1 << MFM_CTRL_CALIBRATON))){
        MFMStopCalibration();
    }
    if(calibration_cnt <= CALIBRATION_TIME){
        calibration_cnt++;
    }

    koeffAB = controlKoeffAB();
    koeffDB = controlKoeffDB();
    mode    = controlMode();
    return 0;
}

int AFEEvent(uint32_t event, void*){
    if(event == 0){
        return 0;
    }
    if(event == controlB0Ev()){
        MFMSetB0(controlB0());
    }
    if(event == controlStartEv()){
        MFMStartIntegral();
    }
    if(event == controlStopEv()){
        MFMStopIntegral();
    }
    if(event == controlResetEv()){
        MFMResetIntegral();
    }
    if(event == controlCalEv()){
        MFMStartCalibration();
    }
    return 0;
}

void MFMSetMode(int new_mode){
    //AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    //TODO regs->MFM.mode = new_mode;
    mode = new_mode;
}

int64_t MFMGetIntegral(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    uint64_t intH = 0;
    uint32_t intL = 0;

    if(mode == MFM_MODE_ANALOG_TO_ANALOG || mode == MFM_MODE_ANALOG_TO_DIGITAL){
        intH = regs->MFM.mf_ana_i_hi;
        intL = regs->MFM.mf_ana_i_low;
    } else if(mode == MFM_MODE_DIGITAL_TO_ANALOG || mode == MFM_MODE_DIGITAL_TO_DIGITAL){
        intH = regs->MFM.mf_dig_i_hi;
        intL = regs->MFM.mf_dig_i_low;
    }
    uint64_t res = (intH << 32) | intL;

    return *(int64_t*)&res;
}

float MFMGetB(){
    if(mode == MFM_MODE_ANALOG_TO_ANALOG || mode == MFM_MODE_ANALOG_TO_DIGITAL){
        return (float)koeffAB * (float)((double)MFMGetIntegral());
    } else if(mode == MFM_MODE_DIGITAL_TO_ANALOG || mode == MFM_MODE_DIGITAL_TO_DIGITAL){
        return (float)koeffDB * (float)((double)MFMGetIntegral());
    }
    return 0.f;
}

int32_t MFMGetADC(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    return regs->MFM.last_cal_adc_data;
}

int32_t MFMGetDAC(){
    //AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    return 0;
}

void AFEEmulinit(){
    REGS_BASE_AFE = (AFERegs *) malloc(sizeof(AFERegs));
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->ctrl_reg = 7;
    regs->dds_ena_reg = 0xf;
    regs->ph_adj_start = 0;
    regs->stat_reg = 0x3;
    regs->jc_status = 0x4ef;
    regs->clk_sync_result_reg = 0x2;
    regs->clk_sync_result = 0;
    regs->MFM.ctrl_reg = 0;
    regs->MFM.stat_reg = 0x1;
    regs->MFM.mf_ana_i_hi = 0;
    regs->MFM.mf_ana_i_low = 0;
    regs->MFM.mf_dig_i_hi = 0;
    regs->MFM.mf_dig_i_low = 0;
    regs->MFM.mf_ref_signal = 0x14df4;
    regs->MFM.mf_zer_signal = 0xffffffde;
    regs->MFM.adc_cal_k = 0x40375;
    regs->MFM.adc_cal_offset = 0xffffffde;
    regs->MFM.last_cal_adc_data = 0x0;
    regs->MFM.last_raw_adc_data = 0xffffffde;
}   

void AFEInit(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->ctrl_reg = 0;
}