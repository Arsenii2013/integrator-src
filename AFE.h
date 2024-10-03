#ifndef _AFE_H_
#define _AFE_H_
#include "main.h"

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
    uint32_t dac_ctrl_reg;
    uint32_t dac_coeff_hi;
    uint32_t dac_coeff_low;
    uint32_t dac_offset;
    uint32_t bser_ctrl_reg;
    uint32_t bser_step_hi;
    uint32_t bser_step_low;
    uint32_t bser_pulse_duration;
    uint32_t bser_pause_duration;
    uint32_t nope_0[6];
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
    uint32_t dac_signal;
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

#define MFM_STAT_CALIBRATION_READY 0

#define CALIBRATION_TIME 50

#define MFM_MODE_ANALOG_TO_ANALOG   0
#define MFM_MODE_ANALOG_TO_DIGITAL  1
#define MFM_MODE_DIGITAL_TO_ANALOG  2
#define MFM_MODE_DIGITAL_TO_DIGITAL 3

#define MFM_SOURCE_ZERO 0 
#define MFM_SOURCE_ANALOG 1
#define MFM_SOURCE_BSER 2 
#define MFM_SOURCE_TEST 3 

int64_t MFMGetIntegral();
float MFMGetB();
int32_t MFMGetADC();
uint32_t MFMGetDAC();

int AFEDDS_SYNC(void*);
int AFEEvent(uint32_t event, void*);
void AFEEmulinit();
void AFEInit();
volatile AFERegs * AFERegPtr();

#endif // _AFE_H_