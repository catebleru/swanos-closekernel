/* Declare constants for the multiboot header. */
.set ALIGN,    1 << 0             /* align loaded modules on page boundaries */
.set MEMINFO,  1 << 1             /* provide memory map */
.set VBE_MODE,  1 << 2
.set FLAGS,    ALIGN | MEMINFO | VBE_MODE  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */
 
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
 

.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 65536  /* 64 KiB for the stack old: 16384*/
stack_top:
 

.set KERNEL_VIRTUAL_BASE, 0xC0000000
.set KERNEL_PAGE_NUMBER, (KERNEL_VIRTUAL_BASE >> 22)

.section .data
.align 0x1000
.global _boot_page_directory
_boot_page_directory:
.set i, 0x00000083
.rept 0x300
    .long i
    .set i, i+0x00400000
.endr

.long 0x00000083

.set i, 0xc0400083
.rept 0xFF # 0xFF = (0xffc00000 - 0xc0400000)/0x00400000 + 1
    .long i
    .set i, i+0x00400000
.endr

.section .text
.global _loader
_loader:
  mov $(_boot_page_directory - KERNEL_VIRTUAL_BASE), %ecx
  mov $(_boot_page_directory - KERNEL_VIRTUAL_BASE), %edx
  or $0x00000003, %ecx
  mov %ecx, 0xFFC(%edx) # bpd + 4092 i.e we write address of page_dir|0000003 to last pde

  # Load Page Directory Base Register. Until paging is set up, the code must
  # be position-independent and use physical addresses, not virtual ones
  mov $(_boot_page_directory - KERNEL_VIRTUAL_BASE), %ecx
  mov %ecx, %cr3

  # Set PSE bit in CR4 to enable 4MB pages.
  mov %cr4, %ecx
  or $0x00000010, %ecx
  mov %ecx, %cr4

  # Set PG bit in CR0 to enable paging.
  mov %cr0, %ecx
  or $0x80000000, %ecx
  mov %ecx, %cr0

  movl $_start, %edx
  jmp *%edx

.global _start
.type _start, @function
_start:

  	mov %cr0, %eax
  	or $1, %al
  	mov %eax, %cr0

	mov $stack_top, %esp

	call kernel

	cli

1:	hlt

	jmp 1b

.size _start, . - _start