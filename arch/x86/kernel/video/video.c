#include <arch/x86/kernel/video/video.h>
#include <stdarg.h>

size_t position[2] = {0, 0};
uint8_t   video_color;
uint16_t* video_buffer;

void video_init(void){
	video_color = COLOR_LIGHT_GREY | COLOR_BLACK << 4;
	video_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < SCREEN_HEIGHT; y++) {
		for (size_t x = 0; x < SCREEN_WIDTH; x++) {
			const size_t index = y * SCREEN_WIDTH + x;
			video_buffer[index] = (uint16_t) ' ' | (uint16_t) video_color << 8;
		}
	}
}

void video_update_cursor(void){
	const size_t index = position[1] * SCREEN_WIDTH + position[0];
	outb(0x0F, 0x3D4);
    outb((uint8_t) (index & 0xFF), 0x3D5);
    outb(0x0E, 0x3D4);
    outb((uint8_t) ((index >> 8) & 0xFF), 0x3D5);
}

void video_delete_symbol(){
	position[0] -= 1;
	const size_t index = position[1] * SCREEN_WIDTH + position[0];
	video_buffer[index] = (uint16_t) '\0' | (uint16_t) video_color << 8;
	video_update_cursor();
}

void video_scroll() {
    if (position[1] > 24){
        for(int i = 0*SCREEN_WIDTH; i < 24 * SCREEN_WIDTH; i++){
            video_buffer[i] = video_buffer[i + SCREEN_WIDTH];
        }
        for(int i = 24 * SCREEN_WIDTH; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++){
            video_buffer[i] = (uint16_t) '\0' | (uint16_t) video_color << 8;
        }
        position[1] = 24;
    }
    video_update_cursor();
}

void video_putchar(char ch, uint16_t color){
	const size_t index = position[1] * SCREEN_WIDTH + position[0];
	if (position[0] == SCREEN_WIDTH || ch == '\n'){
		position[0]  = 0;
		if (++position[1] >= SCREEN_HEIGHT){
			video_scroll();
        }
	} else {
		video_buffer[index] = (uint16_t) ch | (uint16_t) color << 8;
        	position[0]++;
	}
	video_update_cursor();
}

void video_putstr(char *str){
	for(int i = 0; str[i] != '\0'; i++)
		video_putchar(str[i], video_color);
}

void video_putint(int i){
	if(i < 0){
		i = -i;
		video_putchar('-', video_color);
	}
	char str[255];
	uint32_t n, d = 1000000000;
	uint32_t dec_index = 0;

	while((i / d == 0) && (d >= 10)){
		d /= 10;
	}
	n = i;

	while(d >= 10){
		str[dec_index++] = ((char)((int) '0' + n/d));
		n = n % d;
		d /= 10;
	}

	str[dec_index++] = ((char) ((int) '0' + n));
	str[dec_index] = 0;
	video_putstr(str);
}

void video_puthex(uint32_t i){
	const uint8_t hex[16] = "0123456789ABCDEF";
	uint32_t n, d = 0x10000000;

	video_putstr("0x");

	while((i / d == 0) && (d >= 0x10)){
		d /= 0x10;
	}
	n = i;

	while(d >= 0xF) {
        video_putchar(hex[n / d], video_color);
        n = n % d;
        d /= 0x10;
    }

    video_putchar(hex[n], video_color);
}

void video_printf(char *fmt, ...){
	va_list args;
	va_start(args, fmt);

	for(int i = 0; fmt[i]; i++){
		if(fmt[i] == '%'){
			i++;
			switch(fmt[i]){
				case 's':
					video_putstr(va_arg(args, char*));
					break;
				case 'c':
					video_putchar(va_arg(args, int), video_color);
					break;
				case 'd':
					video_putint(va_arg(args, int));
					break;
				case 'x':
					video_puthex(va_arg(args, uint32_t));
					break;
			}
		}
		else {
			video_putchar(fmt[i], video_color);
		}
	}
}
