#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

struct idt_entry_struct {
	uint16_t base_lo;
	uint16_t sel;
	uint8_t  always0;
	uint8_t	 flags;
	uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr_struct {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

void idt_init();
void idt_set_gate(uint32_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif
