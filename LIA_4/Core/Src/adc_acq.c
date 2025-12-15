/* adc_acq.c */
#include "adc_acq.h"

uint16_t g_adc_buf[ADC_BUF_SIZE];
volatile uint8_t g_adc_ht_flag = 0;
volatile uint8_t g_adc_tc_flag = 0;

void ADC_Acq_Start(void)
{
    /* 由 CubeMX 生成 hadc1 和 DMA config，这里只启动 DMA */
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)g_adc_buf, ADC_BUF_SIZE);
}

/* DMA 回调：只置 flag，不做计算 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1) {
        g_adc_ht_flag = 1;
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1) {
        g_adc_tc_flag = 1;
    }
}
