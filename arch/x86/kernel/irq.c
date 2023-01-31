// IRQ & ISR 

#include <arch/x86/kernel/irq.h>
#include <arch/x86/kernel/video/video.h>
#include <arch/x86/regs.h>
#include <arch/x86/ports.h>
#include <drivers/usb/serial/serial.h>
#include <arch/x86/kernel/idt.h>

static irq_handler_t irq_routines[16] = { NULL };

static irq_handler_t isrs_routines[32] = { NULL };

void irq_install_handler(int irq, irq_handler_t handler){
	irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq){
	irq_routines[irq] = 0;
}

void isrs_install_handler(int isrs, irq_handler_t handler){
	isrs_routines[isrs] = handler;
}

void isrs_uninstall_handler(int isrs){
	isrs_routines[isrs] = 0;
}

void irq_remap(){
	outb(0x11, 0x20);
	outb(0x11, 0xA0);
	outb(0x20, 0x21);
	outb(0x28, 0xA1);
	outb(0x04, 0x21);
	outb(0x02, 0xA1);
	outb(0x01, 0x21);
	outb(0x01, 0xA1);
	outb(0x0, 0x21);
	outb(0x0, 0xA1);
}

void irq_gates(){
	idt_set_gate(32, (uint32_t)_irq0, 0x08, 0x8E);
	idt_set_gate(33, (uint32_t)_irq1, 0x08, 0x8E);
	idt_set_gate(34, (uint32_t)_irq2, 0x08, 0x8E);
	idt_set_gate(35, (uint32_t)_irq3, 0x08, 0x8E);
	idt_set_gate(36, (uint32_t)_irq4, 0x08, 0x8E);
	idt_set_gate(37, (uint32_t)_irq5, 0x08, 0x8E);
	idt_set_gate(38, (uint32_t)_irq6, 0x08, 0x8E);
	idt_set_gate(39, (uint32_t)_irq7, 0x08, 0x8E);
	idt_set_gate(40, (uint32_t)_irq8, 0x08, 0x8E);
	idt_set_gate(41, (uint32_t)_irq9, 0x08, 0x8E);
	idt_set_gate(42, (uint32_t)_irq10, 0x08, 0x8E);
	idt_set_gate(43, (uint32_t)_irq11, 0x08, 0x8E);
	idt_set_gate(44, (uint32_t)_irq12, 0x08, 0x8E);
	idt_set_gate(45, (uint32_t)_irq13, 0x08, 0x8E);
	idt_set_gate(46, (uint32_t)_irq14, 0x08, 0x8E);
	idt_set_gate(47, (uint32_t)_irq15, 0x08, 0x8E);
}

void irq_init(){
	irq_remap();
	irq_gates();
	serial_printf("[INFO] IRQ init\n");
	__asm__ __volatile__("sti");
}

void irq_test(){
	video_printf("[test] IRQ exception(0x6) = ");
	serial_printf("[test] IRQ exception = ");
	asm("int $0x6");
}

void irq_ack(int irq_no){
	if(irq_no >= 12){
		outb(0x20, 0xA0);
	}
	outb(0x20, 0x20);
}

void irq_handler(struct regs *r){
	__asm__ __volatile__("cli");
	void (*handler)(struct regs *r);
	if(r->int_no >= 47 || r->int_no <= 32){
		handler = NULL;
	} else {
		handler = irq_routines[r->int_no - 32];
	}
	if(handler){
		handler(r);
	} else {
		irq_ack(r->int_no - 32);
	}
	__asm__ __volatile__("sti");
}

void isrs_init(){
	idt_set_gate(0, (uint32_t)_isr0, 0x08, 0x8E);
	idt_set_gate(1, (uint32_t)_isr1, 0x08, 0x8E);
	idt_set_gate(2, (uint32_t)_isr2, 0x08, 0x8E);
	idt_set_gate(3, (uint32_t)_isr3, 0x08, 0x8E);
	idt_set_gate(4, (uint32_t)_isr4, 0x08, 0x8E);
	idt_set_gate(5, (uint32_t)_isr5, 0x08, 0x8E);
	idt_set_gate(6, (uint32_t)_isr6, 0x08, 0x8E);
	idt_set_gate(7, (uint32_t)_isr7, 0x08, 0x8E);
	idt_set_gate(8, (uint32_t)_isr8, 0x08, 0x8E);
	idt_set_gate(9, (uint32_t)_isr9, 0x08, 0x8E);
	idt_set_gate(10, (uint32_t)_isr10, 0x08, 0x8E);
	idt_set_gate(11, (uint32_t)_isr11, 0x08, 0x8E);
	idt_set_gate(12, (uint32_t)_isr12, 0x08, 0x8E);
	idt_set_gate(13, (uint32_t)_isr13, 0x08, 0x8E);
	idt_set_gate(14, (uint32_t)_isr14, 0x08, 0x8E);
	idt_set_gate(15, (uint32_t)_isr15, 0x08, 0x8E);
	idt_set_gate(16, (uint32_t)_isr16, 0x08, 0x8E);
	idt_set_gate(17, (uint32_t)_isr17, 0x08, 0x8E);
	idt_set_gate(18, (uint32_t)_isr18, 0x08, 0x8E);
	idt_set_gate(19, (uint32_t)_isr19, 0x08, 0x8E);
	idt_set_gate(20, (uint32_t)_isr20, 0x08, 0x8E);
	idt_set_gate(21, (uint32_t)_isr21, 0x08, 0x8E);
	idt_set_gate(22, (uint32_t)_isr22, 0x08, 0x8E);
	idt_set_gate(23, (uint32_t)_isr23, 0x08, 0x8E);
	idt_set_gate(24, (uint32_t)_isr24, 0x08, 0x8E);
	idt_set_gate(25, (uint32_t)_isr25, 0x08, 0x8E);
	idt_set_gate(26, (uint32_t)_isr26, 0x08, 0x8E);
	idt_set_gate(27, (uint32_t)_isr27, 0x08, 0x8E);
	idt_set_gate(28, (uint32_t)_isr28, 0x08, 0x8E);
	idt_set_gate(29, (uint32_t)_isr29, 0x08, 0x8E);
	idt_set_gate(30, (uint32_t)_isr30, 0x08, 0x8E);
	idt_set_gate(31, (uint32_t)_isr31, 0x08, 0x8E);
	idt_set_gate(0x7F, (uint32_t)_isr127, 0x08, 0x8E);
	serial_printf("[INFO] ISR init\n");
}

void fault_handler(struct regs *r){
	if (r->int_no == 8) {
		while(1) __asm__ __volatile__ ("hlt");
	}
	if (r->int_no >= 32 && r->int_no != 0x7F) {
		while(1) __asm__ __volatile__ ("hlt");
	}
	__asm__ __volatile__ ("cli");
	void (*handler)(struct regs *r);
	handler = isrs_routines[r->int_no];
	if (handler) {
		handler(r);
	} else {
		switch(r->int_no){
			case 0x0:
				video_printf("[INT: %x] Divide-by-zero Error\n", r->int_no);
				serial_printf("[INT: %x] Divide-by-zero Error\n", r->int_no);
				break;
			case 0x6:
				video_printf("[INT: %x] Invalid Opcode\n", r->int_no);
				serial_printf("[INT: %x] Invalid Opcode\n", r->int_no);
				break;
			case 0x8:
				video_printf("[INT: %x] Double Fault Exception\n", r->int_no);
				serial_printf("[INT: %x] Double Fault Exception\n", r->int_no);
				break;
			case 0xE:
				video_printf("[INT: %x] Page Fault Exception\n", r->int_no);
				serial_printf("[INT: %x] Page Fault Exception\n", r->int_no);
				break;
			default:
				video_printf("[INT: %x] Unknown Exception\n", r->int_no);
				serial_printf("[INT: %x] Unknown Exception\n", r->int_no);
				break;
		}
	}
	__asm__ __volatile__ ("sti");
}
