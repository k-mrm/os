#ifndef AKARIVMM_MULTIBOOT_H
#define AKARIVMM_MULTIBOOT_H

#include <akarivmm/types.h>

struct multiboot {
	u32 magic;
	u32 flags;
	u32 length;
	u32 checksum;
};

#endif	/* AKARIVMM_MULTIBOOT_H */
