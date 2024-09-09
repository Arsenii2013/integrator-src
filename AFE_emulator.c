#include "AFE_emulator.h"
#include "math.h"

uint32_t emulRunning = 0;
uint64_t timestamp = 0;
uint32_t calibration = 0;

int AFEEmulDDS_SYNC(void *){
    volatile AFERegs* regs = AFERegPtr();
    if(regs->MFM.ctrl_reg & (1 << MFM_CTRL_OPERATION)){
        double arg = (double)timestamp / DDS_SYNC_PRD / 100.* 2 * M_PI;
        double sinus = sin(arg);
        double cosinus = cos(arg); 
        
        int64_t sinus_i = sinus * 0x7fffffffffff0000;
        int64_t cosinus_i = cosinus * 0x7fffffffffff0000;

        // TODO if(regs->MFM.mode == MFM_MODE_ANALOG_TO_ANALOG || regs->MFM.mode == MFM_MODE_ANALOG_TO_DIGITAL)
        regs->MFM.mf_ana_i_hi = sinus_i >> 32;
        regs->MFM.mf_ana_i_low = sinus_i;
        // TODO else if(regs->MFM.mode == MFM_MODE_DIGITAL_TO_ANALOG || regs->MFM.mode == MFM_MODE_DIGITAL_TO_DIGITAL)
        regs->MFM.mf_dig_i_hi = cosinus_i >> 32;
        regs->MFM.mf_dig_i_low = cosinus_i;
        timestamp += DDS_SYNC_PRD;
    }

    if(regs->MFM.ctrl_reg & (1 << MFM_CTRL_ZERO)){
        timestamp = 0;
        regs->MFM.mf_ana_i_hi = 0;
        regs->MFM.mf_ana_i_low = 0;
        regs->MFM.mf_dig_i_hi = 0;
        regs->MFM.mf_dig_i_low = 0;
    }

    if(regs->MFM.ctrl_reg & (1 << MFM_CTRL_CALIBRATON)){
        regs->MFM.stat_reg &= ~(1 << MFM_STAT_CALIBRATION);
    } else {
        regs->MFM.stat_reg |= (1 << MFM_STAT_CALIBRATION);
    }
    return 0;
}