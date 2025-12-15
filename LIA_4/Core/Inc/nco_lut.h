#ifndef NCO_LUT_H
#define NCO_LUT_H

#include "math.h"
#include "lia_config.h"   // 里面有 NCO_LUT_SIZE, NCO_PHASE_BITS 等

#define NCO_LUT_BITS      8                     // 2^8 = 256
#define NCO_INDEX_SHIFT   (NCO_PHASE_BITS - NCO_LUT_BITS)

typedef struct {
    uint32_t phase;
    uint32_t phase_step;
    float    fs;          // 采样率（例如 200k）
} NCO_LUT_t;

/* 正弦 LUT：运行时生成一次 */
extern float nco_sin_lut[NCO_LUT_SIZE];

/* 生成 LUT（在 main/init 的时候调用一次即可） */
void NCO_LUT_Generate(void);

/* NCO 通用接口 */
void NCO_LUT_Init(NCO_LUT_t *nco, float fs, float f0, float phase_offset_rad);
void NCO_LUT_SetFreq(NCO_LUT_t *nco, float f0);
void NCO_LUT_SetPhaseOffset(NCO_LUT_t *nco, float phase_offset_rad);

/* inline 步进函数（查表 + 累加相位） */
static inline void NCO_LUT_Step(NCO_LUT_t *nco, float *s, float *c)
{
    uint32_t idx     = nco->phase >> NCO_INDEX_SHIFT;
    uint32_t idx_cos = (idx + (NCO_LUT_SIZE / 4)) & (NCO_LUT_SIZE - 1); // +90°

    *s = nco_sin_lut[idx];
    *c = nco_sin_lut[idx_cos];

    nco->phase += nco->phase_step;
}

#endif /* NCO_LUT_H */
