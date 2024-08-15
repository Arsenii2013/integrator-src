#include "AFE.h"

AFERegs * REGS_BASE_AFE = 0x40000000 + 0x1800;

uint32_t reset = 0;
uint32_t calibration_cnt = 0;
uint32_t koeffAB = 0;
uint32_t koeffDB = 0;
uint32_t mode = MFM_MODE_ANALOG;

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
    calibration_cnt = 0;
}

void MFMSetB0(uint32_t B0){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    //TODO regs->MFM.b0 = B0;
}

void MFMSetATOA(uint32_t coeff){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    //TODO regs->MFM.analog_to_analog = coeff;
}

void MFMSetATOD(uint32_t coeff){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    //TODO regs->MFM.b_series_analog = coeff;
}

void MFMSetDTOA(uint32_t coeff){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    //TODO regs->MFM.digital_to_analog = coeff;
}

int AFEDDS_SYNC(void*){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    if(reset && ( regs->MFM.ctrl_reg & (1 << MFM_CTRL_ZERO))){
        reset = 0;
        regs->MFM.ctrl_reg &= ~(1 << MFM_CTRL_ZERO);
    }

    if(calibration_cnt == CALIBRATION_TIME && ( regs->MFM.ctrl_reg & (1 << MFM_CTRL_ZERO))){
        regs->MFM.ctrl_reg &= ~(1 << MFM_CTRL_CALIBRATON);
    }else if(calibration_cnt < CALIBRATION_TIME){
        calibration_cnt++;
    }
    return 0;
}

void MFMSetMode(int new_mode){
    mode = new_mode;
}

uint64_t MFMGetIntegral(){
    AFERegs* regs = (AFERegs*) REGS_BASE_AFE;
    uint32_t intH = 0;
    uint32_t intL = 0;

    if(mode == MFM_MODE_ANALOG){
        intH = regs->MFM.mf_ana_i_hi;
        intL = regs->MFM.mf_ana_i_low;
    } else if(mode == MFM_MODE_DIGITAL){
        intH = regs->MFM.mf_dig_i_hi;
        intL = regs->MFM.mf_dig_i_low;
    } else {
        intH = 0xffffffff;
        intL = 0xffffffff;
    }

    return (intH << 32) | intL;
}

uint64_t MFMGetB(){
    if(mode == MFM_MODE_ANALOG){
        return koeffAB * MFMGetIntegral();
    } else if(mode == MFM_MODE_DIGITAL){
        return koeffDB * MFMGetIntegral();
    }
    return 0xffffffffffffffff;
}