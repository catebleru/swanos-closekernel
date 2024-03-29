.macro ISR_NOERR isr_num
.global    _isr\isr_num

_isr\isr_num:

    cli
    push    $0    
    push    $\isr_num
    jmp    isr_common_stub
    
.endm

.macro ISR_ERR isr_num
.global    _isr\isr_num

_isr\isr_num:

    cli
    push    $\isr_num
    jmp    isr_common_stub
    
.endm

.macro IRQ_ENTRY irq_num, isr_num
.global _irq\irq_num

_irq\irq_num:
    
    cli
    push    $0
    push    $\isr_num
    jmp    irq_common_stub  

.endm

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31
ISR_NOERR 127

IRQ_ENTRY 0, 32
IRQ_ENTRY 1, 33
IRQ_ENTRY 2, 34
IRQ_ENTRY 3, 35
IRQ_ENTRY 4, 36
IRQ_ENTRY 5, 37
IRQ_ENTRY 6, 38
IRQ_ENTRY 7, 39
IRQ_ENTRY 8, 40
IRQ_ENTRY 9, 41
IRQ_ENTRY 10, 42
IRQ_ENTRY 11, 43
IRQ_ENTRY 12, 44
IRQ_ENTRY 13, 45
IRQ_ENTRY 14, 46
IRQ_ENTRY 15, 47

.extern fault_handler
isr_common_stub:
	pusha
	push %ds
	push %es
	push %fs
	push %gs
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %esp, %eax
	push %eax
	call fault_handler
	pop %eax
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa
	add $8, %esp
	iret

.extern irq_handler
irq_common_stub:
	pusha
	push %ds
	push %es
	push %fs
	push %gs
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %esp, %eax
	push %eax
	call irq_handler
	pop %eax
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa
	add $8, %esp
	iret

