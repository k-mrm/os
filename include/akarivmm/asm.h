#ifndef AKARIVMM_ASM_H
#define AKARIVMM_ASM_H

#define CR4_PAE		(1 << 5)

#define EFER_LME	(1 << 8)

#ifndef __ASSEMBLER__

#include <akarivmm/types.h>

static inline void
hlt(void) {
	asm volatile("hlt");
}

static inline u32
inb(u16 port) {
	return 0;
}

#endif	/* __ASSEMBLER__ */

#endif	/* AKARIVMM_ASM_H */
