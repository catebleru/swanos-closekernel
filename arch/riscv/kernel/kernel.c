#include <stdint.h>
#include <stddef.h>
#include <drivers/usb/uart/uart.h>

void kernel(void) {
	uart_printf("Hello %s%d%c%x\r\n", "world!", 10, 'c', 0x1);
	for(;;);
}