#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

size_t strlen(const char* str);
char *strchr(const char *str, const char c);
char *strtok(char *str, const char *div);
int strcmp(char *str, char *str2);

void *memset(void *destination, int c, size_t n);
void *memcpy(void *destination, const void *source, size_t n);

#endif
