/* ref_output.c */
#include "ref_output.h"

uint16_t g_dac_buf[DAC_BUF_SIZE];
volatile uint8_t g_dac_ht_flag = 0;
volatile uint8_t g_dac_tc_flag = 0;

static float DAC_Vp = 408.0f;
//static float DAC_offset = 205.0f;
static NCO_LUT_t g_nco_dac;

/* 把 NCO 输出映射到 12bit DAC [0..4095] */
static uint16_t nco_sample_to_dac(float s)
{
    float v = s;
    if (v > 1.0f)  v = 1.0f;
    if (v < -1.0f) v = -1.0f;
    /* 中点 2048，幅度 2047 */
    float dac_f = 200.0f + DAC_Vp * 1.1f + v * DAC_Vp;
    if (dac_f < 0.0f) dac_f = 0.0f;
    if (dac_f > 4095.0f) dac_f = 4095.0f;
    return (uint16_t)dac_f;
}

static void RefOutput_FillHalfBuffer(uint32_t offset, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        float s, c;
        NCO_LUT_Step(&g_nco_dac, &s, &c);
        g_dac_buf[offset + i] = nco_sample_to_dac(c);
    }
}

void RefOutput_Init(float f_ref)
{
    /* NCO: 和 ADC 相同采样率，初始相位 0 */
    NCO_LUT_Init(&g_nco_dac, FS_ADC, f_ref, 0.0f);

    /* 先填满整个 DAC buffer */
    RefOutput_FillHalfBuffer(0, DAC_BUF_SIZE);

    /* 配置 DAC: 由 CubeMX 生成 hdac 配置，这里只启用触发 & DMA */
    // HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)g_dac_buf,
    //                   DAC_BUF_SIZE, DAC_ALIGN_12B_R);
    // TIM2 在 RefOutput_Start 或 main 中启动
}

void RefOutput_Start(void)
{
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)g_dac_buf,
                      DAC_BUF_SIZE, DAC_ALIGN_12B_R);
    HAL_TIM_Base_Start(&htim2);  // TIM2 触发 DAC & ADC
}

/* DAC DMA 回调：只标记 */
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
    g_dac_ht_flag = 1;
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
    g_dac_tc_flag = 1;
}

/* 主循环中调用，根据 flag refill 半缓冲 */
void RefOutput_Service(void)
{
    if (g_dac_ht_flag) {
        g_dac_ht_flag = 0;
        RefOutput_FillHalfBuffer(0, DAC_BUF_SIZE / 2);
    }
    if (g_dac_tc_flag) {
        g_dac_tc_flag = 0;
        RefOutput_FillHalfBuffer(DAC_BUF_SIZE / 2, DAC_BUF_SIZE / 2);
    }
}
