/* lia_config.c */
#include "lia_config.h"

/* 1-2-5 带宽表，单位 Hz */
const float g_bw_table[NUM_BW_MODES] = {
    0.001f, 0.002f, 0.005f,
    0.010f, 0.020f, 0.050f,
    0.100f, 0.200f, 0.500f,
    1.000f, 2.000f, 5.000f,
    10.000f    // 如果不需要 10 Hz，可以删掉，记得改 NUM_BW_MODES
};

/* 事件标志在此定义（避免多重定义） */
volatile uint8_t g_bw_plus_event  = 0;
volatile uint8_t g_bw_minus_event = 0;
volatile uint8_t g_gain_event = 0;
volatile uint8_t g_reset_event = 0;

const float g_gain_table[NUM_GAIN_MODES] = {
		2.24f, 5.26f, 10.56f, 21.2f,
    53.0f, 108.0f, 214.0f, 510.0f
};

int g_gain_index = 1;

