#include "AFE.h"
#include "scr.h"

volatile AFERegs * REGS_BASE_AFE = (AFERegs *)0x40001800;
//volatile AFERegs * REGS_BASE_AFE = (AFERegs *)0x10000000;
struct{
    uint32_t zero;
    uint32_t calibration;
    uint32_t calibration_ready;
    uint32_t operation;
    uint32_t change_in_that_DDS_SYNC;
    float B0;
    float coeffAB;
    float coeffDB;
    float coeffBA;
    float coeffBD;
    uint32_t mode;
} IternalAFEData = {0, 0, 1, 0, 0., 0., 0, 0, 0, MFM_MODE_ANALOG_TO_ANALOG};

void MFMPrintRegs(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    TM_PRINTF("ctrl: %x stat: %x ana_hi: %x ana_low: %x\n\r", regs->MFM.ctrl_reg, regs->MFM.stat_reg, regs->MFM.mf_ana_i_hi, regs->MFM.mf_ana_i_low);
}

void MFMRefreshOffset();

void MFMSetBser(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->MFM.bser_ctrl_reg |= 1 << MFM_BSER_ENA;
}

void MFMResetBser(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->MFM.bser_ctrl_reg &= ~(1 << MFM_BSER_ENA);
}

volatile AFERegs * AFERegPtr(){
    return  REGS_BASE_AFE;
}

void MFMStartIntegral(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->MFM.ctrl_reg |= 1 << MFM_CTRL_OPERATION;
    IternalAFEData.operation = 1;
    IternalAFEData.change_in_that_DDS_SYNC = 1;
    MFMSetBser();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: start integral\n\r");
    MFMPrintRegs();
    #endif
}

void MFMStopIntegral(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->MFM.ctrl_reg &= ~(1 << MFM_CTRL_OPERATION);
    IternalAFEData.operation = 0;
    IternalAFEData.change_in_that_DDS_SYNC = 1;
    MFMResetBser();
    #ifdef DEBUG
    TM_PRINTF("DEBUG: stop integral\n\r");
    MFMPrintRegs();
    #endif
}

void MFMSetZeroIntegral(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->MFM.ctrl_reg |= 1 << MFM_CTRL_ZERO;
    IternalAFEData.zero = 1;
    #ifdef DEBUG
    MFMPrintRegs();
    #endif
}

void MFMResetZeroIntegral(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->MFM.ctrl_reg &= ~(1 << MFM_CTRL_ZERO);
    IternalAFEData.zero = 0;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: zero integral\n\r");
    MFMPrintRegs();
    #endif
}

void MFMSetCalibration(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->MFM.ctrl_reg |= 1 << MFM_CTRL_CALIBRATON;
    IternalAFEData.calibration = 1;
    #ifdef DEBUG
    MFMPrintRegs();
    #endif
}

void MFMResetCalibration(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    regs->MFM.ctrl_reg &= ~(1 << MFM_CTRL_CALIBRATON);
    IternalAFEData.calibration = 0;
    IternalAFEData.calibration_ready = 0;
    #ifdef DEBUG
    TM_PRINTF("DEBUG: start calibration\n\r");
    MFMPrintRegs();
    #endif
}

void MFMEndCalibration(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    if(regs->MFM.stat_reg & (1 << MFM_STAT_CALIBRATION_READY)){
        IternalAFEData.calibration_ready = 1;
        #ifdef DEBUG
        TM_PRINTF("DEBUG: stop calibration\n\r");
        MFMPrintRegs();
        #endif
    }
}

void MFMRefreshCoeffs(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;

    if(IternalAFEData.mode == MFM_MODE_ANALOG_TO_ANALOG){
        uint64_t coeff = (1. / (IternalAFEData.coeffAB * IternalAFEData.coeffBA)) - 1;
        regs->MFM.dac_coeff_hi = coeff >> 32;
        regs->MFM.dac_coeff_low= coeff;
    } else if(IternalAFEData.mode == MFM_MODE_ANALOG_TO_DIGITAL){
        uint64_t coeff = (1. / (IternalAFEData.coeffAB / IternalAFEData.coeffBD));
        regs->MFM.bser_step_hi = coeff >> 32;
        regs->MFM.bser_step_low= coeff;
    } else if(IternalAFEData.mode == MFM_MODE_DIGITAL_TO_ANALOG){
        uint64_t coeff = (1. / (IternalAFEData.coeffDB * IternalAFEData.coeffBA)) - 1;
        regs->MFM.dac_coeff_hi = coeff >> 32;
        regs->MFM.dac_coeff_low= coeff;
    } else if(IternalAFEData.mode == MFM_MODE_DIGITAL_TO_DIGITAL){
        uint64_t coeff = (IternalAFEData.coeffBD / IternalAFEData.coeffDB);
        regs->MFM.bser_step_hi = coeff >> 32;
        regs->MFM.bser_step_low= coeff;
    }
}

void MFMRefreshOffset(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;

    if(IternalAFEData.mode == MFM_MODE_ANALOG_TO_ANALOG){
        regs->MFM.dac_offset = IternalAFEData.B0 * IternalAFEData.coeffBA;
    }
    if(IternalAFEData.mode == MFM_MODE_DIGITAL_TO_ANALOG){
        regs->MFM.dac_offset = IternalAFEData.B0 * IternalAFEData.coeffBA;
    }
}

void MFMRefreshMode(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    uint32_t DAC_ctrl  = regs->MFM.dac_ctrl_reg & 0xffffffffc;
    uint32_t Bser_ctrl = regs->MFM.bser_ctrl_reg & 0xffffffffc;

    if(IternalAFEData.mode == MFM_MODE_ANALOG_TO_ANALOG){
        DAC_ctrl |= MFM_SOURCE_ANALOG;
        Bser_ctrl|= MFM_SOURCE_ZERO;
    } else if(IternalAFEData.mode == MFM_MODE_ANALOG_TO_DIGITAL){
        DAC_ctrl |= MFM_SOURCE_ZERO;
        Bser_ctrl|= MFM_SOURCE_ANALOG;
    } else if(IternalAFEData.mode == MFM_MODE_DIGITAL_TO_ANALOG){
        DAC_ctrl |= MFM_SOURCE_BSER;
        Bser_ctrl|= MFM_SOURCE_ZERO;
    } else if(IternalAFEData.mode == MFM_MODE_DIGITAL_TO_DIGITAL){
        DAC_ctrl |= MFM_SOURCE_ZERO;
        Bser_ctrl|= MFM_SOURCE_BSER;
    }
    regs->MFM.dac_ctrl_reg  = DAC_ctrl;
    regs->MFM.bser_ctrl_reg = Bser_ctrl;
}

int AFEEvent(uint32_t event, void*){
    if(event == 0){
        return 0;
    }
    for(uint32_t i = 0; i < 4; i++){
        if(event == controlStartEv(i)){
            if(!IternalAFEData.calibration_ready){
                statusAFECallibration();
            } else if(IternalAFEData.operation) {
                statusAFEStartStart();
            } else {
                if(IternalAFEData.change_in_that_DDS_SYNC){
                    statusAFEStartStop();
                }
                MFMStartIntegral();
            }
        }
        if(event == controlStopEv(i)){
            if(!IternalAFEData.calibration_ready){
                statusAFECallibration();
            } else if(!IternalAFEData.operation) {
                statusAFEStopStop();
            } else {
                if(IternalAFEData.change_in_that_DDS_SYNC){
                    statusAFEStartStop();
                }
                MFMStopIntegral();
            }
            MFMStopIntegral();
        }
    }
    if(event == controlZeroEv()){
        if(!IternalAFEData.calibration_ready){
            statusAFECallibration();
        } else {
            MFMSetZeroIntegral();
            IternalAFEData.B0 = controlB0();
            MFMRefreshOffset();
        }
    }
    if(event == controlCalEv()){
        if(!IternalAFEData.calibration_ready){
            statusAFECallibration();
        } else {
            MFMSetCalibration();
        }
    }
    return 0;
}

int AFEDDS_SYNC(void*){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    IternalAFEData.change_in_that_DDS_SYNC = 0;
    statusAFEState(regs->MFM.stat_reg >> 1);

    if(IternalAFEData.zero){
        MFMResetZeroIntegral();
    }

    if(IternalAFEData.calibration){
        MFMResetCalibration();
    }

    if(!IternalAFEData.calibration_ready){
        MFMEndCalibration();
    }

    if(IternalAFEData.mode != controlMode()){
        IternalAFEData.mode = controlMode();
        MFMRefreshMode();
        MFMRefreshCoeffs();
    }
    if(IternalAFEData.coeffAB != controlCoeffAB()){
        IternalAFEData.coeffAB = controlCoeffAB();
        MFMRefreshCoeffs();
    }
    if(IternalAFEData.coeffDB != controlBserIn()){
        IternalAFEData.coeffDB = controlBserIn();
        MFMRefreshCoeffs();
    }
    if(IternalAFEData.coeffBA != controlCoeffBA()){
        IternalAFEData.coeffBA = controlCoeffBA();
        MFMRefreshCoeffs();
    }
    if(IternalAFEData.coeffBD != controlBserOut()){
        IternalAFEData.coeffBD = controlBserOut();
        MFMRefreshCoeffs();
    }
    return 0;
}

int64_t MFMGetIntegral(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    uint64_t intH = 0;
    uint32_t intL = 0;

    if(IternalAFEData.mode == MFM_MODE_ANALOG_TO_ANALOG || IternalAFEData.mode == MFM_MODE_ANALOG_TO_DIGITAL){
        intH = regs->MFM.mf_ana_i_hi;
        intL = regs->MFM.mf_ana_i_low;
    } else if(IternalAFEData.mode == MFM_MODE_DIGITAL_TO_ANALOG || IternalAFEData.mode == MFM_MODE_DIGITAL_TO_DIGITAL){
        intH = regs->MFM.mf_dig_i_hi;
        intL = regs->MFM.mf_dig_i_low;
    }
    uint64_t res = (intH << 32) | intL;

    return *(int64_t*)&res;
}

float MFMGetB(){
    if(IternalAFEData.mode == MFM_MODE_ANALOG_TO_ANALOG || IternalAFEData.mode == MFM_MODE_ANALOG_TO_DIGITAL){
        return (float)IternalAFEData.coeffAB * (float)((double)MFMGetIntegral()) + IternalAFEData.B0;
    } else if(IternalAFEData.mode == MFM_MODE_DIGITAL_TO_ANALOG || IternalAFEData.mode == MFM_MODE_DIGITAL_TO_DIGITAL){
        return (float)IternalAFEData.coeffDB * (float)((double)MFMGetIntegral()) + IternalAFEData.B0;
    }
    return 0.f;
}

int32_t MFMGetADC(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    return regs->MFM.last_cal_adc_data;
}

uint32_t MFMGetDAC(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    return regs->MFM.dac_signal;
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
    regs->MFM.ctrl_reg = 0;
    regs->MFM.bser_ctrl_reg = 0;
    regs->MFM.dac_ctrl_reg = 0;
    MFMRefreshMode();
    MFMRefreshCoeffs();
    MFMRefreshOffset();
    MFMResetBser();
    MFMSetZeroIntegral();
    regs->MFM.bser_pulse_duration = 100;
    regs->MFM.bser_pause_duration = 200;
    #ifdef DEBUG
    MFMPrintRegs();
    #endif
}