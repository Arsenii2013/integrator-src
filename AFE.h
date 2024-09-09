#ifndef _AFE_H_
#define _AFE_H_
#include "main.h"
#include "scr.h"

typedef struct{
    uint32_t dds_freq;
    uint32_t ph_adj_desired_phase;
    uint32_t ph_adj_pre_delay_time;
    uint32_t ph_adj_post_delay_time;
    uint32_t init_phase;
    uint32_t nope_0[8];
    uint32_t stat_reg;
    uint32_t phase;
    uint32_t freq;
} DDSRegs;

typedef struct{
    uint32_t ctrl_reg;
    uint32_t nope_0[15];
    uint32_t stat_reg;
    uint32_t mf_ana_i_hi;
    uint32_t mf_ana_i_low;
    uint32_t mf_dig_i_hi;
    uint32_t mf_dig_i_low;
    uint32_t mf_ref_signal;
    uint32_t mf_zer_signal;
    uint32_t adc_cal_k;
    uint32_t adc_cal_offset;
    uint32_t last_cal_adc_data;
    uint32_t last_raw_adc_data;
} MFMRegs;

typedef struct{
    uint32_t ctrl_reg;
    uint32_t dds_ena_reg;
    uint32_t ph_adj_start;
    uint32_t nope_0[12];
    uint32_t stat_reg;
    uint32_t jc_status;
    uint32_t clk_sync_result_reg;
    uint32_t clk_sync_result;
    uint32_t nope_1[13];
    DDSRegs  DDS0;
    uint32_t nope_2[16];
    DDSRegs  DDS1;
    uint32_t nope_3[16];
    DDSRegs  DDS2;
    uint32_t nope_4[16];
    DDSRegs  DDS3;
    uint32_t nope_5[16];
    MFMRegs  MFM;
} AFERegs;

#define MFM_CTRL_OPERATION  0
#define MFM_CTRL_ZERO       1
#define MFM_CTRL_CALIBRATON 2
#define MFM_CTRL_BREF_ENA   3
#define MFM_CTRL_INT_SYNC   4

#define MFM_STAT_CALIBRATION 0

#define CALIBRATION_TIME    1

#define MFM_MODE_ANALOG_TO_ANALOG 0
#define MFM_MODE_ANALOG_TO_DIGITAL 1
#define MFM_MODE_DIGITAL_TO_ANALOG 2
#define MFM_MODE_DIGITAL_TO_DIGITAL 3

void MFMStartIntegral();
void MFMStopIntegral();
void MFMResetIntegral();
void MFMStartCalibration();
void MFMSetB0(uint32_t B0);
void MFMSetATOA(uint32_t coeff);
void MFMSetATOD(uint32_t coeff);
void MFMSetDTOA(uint32_t coeff);
void MFMSetMode(int new_mode);
uint64_t MFMGetIntegral();
uint64_t MFMGetB();

int AFEDDS_SYNC(void*);
int AFEEvent(uint32_t event, void*);
void AFEEmulinit();
volatile AFERegs * AFERegPtr();

#endif // _AFE_H_