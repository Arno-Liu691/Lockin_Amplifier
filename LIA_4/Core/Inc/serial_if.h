/* serial_if.h */
#ifndef SERIAL_IF_H
#define SERIAL_IF_H

#include "lia_config.h"
#include <stdarg.h>

/* 简易 printf 封装 */
void Serial_Print(const char *s);
void Serial_Printf(const char *fmt, ...);

/* 读一行（阻塞），返回长度（不包含 '\0'） */
int Serial_ReadLine(char *buf, uint32_t maxlen);

/* 打印提示，读取 float（阻塞），返回 0 成功，非 0 失败 */
int Serial_ReadFloat(const char *prompt, float *out_val);

#endif /* SERIAL_IF_H */
