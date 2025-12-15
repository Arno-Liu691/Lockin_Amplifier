/* buttons.h */
#ifndef BUTTONS_H
#define BUTTONS_H

#include "debounce.h"

void Buttons_Init(void);

/* EXTI 中断处理函数中要调用的接口 */
void Buttons_OnExti(uint16_t GPIO_Pin);

/* 主循环中调用，更新 g_bw_plus_event / g_bw_minus_event 等 */
void Buttons_Service(void);

#endif /* BUTTONS_H */
