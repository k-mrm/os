CONFIG ?= .config

include $(CONFIG)

DIR ?= .

CC = $(PREFIX)gcc
AS = $(PREFIX)as
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

CFLAGS := -Wall -Og -g -MD -ffreestanding -nostdinc -nostdlib -nostartfiles
CFLAGS += -I ./include/
LDFLAGS :=

QEMUPREFIX ?=

NAME = kernel
elf = $(NAME).elf
img = $(NAME).img
iso = $(NAME).iso
map = $(NAME).map

NCPU ?= 1
MEMSZ ?= 512

arch-$(CONFIG_X86_64) := x86-64
arch-$(CONFIG_AARCH64) := aarch64

subdirs-1 = core arch/$(arch-1)
subdirs-1 += driver

OBJS = $(objs-1:%=%)

$(elf): arch/$(arch-1)/link.ld $(CONFIG)
	$(MAKE) -f Makefile.build build DIR=$(DIR)
	@$(LD) -n $(LDFLAGS) -Map $(map) -T arch/$(arch-1)/link.ld -o $@ $(OBJS)

iso: $(iso)

$(iso): $(elf) boot/grub.cfg
	@mkdir -p iso/boot/grub
	@cp boot/grub.cfg iso/boot/grub
	@cp $(elf) iso/boot/
	@grub-mkrescue -o $@ iso/

clean:
	$(RM) $(OBJS) $(elf) $(iso) $(img) $(map) boot/boot.o
	$(RM) -rf iso/

#qemu-img: $(img)
#	$(QEMU) -nographic -drive file=$(img),index=0,media=disk,format=raw -smp $(NCPU) -m $(MEMSZ)

qemu-iso: $(iso)
	$(QEMU) -drive file=$(iso),format=raw -smp $(NCPU) -m $(MEMSZ)

.PHONY: clean qemu iso
