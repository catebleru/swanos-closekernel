#include <ports.h>
#include <stdarg.h> 

inline uint8_t inb(uint16_t port){
	uint8_t ret;
	asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
	return ret;
}

inline uint16_t inw(uint16_t port){
	uint16_t ret;
	asm volatile ( "inw %1, %0" : "=a"(ret) : "Nd"(port) );
	return ret;
}

inline uint32_t inl(uint16_t port){
	uint32_t ret;
	asm volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port) );
	return ret;
}

inline void outb(uint16_t port, uint8_t  val){
	asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

inline void outw(uint16_t port, uint16_t val){
	asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
}

inline void outl(uint16_t port, uint32_t val){
	asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(port) );
}

inline uint64_t rdtsc(){
    uint64_t ret;
    asm volatile ( "rdtsc" : "=A"(ret) );
    return ret;
}

inline void insl(unsigned short port, unsigned int buffer, unsigned long count){
	asm("cld; rep; insl" :: "D" (buffer), "d" (port), "c" (count));
}

void io_wait(void){
    outb(0x80, 0);
}

void sleep(int32_t i){
    for (int32_t j = i * 1000; j != 0; j--){
        io_wait();
    }
}

int32_t com1_is_transmit_empty(){
    return inb(PORT_COM1 + 5) & 0x20;
}

void com1_write_char(char a){
    while (com1_is_transmit_empty() == 0);
    outb(PORT_COM1, a);
}

void qemu_puts(char *c){
    for(int i=0; c[i]!='\0';i++)
        com1_write_char(c[i]);
}


void qemu_putint(int i){
    if(i < 0){
        i = -i;
        com1_write_char('-');
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
    qemu_puts(str);
}

void qemu_puthex(uint32_t i){
    const unsigned char hex[16]  =  { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    unsigned int n, d = 0x10000000;
    qemu_puts("0x");
    while((i / d == 0) && (d >= 0x10)){
        d /= 0x10;
    }
    n = i;

    while(d >= 0xF){
        com1_write_char(hex[n / d]);
        n = n % d;
        d /= 0x10;
    }

    com1_write_char(hex[n]);
}

void qemu_printf(char* format, ...){ 
    va_list arg; 
    va_start(arg, format); 
    for(int i = 0; format[i]; i++){
        if(format[i] == '%'){
            i++;
            switch(format[i]) { 
                case 'c':
                    com1_write_char(va_arg(arg, int));
                    break; 
                case 'd':
                    qemu_putint(va_arg(arg, int));
                    break; 
                case 's':
                    qemu_puts(va_arg(arg, char*)); 
                    break; 
                case 'x':
                    qemu_puthex(va_arg(arg, uint32_t));
                    break;
            }  
        } else {
            com1_write_char(format[i]);
        }
    } 
    va_end(arg); 
}

void qemu_log(char* format, ...){ 
    va_list arg; 
    va_start(arg, format); 
	qemu_printf("[LOG] ");
    for(int i = 0; format[i]; i++){
        if(format[i] == '%'){
            i++;
            switch(format[i]) { 
                case 'c':
                    com1_write_char(va_arg(arg, int));
                    break; 
                case 'd':
                    qemu_putint(va_arg(arg, int));
                    break; 
                case 's':
                    qemu_puts(va_arg(arg, char*)); 
                    break; 
                case 'x':
                    qemu_puthex(va_arg(arg, uint32_t));
                    break;
            }  
        } else {
            com1_write_char(format[i]);
        }
    } 
    va_end(arg); 
}