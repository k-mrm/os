CONFIG ?= .config

include $(CONFIG)

DIR ?= .

QEMUPREFIX ?=
QEMU = $(QEMUPREFIX)qemu-system-x86_64

NAME = kernel
elf = $(NAME).elf
img = $(NAME).img
iso = $(NAME).iso
map = $(NAME).map

NCPU ?= 4
MEMSZ ?= 512

arch-$(CONFIG_X86_64) := x86-64
arch-$(CONFIG_AARCH64) := aarch64

ifndef arch-1
$(error arch is not defined)
endif

subdirs-1 = core arch/$(arch-1)
#subdirs-1 += driver

$(elf): arch/$(arch-1)/link.ld $(CONFIG)
	$(MAKE) -f Makefile.build build DIR=$(DIR) ARCH=$(arch-1)
	@$(LD) -n -Map $(map) -T arch/$(arch-1)/link.ld -o $@ ./output.o

iso: $(iso)

$(iso): $(elf) grub.cfg
	@mkdir -p iso/boot/grub
	@cp grub.cfg iso/boot/grub
	@cp $(elf) iso/boot/
	@grub-mkrescue -o $@ iso/

clean:
	$(RM) $(elf) $(iso) $(img) $(map)
	$(RM) -rf iso/
	$(MAKE) -f Makefile.build cleand DIR=$(DIR)

#qemu-img: $(img)
#	$(QEMU) -nographic -drive file=$(img),index=0,media=disk,format=raw -smp $(NCPU) -m $(MEMSZ)

qemu-iso: $(iso)
	$(QEMU) -nographic -drive file=$(iso),format=raw -serial mon:stdio -smp $(NCPU) -m $(MEMSZ)

qemu-gdb: $(iso)
	$(QEMU) -nographic -drive file=$(iso),format=raw -serial mon:stdio -smp $(NCPU) -m $(MEMSZ) -S -gdb tcp::1234

.PHONY: clean iso
