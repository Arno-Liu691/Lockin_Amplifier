/* ref_output.h */
#ifndef REF_OUTPUT_H
#define REF_OUTPUT_H

#include "lia_config.h"
#include "nco_lut.h"

#define DAC_BUF_SIZE   1024   // 可以和 ADC_BLOCK_SIZE 一样或不同

extern uint16_t g_dac_buf[DAC_BUF_SIZE];
extern volatile uint8_t g_dac_ht_flag;
extern volatile uint8_t g_dac_tc_flag;

/* 初始化 NCO + DAC + DMA（不含 TIM2，TIM2 由主工程初始化） */
void RefOutput_Init(float f_ref);

/* 启动 DAC DMA（以及 TIM2，如果你希望在此一起启动） */
void RefOutput_Start(void);

/* 在主循环里调用，用于根据 DMA 标志 refill 半缓冲 */
void RefOutput_Service(void);

#endif /* REF_OUTPUT_H */
