/*
 * Copyright (c) 2024, akarilab.net
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ARCH_MEMLAYOUT_H
#define _ARCH_MEMLAYOUT_H

#include <arch/asm.h>

// Direct mapping offset
#define DMAP_BASE	ULL(0xffff800000000000)
#define DMAP_END	ULL(0xffffc00000000000)

#define PAGE_OFFSET	DMAP_BASE

#define KLINK_OFFSET	ULL(0xffffffff80000000)
#define KERNLINK	ULL(0xffffffff80100000)
#define KERNLINK_PA	ULL(0x100000)

#ifndef __ASSEMBLER__

#include <akari/types.h>

static inline bool
IsDmap(ulong addr)
{
	return DMAP_BASE <= addr && addr < DMAP_END;
}

static inline PHYSADDR
V2P(void *p)
{
	ulong va = (ulong)p;

	return IsDmap((ulong)va) ? va - PAGE_OFFSET : va - KLINK_OFFSET;
}

static inline void *
P2V(PHYSADDR pa)
{
	return (void *)(pa + PAGE_OFFSET);
}

#endif	// __ASSEMBLER__

#endif	// _ARCH_MEMLAYOUT_H
