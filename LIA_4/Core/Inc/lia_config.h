/* lia_config.h */
#ifndef LIA_CONFIG_H
#define LIA_CONFIG_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <math.h>

/* ----------------- 采样与缓冲参数 ----------------- */
#define FS_ADC             400000.0f   // ADC & DAC 采样率 300 kHz
#define ADC_BLOCK_SIZE     2048        // 半缓冲大小
#define ADC_BUF_SIZE       (2 * ADC_BLOCK_SIZE)

#define NCO_LUT_SIZE       256
#define NCO_PHASE_BITS     32
#define NCO_PHASE_MAX      4294967296.0f  // 2^32

/* 抽取：每 DECIM_N 个样点做一次块平均 */
#define DECIM_N            4000

/* 带宽表（1-2-5）: 1 mHz ~ 5 Hz 示例，可自行扩展到 10 Hz */
#define NUM_BW_MODES       13
extern const float g_bw_table[NUM_BW_MODES];

/* 去抖时间 */
#define DEBOUNCE_MS        30

/* 打印周期：每多少个“块更新”打印一次结果 */
#define PRINT_PERIOD_BLOCKS  100

/* ---- HAL 句柄（由你的 CubeMX 工程提供） ---- */
extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef  hadc1;
extern DAC_HandleTypeDef  hdac;
extern TIM_HandleTypeDef  htim2;

/* ---- 对外事件标志（由 buttons.c 置位，lia_core 读取） ---- */
extern volatile uint8_t g_bw_plus_event;
extern volatile uint8_t g_bw_minus_event;
extern volatile uint8_t g_gain_event;
extern volatile uint8_t g_reset_event;

#define ADC_VREF   3.3f   // 根据你板子实际情况改
#define DAC_VREF   3.3f   // 一般也是 3.3V

/* 前端模拟增益档位表（8 档），现在先都写 1.0，将来自己改 */
#define NUM_GAIN_MODES 8
extern const float g_gain_table[NUM_GAIN_MODES];
extern int g_gain_index;

#endif /* LIA_CONFIG_H */
