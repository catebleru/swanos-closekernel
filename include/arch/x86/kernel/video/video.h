#ifndef	_VIDEO_H
#define	_VIDEO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SCREEN_WIDTH	80
#define SCREEN_HEIGHT	25

enum color_table {
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN  = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
        COLOR_LIGHT_BLUE = 9,
        COLOR_LIGHT_GREEN = 10,
        COLOR_LIGHT_CYAN = 11,
        COLOR_LIGHT_RED = 12,
        COLOR_LIGHT_MAGENTA = 13,
        COLOR_LIGHT_BROWN = 14,
        COLOR_WHITE = 15,
};

void video_init(void);
void video_delete_symbol();
void video_putchar(char ch, uint16_t color);
void video_putstr(char *str);
void video_putint(int i);
void video_puthex(uint32_t i);
void video_printf(char *fmt, ...);
#endif
