#include <console.h>
#include <gdt.h>
#include <idt.h>
#include <irq.h>
#include <isr.h>
#include <kernel.h>
#include <ports.h>

void kernel_main(struct multiboot_info *mboot_info){
	qemu_log("Multiboot magic number - %x\n", mboot_info);
	cinit();
	gdt_init();
	idt_init();
	irq_init();
	isrs_init();
}
