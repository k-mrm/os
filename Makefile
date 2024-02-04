PREFIX =
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

CFLAGS = -Wall -Og -g -MD -ffreestanding -nostdinc -nostdlib -nostartfiles
CFLAGS += -I ./include/
LDFLAGS =

QEMUPREFIX =
QEMU = $(QEMUPREFIX)qemu-system-x86_64

NAME = kernel
elf = $(NAME).elf
img = $(NAME).img
iso = $(NAME).iso
map = $(NAME).map

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

$(elf): $(OBJS) link.ld
	@echo LD $@
	@$(LD) -n $(LDFLAGS) -Map $(map) -T link.ld -o $@ $(OBJS)

$(img): bootblock $(elf)

iso: $(iso)

$(iso): $(elf) boot/grub.cfg
	@mkdir -p iso/boot/grub
	@cp boot/grub.cfg iso/boot/grub
	@cp $(elf) iso/boot/
	@grub-mkrescue -o $@ iso/

clean:
	$(RM) $(OBJS) $(elf) $(iso) $(img) boot/boot.o
	$(RM) -rf iso/

qemu-img: $(img)
	$(QEMU) -nographic -drive file=$(img),index=0,media=disk,format=raw -smp $(NCPU) -m $(MEMSZ)

qemu-iso: $(iso)
	$(QEMU) -drive file=$(iso),format=raw -smp $(NCPU) -m $(MEMSZ)

.PHONY: clean qemu
