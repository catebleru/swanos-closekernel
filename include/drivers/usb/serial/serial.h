#ifndef _SERIAL_H
#define _SERIAL_H

#include <stdint.h>

#define COM1_PORT 0x3f8

int serial_init();
int serial_received();
char read_serial();
int is_transmit_empty();
void serial_write(char ch);
void serial_putstr(char *str);
void serial_putint(int i);
void serial_puthex(uint32_t i);
void serial_printf(char *fmt, ...);

#endif
