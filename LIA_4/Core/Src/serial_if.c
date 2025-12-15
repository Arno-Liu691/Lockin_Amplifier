/* serial_if.c */
#include "serial_if.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void Serial_Print(const char *s)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)s, (uint16_t)strlen(s), HAL_MAX_DELAY);
}

void Serial_Printf(const char *fmt, ...)
{
    char buf[128];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    Serial_Print(buf);
}

int Serial_ReadLine(char *buf, uint32_t maxlen)
{
    uint32_t idx = 0;
    while (1) {
        uint8_t ch;
        if (HAL_UART_Receive(&huart2, &ch, 1, HAL_MAX_DELAY) != HAL_OK) {
            return -1;
        }
        if (ch == '\r' || ch == '\n') {
            if (idx == 0) {
                // 忽略空行
                continue;
            }
            break;
        }
        if (idx < maxlen - 1) {
            buf[idx++] = (char)ch;
        }
    }
    buf[idx] = '\0';
    return (int)idx;
}

int Serial_ReadFloat(const char *prompt, float *out_val)
{
    char buf[32];
    Serial_Print(prompt);
    Serial_Print("\r\n> ");
    int n = Serial_ReadLine(buf, sizeof(buf));
    if (n <= 0) return -1;
    *out_val = (float)atof(buf);
    return 0;
}
