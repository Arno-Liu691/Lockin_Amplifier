/* lia_core.h */
#ifndef LIA_CORE_H
#define LIA_CORE_H

#include "lia_config.h"
#include "nco_lut.h"
#include "adc_acq.h"
#include "ref_output.h"
#include "serial_if.h"
#include "buttons.h"

typedef struct {
    float I_acc;
    float Q_acc;
    float alpha;          // 最终 IIR 低通系数
    uint32_t decim_counter;
    float i_sum, q_sum;

    int    bw_index;      // 当前带宽档位索引
    float  Fs_block;      // 块更新频率 (= FS_ADC / DECIM_N)

    uint32_t print_counter;
} LIA_State_t;

float LIA_ComputeResponse(float amp_vpp);


void LIA_SystemInit();
void LIA_Task(void);



#endif /* LIA_CORE_H */
