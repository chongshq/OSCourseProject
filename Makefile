
######################
# Makefile for Yinix #
######################


# Entry point of Yinix
# It must be as same as 'KernelEntryPointPhyAddr' in load.inc!!!
ENTRYPOINT	= 0x30400

# Offset of entry point in kernel file
# It depends on ENTRYPOINT
ENTRYOFFSET	=   0x400


# Programs, flags, etc.
ASM		= nasm
DASM		= ndisasm
CC		= gcc
LD		= ld
ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -f elf
CFLAGS		= -I include/ -c -fno-builtin -fno-stack-protector
LDFLAGS		= -s -Ttext $(ENTRYPOINT)
DASMFLAGS	= -u -o $(ENTRYPOINT) -e $(ENTRYOFFSET)

# This Program
YINIXBOOT	= boot/boot.bin boot/loader.bin
YINIXKERNEL	= kernel.bin
OBJS		= kernel/kernel.o kernel/syscall.o kernel/start.o kernel/main.o\
			kernel/clock.o kernel/i8259.o kernel/global.o kernel/protect.o\
			kernel/proc.o kernel/keyboard.o kernel/tty.o kernel/console.o\
			lib/klib.o lib/klibc.o lib/string.o kernel/printf.o kernel/readchar.o\
			kernel/sudo.o
DASMOUTPUT	= kernel.bin.asm

# All Phony Targets
.PHONY : everything final image clean realclean disasm all buildimg

# Default starting position
everything : $(YINIXBOOT) $(YINIXKERNEL)

all : realclean everything

final : all clean

image : final buildimg

clean :
	rm -f $(OBJS)

realclean :
	rm -f $(OBJS) $(YINIXBOOT) $(YINIXKERNEL)

disasm :
	$(DASM) $(DASMFLAGS) $(YINIXKERNEL) > $(DASMOUTPUT)

# Write "boot.bin" & "loader.bin" into floppy image "YINIX.IMG"
# We assume that "YINIX.IMG" exists in current folder
buildimg :
	mount YINIX.IMG /mnt/floppy -o loop
	cp -f boot/loader.bin /mnt/floppy/
	cp -f kernel.bin /mnt/floppy
	umount  /mnt/floppy

boot/boot.bin : boot/boot.asm boot/include/load.inc boot/include/fat12hdr.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/loader.bin : boot/loader.asm boot/include/load.inc boot/include/fat12hdr.inc boot/include/pm.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(YINIXKERNEL) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(YINIXKERNEL) $(OBJS)

kernel/kernel.o : kernel/kernel.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/syscall.o : kernel/syscall.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/start.o: kernel/start.c include/type.h include/const.h include/protect.h include/string.h include/proc.h include/proto.h \
			include/tty.h include/console.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/main.o: kernel/main.c include/type.h include/const.h include/protect.h include/string.h include/proc.h include/proto.h \
			include/tty.h include/console.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/i8259.o: kernel/i8259.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/global.o: kernel/global.c include/type.h include/const.h include/protect.h include/proc.h \
			include/tty.h include/console.h include/global.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/protect.o: kernel/protect.c include/type.h include/const.h include/protect.h include/proc.h include/proto.h \
			include/tty.h include/console.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/clock.o: kernel/clock.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/proc.o: kernel/proc.c include/type.h include/const.h include/protect.h include/string.h include/proc.h include/proto.h \
			include/tty.h include/console.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/keyboard.o: kernel/keyboard.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/keyboard.h include/keymap.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/tty.o: kernel/tty.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/keyboard.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/printf.o : kernel/printf.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/keyboard.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/readchar.o :  kernel/readchar.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/keyboard.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/console.o: kernel/console.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/keyboard.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/sudo.o: kernel/sudo.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/keyboard.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

lib/klibc.o: lib/klib.c include/type.h include/const.h include/protect.h include/string.h include/proc.h include/proto.h \
			include/tty.h include/console.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

lib/klib.o : lib/klib.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/string.o : lib/string.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

