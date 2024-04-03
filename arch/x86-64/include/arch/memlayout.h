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

// Direct mapping offset: 0xffff800000000000 - 0xffffc00000000000

#define KLINK_OFFSET	ULL(0xffff800000000000)
#define KERNLINK	ULL(0xffff800000100000)
#define KERNLINK_PA	ULL(0x100000)

#define PAGE_OFFSET	KLINK_OFFSET

#ifndef __ASSEMBLER__

extern char __kstart[], __kend[];
extern char __ktext[], __ktext_e[];
extern char __rodata[], __rodata_e[];
extern char __kinit[], __kinit_e[];

#include <akari/types.h>

static inline PHYSADDR
V2P(void *p)
{
	ulong va = (ulong)p;

	return va - PAGE_OFFSET;
}

static inline void *
P2V(PHYSADDR pa)
{
	return (void *)(pa + PAGE_OFFSET);
}

static inline void *
KernelStart(void)
{
	return __kstart;
}

static inline void *
KernelEnd(void)
{
	return __kend;
}

#define IS_KERN_TEXT(addr)	((ulong)__ktext <= (addr) && (addr) < (ulong)__ktext_e)
#define IS_KERN_RODATA(addr)	((ulong)__rodata <= (addr) && (addr) < (ulong)__rodata_e)
#define IS_KINIT(addr)		((ulong)__kinit <= (addr) && (addr) < (ulong)__kinit_e)

#endif	// __ASSEMBLER__

#endif	// _ARCH_MEMLAYOUT_H
