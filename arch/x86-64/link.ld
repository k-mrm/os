OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(entry32)

virt = 0xffffffff80100000;
phys = 0x100000;
offset = virt - phys;

phys_entry32 = ABSOLUTE(entry32 - offset);
pa_trampoline64 = ABSOLUTE(trampoline64 - offset);

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
	.boot_pgt : AT(ADDR(.boot_pgt) - offset) {
		__boot_pgt_s = .;
		__boot_pml4 = .;
		. += 4096;
		__boot_pdpt = .;
		. += 4096;
		__boot_pd = .;
		. += 4096;
		__boot_pgt_e = .;
    	}

	. = ALIGN(4096);
	.bss : AT(ADDR(.bss) - offset) {
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
