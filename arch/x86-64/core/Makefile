CFLAGS += -mno-mmx -mno-sse -mno-sse2
CFLAGS += -I./arch/x86-64/include/

subdirs-1 += apic/

obj-1 += entry.o main.o
obj-1 += acpi.o serial.o
obj-1 += mm.o apic.o
obj-1 += trap.o
obj-1 += trap-handler.o
obj-1 += irq.o
obj-1 += hpet.o
