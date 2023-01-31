#ifndef _SHELL_H
#define _SHELL_H

#include <stdint.h>

#define KBDPS2 0x01

#define SHELL_SPECIAL_SYMBOL '$'
#define BUFFER_SIZE 500 // TODO: malloc for give memory to buffer

void shell_init(uint32_t keyboard_address);
void shell_command(char *command);

#endif