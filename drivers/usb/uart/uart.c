#include <stdint.h>
#include <drivers/usb/uart/uart.h>

uint8_t *uart = (uint8_t*)COM1_PORT;

void uart_write(char c){
	*uart = c;
}
 
void uart_putstr(const char *str){
	for(;*str != '\0'; str++)
		uart_write(*str);
}

void uart_putint(int i){
    if(i < 0){
        i = -i;
        uart_write('-');
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
    uart_putstr(str);
}

void uart_puthex(uint32_t i){
	const uint8_t hex[16] = "0123456789ABCDEF";
	uint32_t n, d = 0x10000000;

	uart_putstr("0x");

	while((i / d == 0) && (d >= 0x10)){
		d /= 0x10;
	}
	n = i;

	while(d >= 0xF) {
        uart_write(hex[n / d]);
        n = n % d;
        d /= 0x10;
    }

    uart_write(hex[n]);
}

void uart_printf(char *fmt, ...){
    va_list args;
    va_start(args, fmt);

    for(int i = 0; fmt[i]; i++){
        if(fmt[i] == '%'){
            i++;
            switch(fmt[i]){
                case 's':
                    uart_putstr(va_arg(args, char*));
                    break;
                case 'c':
                    uart_write(va_arg(args, int));
                    break;
                case 'd':
                    uart_putint(va_arg(args, int));
                    break;
                case 'x':
					uart_puthex(va_arg(args, uint32_t));
					break;
            }
        }
        else {
            uart_write(fmt[i]);
        }
    }
}