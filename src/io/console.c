#include <console.h>
#include <ports.h>
#include <stdarg.h> 


size_t row;
size_t column;
uint8_t color;
static uint16_t* buffer;


void update_cursor(){
    uint16_t pos = row * SCREEN_WIDTH + column;
 
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}


void cset_color(uint8_t colfg, uint8_t colbg){
    color = colfg | colbg << 4;
}


void cinit(void) {
    row = 0;
    column = 0;
    color = COLOR_LIGHT_GREY | COLOR_BLACK << 4;
    buffer = (uint16_t*) 0xC03FF000;
    for (size_t y = 0; y < SCREEN_HEIGHT; y++){
        for (size_t x = 0; x < SCREEN_WIDTH; x++){
            const size_t index = y * SCREEN_WIDTH + x;
            buffer[index] = (uint16_t) ' ' | (uint16_t) color << 8;
        }
    }
}


void video_scroll() {
    if (row > 24){
        for (int i = 0*SCREEN_WIDTH; i < 24*SCREEN_WIDTH; i++) {
            buffer[i] = buffer[i + SCREEN_WIDTH];
        }
        // The last line should now be blank. Do this by writing
        // 80 spaces to it.
        for (int i = 24 * SCREEN_WIDTH; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++) {
            buffer[i] = (uint16_t) ' ' | (uint16_t) color << 8;
        }
        // The cursor should now be on the last line.
        row = 24;
    }
    update_cursor();
}


void cputch(char c){
    const size_t index = row * SCREEN_WIDTH + column;
    if(c != '\n') buffer[index] = (uint16_t) c | (uint16_t) color << 8;
    if (++column == SCREEN_WIDTH || c == '\n') {
        column = 0;
        if (++row >= SCREEN_HEIGHT){
			video_scroll();
        }
    }
    update_cursor();
}


void cputs(char *c){
    for(int i=0; c[i]!='\0';i++)
        cputch(c[i]);
}


void cputint(int i){
    if(i < 0){
        i = -i;
        cputch('-');
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
    cputs(str);
}

void cputhex(uint32_t i){
    const unsigned char hex[16]  =  { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    unsigned int n, d = 0x10000000;
    cputs("0x");
    while((i / d == 0) && (d >= 0x10)){
        d /= 0x10;
    }
    n = i;

    while(d >= 0xF){
        cputch(hex[n / d]);
        n = n % d;
        d /= 0x10;
    }

    cputch(hex[n]);
}


void cbackspace(){
    column -= 1;
    const size_t index = row * SCREEN_WIDTH + column;
    buffer[index] = (uint16_t) ' ' | (uint16_t) color << 8;
    cputch(' ');
    column -= 1;
    update_cursor();
}

void log(char *str, bool ok){
    cputs("[");
    cset_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
    if(ok) cputs("OK");
    else {
        cset_color(COLOR_LIGHT_RED, COLOR_BLACK);
        cputs("ERR");
    }
    cset_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    cputs("] ");
    cputs(str);
}

void cprintf(char* format, ...){ 
    va_list arg; 
    va_start(arg, format); 
    for(int i = 0; format[i]; i++){
        if(format[i] == '%'){
            i++;
            switch(format[i]) { 
                case 'c':
                    cputch(va_arg(arg, int));
                    break; 
                case 'd':
                    cputint(va_arg(arg, int));
                    break; 
                case 's':
                    cputs(va_arg(arg, char*)); 
                    break; 
                case 'x':
                    cputhex(va_arg(arg, uint32_t));
                    break;
            }  
        } else {
            cputch(format[i]);
        }
    } 
    va_end(arg); 
} 