/* lia_core.c */
#include "lia_core.h"

static LIA_State_t g_lia;
static NCO_LUT_t   g_nco_demod;

#define AMP_CAL_OFFSET_RATIO    1.0f
#define PHASE_DELAY_INTERCEPT_DEG  0.0f
#define PHASE_DELAY_SLOPE_DEG_PER_HZ  0.0f
#define SYSTEM_DELAY 0.0028611111f

#define bw_default 9

float f_ref = 1000.0f;
static float PHASE_CAL_OFFSET_RAD;
static const float DAC_FS_FRAC = 2047.0f / 2048.0f;

float phase_cali(float freq, float raw_phase) {
    float k, b;
    // 根据频率分段选择线性参数 (Delta = k * f + b)
    if (freq <= 49.0f) {
        // Zone 1
        k = 0.88734f; b = -60.322f;
    }
    else if (freq <= 120.0f) {
        // Zone 2
        k = 0.03198f; b = -16.402f;
    }
    else if (freq <= 600.0f) {
        // Zone 3
        k = -0.06568f; b = -6.6799f;
    }
    else if (freq <= 2500.0f) {
        // Zone 4
        k = -0.07633f; b = -1.3698f;
    }
    else if (freq <= 12000.0f) {
        // Zone 5
        k = -0.07680f; b = -0.372f;
    }
    else if (freq <= 25000.0f) {
        // Zone 6
        k = -0.07681f; b = -0.272f;
    }
    else if (freq <= 60000.0f) {
        // Zone 7
        k = -0.07667f; b = -1.968f;
    }
    else if (freq <= 80000.0f) {
        // Zone 8
        k = -0.07671f; b = -2.391f;
    }
    else {
        // Zone 9
        k = -0.07673f; b = -2.306f;
    }
    // 1. 计算理论应该补偿的相位值
    float correction_val = k * freq + b;
    // 2. 加上原始相位
    float final_phase = raw_phase + correction_val;
    // 3. 归一化处理 (把结果限制在 -180 到 180 之间)
    // 这是为了处理相位解缠后的数值可能很大的情况
    while (final_phase > 180.0f) {
        final_phase -= 360.0f;
    }
    while (final_phase <= -180.0f) {
        final_phase += 360.0f;
    }
    return final_phase;
}

float amp_cali(float freq, float raw_amp) {
    float k, b;
    // 根据频率分段选择线性参数 (Gain = k * f + b)
    if (freq <= 45.0f) {
        k = -0.00440f; b = 1.3074f;
    }
    else if (freq <= 100.0f) {
        k = -0.000152f; b = 1.0950f;
    }
    else if (freq <= 600.0f) {
        k = -5.00e-7f; b = 1.0805f;
    }
    else if (freq <= 1500.0f) {
        k = -1.13e-6f; b = 1.0808f;
    }
    else if (freq <= 8000.0f) {
        k = 5.00e-7f; b = 1.0776f;
    }
    else if (freq <= 30000.0f) {
        k = 2.00e-7f; b = 1.0781f;
    }
    else if (freq <= 50000.0f) {
        k = 2.03e-7f; b = 1.0779f;
    }
    else if (freq <= 80000.0f) {
        k = -1.50e-7f; b = 1.0802f;
    }
    else {
        k = -1.25e-6f; b = 1.1680f;
    }

    // 计算补偿增益
    float gain = k * freq + b;
    // 返回校准后的幅度
    return raw_amp * gain;
}

// 幅度微调：输入初步校准后的 amp，输出最终 amp
float amp_fine_tune(float freq, float pre_cali_amp) {
    float correction_factor = 1.0f;
    if (freq <= 32.0f) {
        correction_factor = 0.925f + 0.00241f * (freq - 28.89f);
    }
    else if (freq <= 55.0f) {
        correction_factor = 0.955f + 0.0035f * (freq - 33.1f);
    }
    else if (freq >= 75000.0f) {
        correction_factor = 1.0122f + 8.59e-7f * (freq - 80000.0f);
    }

    return pre_cali_amp * correction_factor;
}

// 相位微调：输入初步校准后的 phase，输出最终 phase
float phase_fine_tune(float freq, float pre_cali_phase) {
    float final_phase = pre_cali_phase;
    if (freq <= 53.0f) {
        float adjustment = 13.01f + (freq - 26.29f) * -0.484f;
        final_phase += adjustment;
    }
    else if (freq >= 50000.0f) {
        final_phase -= 2.12f;
    }
    // 再次归一化，防止边界溢出
    while (final_phase > 180.0f) final_phase -= 360.0f;
    while (final_phase <= -180.0f) final_phase += 360.0f;
    return final_phase;
}


float LIA_ComputeResponse(float amp_vpp)
{
    float v_dac_vpp = DAC_VREF * DAC_FS_FRAC;  // 可以近似为 DAC_VREF/2
    if (v_dac_vpp > 0.0f) {
        return amp_vpp / v_dac_vpp;
    } else {
        return 0.0f;
    }
}

static void LIA_UpdateAlpha(void)
{
    float bw = g_bw_table[g_lia.bw_index];
    float alpha = 2.0f * (float)M_PI * bw / g_lia.Fs_block;
    if (alpha > 1.0f) alpha = 1.0f;
    if (alpha < 0.0f) alpha = 0.0f;
    g_lia.alpha = alpha;
}

static void LIA_ProcessBlock(uint16_t *p, uint32_t len)
{
    for (uint32_t n = 0; n < len; n++) {
        /* 12bit ADC → [-1,1] */
        float x = ((float)p[n] - 2048.0f) * (1.0f / 2048.0f);

        /* NCO 一步：使用 demod NCO（与 DAC 使用相同频率） */
        float s, c;
        NCO_LUT_Step(&g_nco_demod, &s, &c);

        float I = x * c;
        float Q = x * s;

        g_lia.i_sum += I;
        g_lia.q_sum += Q;
        g_lia.decim_counter++;

        if (g_lia.decim_counter >= DECIM_N) {
            float invN = 1.0f / (float)DECIM_N;
            float i_block = g_lia.i_sum * invN;
            float q_block = g_lia.q_sum * invN;

            /* 最后一层 IIR 低通 */
            g_lia.I_acc += g_lia.alpha * (i_block - g_lia.I_acc);
            g_lia.Q_acc += g_lia.alpha * (q_block - g_lia.Q_acc);

            g_lia.decim_counter = 0;
            g_lia.i_sum = 0.0f;
            g_lia.q_sum = 0.0f;

            g_lia.print_counter++;
        }
    }
}

void LIA_SystemInit()
{
	/* 初始化按键模块 */
	Buttons_Init();

    if (Serial_ReadFloat("f_ref (Hz)", &f_ref) != 0) {
	    f_ref = 1000.0f;
    }
    if (f_ref < 9.9f)      f_ref = 10.0f;
    if (f_ref > 100000.1f)  f_ref = 100000.0f;
    /* 初始化状态 */
    g_lia.I_acc = 0.0f;
    g_lia.Q_acc = 0.0f;
    g_lia.decim_counter = 0;
    g_lia.i_sum = 0.0f;
    g_lia.q_sum = 0.0f;
    g_lia.print_counter = 0;

    g_lia.Fs_block = FS_ADC / (float)DECIM_N;
    g_lia.bw_index = bw_default;   // 初始带宽
    LIA_UpdateAlpha();

    /* 初始化 demod NCO：与 DAC 一样的 fs 与 f_ref，带可选相位校准 */
    PHASE_CAL_OFFSET_RAD = -2.0f * (float)M_PI * f_ref * SYSTEM_DELAY;
    NCO_LUT_Init(&g_nco_demod, FS_ADC, f_ref, PHASE_CAL_OFFSET_RAD);

    /* 初始化参考输出（DAC+NCO） */
    RefOutput_Init(f_ref);

    /* 启动 ADC 捕获 */
    ADC_Acq_Start();

    /* 启动 DAC & TIM2 */
    RefOutput_Start();

    Serial_Printf("LIA init done. f_ref = %.3f Hz\r\n", f_ref);
    Serial_Printf("Initial BW = %.3f Hz\r\n", g_bw_table[g_lia.bw_index]);
}

void LIA_Task(void)
{
    /* 1) 处理 ADC DMA block */
    if (g_adc_ht_flag) {
        g_adc_ht_flag = 0;
        LIA_ProcessBlock(&g_adc_buf[0], ADC_BLOCK_SIZE);
    }
    if (g_adc_tc_flag) {
        g_adc_tc_flag = 0;
        LIA_ProcessBlock(&g_adc_buf[ADC_BLOCK_SIZE], ADC_BLOCK_SIZE);
    }

    /* 2) 处理 DAC 半缓冲 refill */
    RefOutput_Service();

    /* 3) 按键去抖 + 带宽事件 */
    Buttons_Service();

    if (g_bw_plus_event) {
        g_bw_plus_event = 0;
        if (g_lia.bw_index < (NUM_BW_MODES - 1)) {
            g_lia.bw_index++;
            LIA_UpdateAlpha();
            Serial_Printf("BW -> %.3f Hz\r\n", g_bw_table[g_lia.bw_index]);
        }
    }
    if (g_bw_minus_event) {
        g_bw_minus_event = 0;
        if (g_lia.bw_index > 0) {
            g_lia.bw_index--;
            LIA_UpdateAlpha();
            Serial_Printf("BW -> %.3f Hz\r\n", g_bw_table[g_lia.bw_index]);
        }
    }
    /* 3) GAIN 档位事件 */
    if (g_gain_event) {
        g_gain_event = 0;
        g_gain_index++;
        if (g_gain_index >= NUM_GAIN_MODES) {
            g_gain_index = 0;
        }
        Serial_Printf("GAIN -> %.3f\r\n", g_gain_table[g_gain_index]);
    }
//    if (g_reset_event) {
//        g_reset_event = 0;
//        NVIC_SystemReset();  // 软件复位 MCU，相当于按硬件 RESET
//    }

    /* 4) 周期性打印幅度/相位 */
    if (g_lia.print_counter >= PRINT_PERIOD_BLOCKS) {
        g_lia.print_counter = 0;

        float I = g_lia.I_acc;
        float Q = g_lia.Q_acc;
        float amp = ((sqrtf(I*I + Q*Q) / AMP_CAL_OFFSET_RATIO)* 3.3f * 2.0f) / g_gain_table[g_gain_index];
        amp = amp_cali(f_ref, amp);
        amp = amp_fine_tune(f_ref,amp);// * 0.985f;
        float phase = atan2f(Q, I);  // rad
        float phase_deg = 0.0f - phase * (180.0f / (float)M_PI);
        phase_deg = phase_cali(f_ref, phase_deg);
        phase_deg = phase_fine_tune(f_ref, phase_deg);
//        float phase_deg =  0.0f - PHASE_DELAY_INTERCEPT_DEG - PHASE_DELAY_SLOPE_DEG_PER_HZ * f_ref
//        		- phase * (180.0f / (float)M_PI);

        Serial_Printf("Vpp=%.6f, Phase=%.2f deg\r\n", amp, phase_deg);
    }
}


