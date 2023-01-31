#include <arch/x86/kernel/shell/shell.h>
#include <arch/x86/kernel/video/video.h>
#include <drivers/usb/serial/serial.h>
#include <drivers/input/keyboard/ps2kbd.h>
#include <drivers/input/misc/pcspkr.h>
#include <libc/string.h>

char buffer[BUFFER_SIZE];

void shell_init(uint32_t keyboard_address){
    video_printf("%s%c ", "root", SHELL_SPECIAL_SYMBOL);
    char current_char;
    for(;;){
        switch(keyboard_address){
            case KBDPS2:
                current_char = ps2kbd_kintochar(ps2kbd_getkey());
                switch(current_char){
                    case '\n':
                        shell_command(buffer);
                        memset(&buffer, 0, strlen(buffer));
                        video_printf("\n%s%c ", "root", SHELL_SPECIAL_SYMBOL);
                    break;
                    case '\r':
                        if(strlen(buffer) != 0){
                            buffer[strlen(buffer)-1] = '\0';
                            video_delete_symbol();
                        }
                    break;
                    case '\0':
                        break;
                    default:
                        buffer[strlen(buffer)] = current_char;
                        video_printf("%c", current_char);
                }
                break;
            default:
                serial_printf("[INFO] Undefined keyboard address %x",
                    keyboard_address);
                asm __volatile__("hlt");
       }
    }
}

void shell_command(char *command){
    char *args = strtok(command, " ");
    if(strcmp(args, "help") == 0){
        video_printf("\nhelp:\n"
            " [] beep  - triggering a small sound with \"pcspeaker\"\n"
            " [] tests - run kernel tests\n");
    } else if(strcmp(args, "beep") == 0){
        beep(1000, 1000);
    } else if(strcmp(args, "tests") == 0){
        video_printf("\n");
        kernel_test();
    } else {
        video_printf("\n%s: undefined command.\n", args);
    }
}
