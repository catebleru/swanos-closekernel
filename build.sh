#!/bin/bash
set -e
OBJECTS="bin/kernel.o bin/kernel_entry.o bin/console.o bin/ports.o bin/string.o bin/gdt.o bin/idt.o bin/interrupts.o bin/pci.o bin/isr.o bin/irq.o bin/keyboard.o bin/description_tables.o bin/pcspkr.o bin/stdlib.o bin/cmos.o"
CC=i686-elf-gcc
AS=i686-elf-as
CFLAGS="-g -I include -ffreestanding -Wall -Wextra -O2 -c"

for param in "$@"
do
  if [ "$param" == "llvm" ]; then
    CC="clang -target i386-pc-none-elf"
    AS=i686-elf-as
  fi
done

if [ ! -x "$(command -v i686-elf-gcc)" ]; then
  echo "ERROR: i686-elf-tools not installed!"
  exit
fi
if [ ! -x "$(command -v git)" ]; then
  echo "ERROR: git not installed!"
  exit
fi

$CC $CFLAGS src/io/ports.c -o bin/ports.o
$CC $CFLAGS src/io/console.c -o bin/console.o

$CC $CFLAGS src/libc/string.c -o bin/string.o
$CC $CFLAGS src/libc/stdlib.c -o bin/stdlib.o

$CC $CFLAGS src/arch/x86/gdt.c -o bin/gdt.o
$CC $CFLAGS src/arch/x86/idt.c -o bin/idt.o
$AS src/arch/x86/description_tables.s -o bin/description_tables.o

$CC $CFLAGS -c src/interrupts/irq.c -o bin/irq.o
$CC $CFLAGS -c src/interrupts/isr.c -o bin/isr.o
nasm src/interrupts/interrupts.asm -f elf32 -O0 -o bin/interrupts.o

$CC $CFLAGS -c src/drivers/pci.c -o bin/pci.o
$CC $CFLAGS -c src/drivers/pcspkr.c -o bin/pcspkr.o
$CC $CFLAGS -c src/drivers/cmos.c -o bin/cmos.o
$CC $CFLAGS -c src/drivers/keyboard.c -o bin/keyboard.o

$CC $CFLAGS src/kernel.c -o bin/kernel.o

$AS src/kernel_entry.s -o bin/kernel_entry.o
$CC -g -I include -ffreestanding -Wall -Wextra -O2 -nostdlib -lgcc -T link.ld -o build/boot/kernel.elf $OBJECTS

for param in "$@"
do
  if [ "$param" == "iso_build" ]; then
    if [ ! -d "limine" ]; then
      git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1
    fi
      make -C limine
      mkdir -p iso_root
      cp -v build/boot/kernel.elf limine.cfg limine/limine.sys \
            limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/

      xorriso -as mkisofs -b limine-cd.bin \
              -no-emul-boot -boot-load-size 4 -boot-info-table \
              --efi-boot limine-cd-efi.bin \
              -efi-boot-part --efi-boot-image --protective-msdos-label \
              iso_root -o swanos-latest.iso

      ./limine/limine-deploy swanos-latest.iso
  fi

  if [ "$param" == "run" ]; then
    if [ -x "$(command -v qemu-system-i386)" ]; then
      qemu-system-i386 -m 64M -name SwanOS -kernel build/boot/kernel.elf -monitor stdio -serial file:Qemu.log -soundhw pcspk
    else
      echo "ERROR: qemu not installed!"
      exit
    fi
  fi
done