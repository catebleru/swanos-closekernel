#include <drivers/usb/serial/serial.h>
#include <arch/x86/ports.h>
#include <stdarg.h>

int serial_init(){
	outb(0x00, COM1_PORT + 1);
	outb(0x80, COM1_PORT + 3);
	outb(0x03, COM1_PORT + 0);
	outb(0x00, COM1_PORT + 1);
	outb(0x03, COM1_PORT + 3);
	outb(0xC7, COM1_PORT + 2);
	outb(0x0B, COM1_PORT + 4);
	outb(0x1E, COM1_PORT + 4);
	outb(0xAE, COM1_PORT + 0);

	if(inb(COM1_PORT + 0) != 0xAE){
		return 1;
	}

	outb(0x0F, COM1_PORT + 4);
	return 0;
}

int serial_received(){
	return inb(COM1_PORT + 5) & 1;
}

char read_serial(){
	while (serial_received() == 0);

	return inb(COM1_PORT);
}

int is_transmit_empty(){
	return inb(COM1_PORT + 5) & 0x20;
}

void serial_write(char ch){
	while (is_transmit_empty() == 0);

	outb(ch, COM1_PORT);
}

void serial_putstr(char *str){
	for(int i = 0; str[i] != '\0'; i++)
		serial_write(str[i]);
}

void serial_putint(int i){
    if(i < 0){
        i = -i;
        serial_write('-');
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
    serial_putstr(str);
}

void serial_puthex(uint32_t i){
	const uint8_t hex[16] = "0123456789ABCDEF";
	uint32_t n, d = 0x10000000;

	serial_putstr("0x");

	while((i / d == 0) && (d >= 0x10)){
		d /= 0x10;
	}
	n = i;

	while(d >= 0xF) {
        serial_write(hex[n / d]);
        n = n % d;
        d /= 0x10;
    }

    serial_write(hex[n]);
}

void serial_printf(char *fmt, ...){
    va_list args;
    va_start(args, fmt);

    for(int i = 0; fmt[i]; i++){
        if(fmt[i] == '%'){
            i++;
            switch(fmt[i]){
                case 's':
                    serial_putstr(va_arg(args, char*));
                    break;
                case 'c':
                    serial_write(va_arg(args, int));
                    break;
                case 'd':
                    serial_putint(va_arg(args, int));
                    break;
                case 'x':
					serial_puthex(va_arg(args, uint32_t));
					break;
            }
        }
        else {
            serial_write(fmt[i]);
        }
    }
}

