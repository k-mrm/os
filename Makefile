PREFIX =
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

CFLAGS = -Wall -Og -g -MD -ffreestanding -nostdinc -nostdlib -nostartfiles
CFLAGS += -I ./include/
LDFLAGS =

QEMUPREFIX =
QEMU = $(QEMUPREFIX)qemu-system-x86_64

ifndef NCPU
NCPU = 1
endif

ifndef MEMSZ
MEMSZ = 512
endif

C = core/
OBJS = $(C)entry.o $(C)main.o

%.o: %.c
	@echo CC $@
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	@echo CC $@
	@$(CC) $(CFLAGS) -c $< -o $@

bootblock: boot/boot.o

kernel.elf: $(OBJS) link.ld
	@echo LD $@
	@$(LD) -n $(LDFLAGS) -T link.ld -o $@ $(OBJS)

kernel.img: bootblock kernel.elf

kernel.iso: kernel.elf boot/grub.cfg
	@mkdir -p iso/boot/grub
	@cp boot/grub.cfg iso/boot/grub
	@cp kernel.elf iso/boot/
	@grub-mkrescue -o $@ iso/

clean:
	$(RM) $(OBJS) kernel.elf kernel.iso boot/boot.o
	$(RM) -rf iso/

qemu-img: kernel.img
	$(QEMU) -nographic -drive file=kernel.img,index=0,media=disk,format=raw -smp $(NCPU) -m $(MEMSZ)

qemu-iso: kernel.iso
	$(QEMU) -nographic -drive file=kernel.iso,format=raw -smp $(NCPU) -m $(MEMSZ)

.PHONY: clean qemu
