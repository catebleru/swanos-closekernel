#include <arch/x86/kernel/video/video.h>
#include <drivers/input/misc/pcspkr.h>
#include <drivers/input/keyboard/ps2kbd.h>
#include <drivers/rtc/cmos.h>
#include <drivers/usb/serial/serial.h>
#include <arch/x86/kernel/idt.h>
#include <arch/x86/kernel/irq.h>
#include <arch/x86/kernel/gdt.h>
#include <arch/x86/kernel/shell/shell.h>

void kernel_test(void){
	irq_test();
}

void kernel(){
	video_init();
	serial_init();
	video_printf("[TIME] %d:%d\n", cmos_get_hour(), cmos_get_minute());
	serial_printf("[TIME] %d:%d\n", cmos_get_hour(), cmos_get_minute());
	gdt_init();
	idt_init();
	irq_init();
	isrs_init();
	kernel_test();
	shell_init(KBDPS2);
}
