#ifndef _PS2KBD_H
#define _PS2KBD_H

#define KB_DATA_PORT  0x60

#include <stdint.h>

int ps2kbd_getkey();
char ps2kbd_kintochar(int scancode);
int ps2kbd_iskeypressed(int key);

#endif
