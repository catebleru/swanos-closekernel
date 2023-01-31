DIR=~/i686-elf-tools/bin/
LIMINE_DIR=~/.limine/
CC=i686-elf-gcc
KERNEL=kernel.elf

CFLAGS_X86=-ffreestanding -Wall -g -Wextra -O0 -I include/ -g -c
TARGET_X86=i386-pc-none-elf
CC_X86=clang -target $(TARGET_X86)
LDFLAGS_X86=-T arch/x86/link.ld -nostdlib -O0
OBJECTS_X86=bin/pcspkr.o bin/ps2kbd.o bin/cmos.o bin/serial.o bin/string.o bin/kernel.o bin/video.o bin/shell.o bin/boot.o bin/ports.o bin/idt.o bin/gdt.o bin/description_tables.o bin/irq.o bin/interrupts.o
build-x86:
	$(CC_X86) $(CFLAGS_X86) drivers/input/misc/pcspkr.c -o bin/pcspkr.o
	$(CC_X86) $(CFLAGS_X86) drivers/input/keyboard/ps2kbd.c -o bin/ps2kbd.o
	$(CC_X86) $(CFLAGS_X86) drivers/rtc/cmos.c -o bin/cmos.o
	$(CC_X86) $(CFLAGS_X86) drivers/usb/serial/serial.c -o bin/serial.o
	$(CC_X86) $(CFLAGS_X86) libc/string.c -o bin/string.o
	$(CC_X86) $(CFLAGS_X86) arch/x86/kernel/idt.c -o bin/idt.o
	$(CC_X86) $(CFLAGS_X86) arch/x86/kernel/gdt.c -o bin/gdt.o
	$(CC_X86) $(CFLAGS_X86) arch/x86/kernel/description_tables.s -o bin/description_tables.o
	$(CC_X86) $(CFLAGS_X86) arch/x86/kernel/irq.c -o bin/irq.o
	$(CC_X86) $(CFLAGS_X86) arch/x86/kernel/interrupts.s -o bin/interrupts.o
	$(CC_X86) $(CFLAGS_X86) arch/x86/boot.s -o bin/boot.o
	$(CC_X86) $(CFLAGS_X86) arch/x86/ports.c -o bin/ports.o
	$(CC_X86) $(CFLAGS_X86) arch/x86/kernel/kernel.c -o bin/kernel.o
	$(CC_X86) $(CFLAGS_X86) arch/x86/kernel/video/video.c -o bin/video.o
	$(CC_X86) $(CFLAGS_X86) arch/x86/kernel/shell/shell.c -o bin/shell.o
	$(CC_X86) $(LDFLAGS_X86) -o $(KERNEL) $(OBJECTS) $(OBJECTS_X86)

CFLAGS_RISCV=-ffreestanding -Wall -g -Wextra -O0 -I include/ -g -c -mcmodel=medany
CC_RISCV=riscv64-unknown-elf-gcc
LDFLAGS_RISCV=-T arch/riscv/link.ld -nostdlib -O0
OBJECTS_RISCV = bin/boot.o bin/kernel.o bin/uart.o bin/string.o
build-riscv:
	$(CC_RISCV) $(CFLAGS_RISCV) libc/string.c -o bin/string.o
	$(CC_RISCV) $(CFLAGS_RISCV) drivers/usb/uart/uart.c -o bin/uart.o
	$(CC_RISCV) $(CFLAGS_RISCV) arch/riscv/boot.s -o bin/boot.o
	$(CC_RISCV) $(CFLAGS_RISCV) arch/riscv/kernel/kernel.c -o bin/kernel.o
	$(CC_RISCV) $(LDFLAGS_RISCV) -o $(KERNEL) $(OBJECTS) $(OBJECTS_RISCV)

build-iso:
	cp -v kernel.elf isodir/
	xorriso -as mkisofs -b limine-cd.bin \
              -no-emul-boot -boot-load-size 4 -boot-info-table \
              --efi-boot limine-cd-efi.bin \
              -efi-boot-part --efi-boot-image --protective-msdos-label \
              isodir -o latest.iso

	$(LIMINE_DIR)limine-deploy latest.iso

run-iso:
	qemu-system-i386 -m 512 -cdrom latest.iso -monitor stdio -serial file:serial.log -soundhw pcspk

run-x86:
	qemu-system-i386 -m 512 -kernel kernel.elf -monitor stdio -serial file:serial.log -soundhw pcspk

run-riscv:
	qemu-system-riscv64 -machine virt -bios kernel.elf -serial file:serial.log
