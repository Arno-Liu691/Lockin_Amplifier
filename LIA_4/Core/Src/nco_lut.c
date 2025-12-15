#include "nco_lut.h"

float nco_sin_lut[NCO_LUT_SIZE];

void NCO_LUT_Generate(void)
{
    for (uint32_t k = 0; k < NCO_LUT_SIZE; k++) {
        float theta = 2.0f * (float)M_PI * (float)k / (float)NCO_LUT_SIZE;
        nco_sin_lut[k] = sinf(theta);
    }
}

/* 内部小工具：把相位偏移（弧度）映射到 0..2^32-1 */
static uint32_t phase_from_offset(float phase_offset_rad)
{
    float frac = phase_offset_rad / (2.0f * (float)M_PI);
    while (frac < 0.0f)  frac += 1.0f;
    while (frac >= 1.0f) frac -= 1.0f;
    return (uint32_t)(frac * NCO_PHASE_MAX);
}

void NCO_LUT_Init(NCO_LUT_t *nco, float fs, float f0, float phase_offset_rad)
{
    nco->fs    = fs;
    nco->phase = phase_from_offset(phase_offset_rad);
    NCO_LUT_SetFreq(nco, f0);
}

void NCO_LUT_SetFreq(NCO_LUT_t *nco, float f0)
{
    if (f0 < 0.0f) f0 = -f0;
    if (nco->fs <= 0.0f) return;

    double step = ((double)f0 / (double)nco->fs) * (double)NCO_PHASE_MAX;
    nco->phase_step = (uint32_t)step;
}

void NCO_LUT_SetPhaseOffset(NCO_LUT_t *nco, float phase_offset_rad)
{
    nco->phase = phase_from_offset(phase_offset_rad);
}
