/* adc_acq.h */
#ifndef ADC_ACQ_H
#define ADC_ACQ_H

#include "lia_config.h"

extern uint16_t g_adc_buf[ADC_BUF_SIZE];
extern volatile uint8_t g_adc_ht_flag;
extern volatile uint8_t g_adc_tc_flag;

void ADC_Acq_Start(void);

#endif /* ADC_ACQ_H */
