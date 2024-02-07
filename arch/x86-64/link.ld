OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(entry32)

virt = 0xffffffff80100000;
phys = 0x100000;
offset = virt - phys;

phys_entry32 = ABSOLUTE(entry32 - offset);

SECTIONS {
	. = virt;
	kstart = .;

	.text : AT(ADDR(.text) - offset) {
		KEEP(*(.text.head))
		KEEP(*(.text.boot))
		*(.text)
		. = ALIGN(4096);
		__kinit = .;
		*(.text.init)
		. = ALIGN(4096);
		__kinit_end = .;
		*(.text.*)
	}
	
	. = ALIGN(4096);
	.data : AT(ADDR(.data) - offset) {
		*(.data .data.*)
	}

	. = ALIGN(4096);
	.rodata : AT(ADDR(.rodata) - offset) {
		*(.rodata .rodata.*)
	}

	. = ALIGN(4096);
	.bss : AT(ADDR(.rodata) - offset) {
		__bss_start = .;
		*(.bss .bss.*)
		*(COMMON)
		__bss_end = .;
	}
	
	. = ALIGN(4096);
	kend = .;

	/DISCARD/ : {
		*(.comment .comment.*)
	}
}
