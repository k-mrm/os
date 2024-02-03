OUTPUT_ARCH(i386:x86-64)
ENTRY(entry32)

SECTIONS {
	. = 1M;
	vmm_start = .;

	.text : {
		KEEP(*(.text.head))
		KEEP(*(.text.boot))
		*(.text)
		*(.text.*)
	}
	
	. = ALIGN(4096);
	.data : {
		*(.data .data.*)
	}

	. = ALIGN(4096);
	.rodata : {
		*(.rodata .rodata.*)
	}

	. = ALIGN(16);
	.bss : {
		__bss_start = .;
		*(.bss .bss.*)
		__bss_end = .;
	}
	
	. = ALIGN(4096);
	vmm_end = .;

	/DISCARD/ : {
		*(.comment .comment.*)
	}
}

__bss_size = (__bss_end - __bss_start) >> 3;
