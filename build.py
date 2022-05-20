import os, sys

OBJECTS = "bin/kernel.o bin/kernel_entry.o bin/console.o bin/ports.o bin/string.o bin/gdt.o bin/idt.o"

os.system('i686-elf-gcc -g -I include -ffreestanding -Wall -Wextra -O0 -c src/libc/string.c -o bin/string.o')
os.system('i686-elf-gcc -g -I include -ffreestanding -Wall -Wextra -O0 -c src/io/ports.c -o bin/ports.o')
os.system('i686-elf-gcc -g -I include -ffreestanding -Wall -Wextra -O0 -c src/io/console.c -o bin/console.o')
os.system('i686-elf-gcc -g -I include -ffreestanding -Wall -Wextra -O0 -c src/interrupts/gdt.c -o bin/gdt.o')
os.system('i686-elf-gcc -g -I include -ffreestanding -Wall -Wextra -O0 -c src/interrupts/idt.c -o bin/idt.o')
os.system('i686-elf-gcc -g -I include -ffreestanding -Wall -Wextra -O0 -c src/kernel.c -o bin/kernel.o')

os.system('i686-elf-as src/kernel_entry.s -o bin/kernel_entry.o')
os.system('i686-elf-gcc -g -I include -ffreestanding -Wall -Wextra -O0 -nostdlib -lgcc -T link.ld -o build/boot/kernel.elf {}'.format(OBJECTS))
if sys.platform == "linux" or sys.platform == "linux2":
    os.system("""grub-mkrescue -o "swanos-0.1.0.iso" build/ -V SwanOS""")
else:
    os.system("""wsl grub-mkrescue -o "swanos-0.1.0.iso" build/ -V SwanOS""")

os.system("qemu-system-i386 -m 16 -name SwanOS -cdrom swanos-0.1.0.iso -serial file:Qemu.log -no-reboot")