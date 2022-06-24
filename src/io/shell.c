#include <shell.h>
#include <keyboard.h>
#include <console.h>
#include <ports.h>
#include <string.h>
#define SIZE_BUFFER 77

char buffer_shell[SIZE_BUFFER];

char *get_command(){
    int key = kb_getkey();
    cset_color(COLOR_CYAN, COLOR_BLACK);
    cputs("\n~$ ");
    cset_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    for(int i=0; i!=sizeof(buffer_shell); i++) buffer_shell[i] = '\0'; // clear buffer
    while(kb_sctochar(key) != '\n'){
        if(strlen(buffer_shell) == SIZE_BUFFER){ // checking if the buffer overflows...
            log("buffer full\n", false);         // ...temporary until we write a memory manager
            return "";
        }
        if(kb_sctochar(key) != 0){ 
            buffer_shell[strlen(buffer_shell)] = kb_sctochar(key);
            cputch(kb_sctochar(key));
        }
        key = kb_getkey();
    }
    return buffer_shell;
}

void shell_init(){
    for(;;){
        char *command = get_command();
        if(strcmp(command, "help")==0){
            cputs("\nhere all commands are output inside in the kernel: \n\
help - all commands are output inside to the kernel\n");
        } else if(strcmp(command, "reboot")==0){
            uint8_t good = 0x02;
            while(good & 0x02) good = inb(0x64);
            outb(0x64, 0xFE);
            asm volatile("hlt");
        } else if(strcmp(command, "shutdown")==0){
            outb(0x64, 0xFE);
            outw(0xB004, 0x2000);
            outw(0x604, 0x2000);
            outw(0x4004, 0x3400);
        }
    }
}