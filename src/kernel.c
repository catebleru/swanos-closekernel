#include <kheap.h>
#include <console.h>
#include <gdt.h>
#include <idt.h>
#include <irq.h>
#include <isr.h>
#include <kernel.h>
#include <shell.h>
#include <pmm.h>
#include <vmm.h>

void kernel_main(struct multiboot_info *mboot_info){
	cinit();
	cprintf("Multiboot magic number - %x\n", mboot_info);
	gdt_init();
	idt_init();
	irq_init();
	isrs_init();
	pmm_init(mboot_info);
	vmm_init();
	kheap_init();
	shell_init();
}
