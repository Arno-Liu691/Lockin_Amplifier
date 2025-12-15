/* buttons.c */
#include "buttons.h"
#include "lia_config.h"
#include "main.h"

/* 四个按键的去抖结构体 */
static DebounceButton btn_plus;
static DebounceButton btn_minus;
static DebounceButton btn_gain;
//static DebounceButton btn_reset;

/* 事件标志位：在别的模块里定义，在这里只声明 */
extern volatile uint8_t g_bw_plus_event;
extern volatile uint8_t g_bw_minus_event;
extern volatile uint8_t g_gain_event;
//extern volatile uint8_t g_reset_event;

void Buttons_Init(void)
{
    Debounce_Init(&btn_plus,  BW_PLUS_GPIO_Port,  BW_PLUS_Pin);
    Debounce_Init(&btn_minus, BW_MINUS_GPIO_Port, BW_MINUS_Pin);
    Debounce_Init(&btn_gain,  GAIN_GPIO_Port,    GAIN_Pin);
//    Debounce_Init(&btn_reset, RESET_GPIO_Port,   RESET_Pin);

    /* 清事件标志，避免上电残留 */
    g_bw_plus_event  = 0;
    g_bw_minus_event = 0;
    g_gain_event     = 0;
//    g_reset_event    = 0;
}

/* 在具体 EXTI IRQ Handler 中，HAL_GPIO_EXTI_IRQHandler()
 * 回调 HAL_GPIO_EXTI_Callback() 时会把对应的 GPIO_Pin 传进来，
 * 这里根据 Pin 分发到对应的 DebounceButton。
 */
void Buttons_OnExti(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == BW_PLUS_Pin) {
        Debounce_OnExtiIRQ(&btn_plus);
    } else if (GPIO_Pin == BW_MINUS_Pin) {
        Debounce_OnExtiIRQ(&btn_minus);
    } else if (GPIO_Pin == GAIN_Pin) {
        Debounce_OnExtiIRQ(&btn_gain);
    }
//      else if (GPIO_Pin == RESET_Pin) {
//        Debounce_OnExtiIRQ(&btn_reset);
//    }
}

/* 主循环调用：把“稳定状态 + 边沿检测”转成一次性的事件标志位 */
void Buttons_Service(void)
{
    /* 你的按键是上拉，按下接地：active_low = 1 */
    if (Debounce_PollPressed(&btn_plus, 1u)) {
        g_bw_plus_event = 1u;
    }

    if (Debounce_PollPressed(&btn_minus, 1u)) {
        g_bw_minus_event = 1u;
    }

    if (Debounce_PollPressed(&btn_gain, 1u)) {
        g_gain_event = 1u;
    }
//
//    if (Debounce_PollPressed(&btn_reset, 1u)) {
//        g_reset_event = 1u;
//    }
}

/* HAL 的通用 EXTI 回调：放在这里统一转到 Buttons_OnExti() */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    Buttons_OnExti(GPIO_Pin);
}
