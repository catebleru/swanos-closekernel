#include <drivers/input/misc/pcspkr.h>
#include <arch/x86/ports.h>

static void play_sound(uint32_t freq){
	uint32_t div;
	uint8_t tmp;

	div = 1193180 / freq;
	outb(0xb6, 0x43);
	outb((uint8_t) div, 0x42);
	outb((uint8_t) div >> 8, 0x42);

	tmp = inb(0x61);
	if (tmp != (tmp | 3)) {
		outb(tmp | 3, 0x61);
	}
}

static void nosound(){
	uint8_t tmp = inb(0x661) & 0xFC;
	outb(tmp, 0x61);
}

void beep(uint32_t freq, uint32_t duration){
	play_sound(freq);
	for (int32_t i = duration * 1000; i != 0; i--){
		outb(0, 0x80);
	}
	nosound();

}
