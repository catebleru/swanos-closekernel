#include <console.h>
#include <gdt.h>
#include <idt.h>
#include <irq.h>
#include <isr.h>
#include <kernel.h>
#include <ports.h>

void kernel_main(uint32_t magic_number, multiboot_info_t* mboot_info){
	qemu_log("Multiboot magic number - %x\n", magic_number);
	cinit();
	gdt_init();
	idt_init();
	irq_init();
	isrs_init();
	qemu_putint(mboot_info->flags);
}
