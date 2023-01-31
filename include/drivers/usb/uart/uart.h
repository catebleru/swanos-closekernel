#ifndef _UART_H
#define _UART_H

#include <stdarg.h>
#include <stdint.h>

#define COM1_PORT 0x10000000

void uart_write(char c);
void uart_putstr(const char *str);
void uart_putint(int i);
void uart_puthex(uint32_t i);
void uart_printf(char *fmt, ...);

#endif