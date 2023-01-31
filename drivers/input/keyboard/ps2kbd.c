#include <drivers/input/keyboard/ps2kbd.h>
#include <arch/x86/ports.h>

int ps2kbd_getkey(){
	if(inb(0x64) & 1) return inb(KB_DATA_PORT);
	return 0;
}

char ps2kbd_kintochar(int scancode){
	switch(scancode){
		case 0x1C:
			return '\n';
		case 0x39:
			return ' ';
		case 0xE:
			return '\r';
		case 0xB4:
			return '.';
		case 0xB5:
			return '/';
		case 0x29:
			return '0';
	}
	if(scancode >= 0x2 && scancode <= 0xB)
		return "1234567890"[scancode - 0x2];
	if(scancode >= 0x10 && scancode <= 0x1C)
		return "qwertyuiop"[scancode - 0x10];
	if(scancode >= 0x1E && scancode <= 0x26)
		return "asdfghjkl"[scancode - 0x1E];
	if(scancode >= 0x2C && scancode <= 0x32)
		return "zxcvbnm"[scancode - 0x2C];
	return '\0';
}

int ps2kbd_iskeypressed(int key){
	if(inb(0x64) & 1)
		if(inb(KB_DATA_PORT) == key) return 1;
	return 0;
}
