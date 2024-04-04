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

#ifndef _X86_MM_H
#define _X86_MM_H

#include <arch/asm.h>

#define PTE_P		(1 << 0)
#define PTE_W		(1 << 1)
#define PTE_U		(1 << 2)
#define PTE_XD		(1ull << 63)

/*
 *  x86 48bit Virtual Address
 *
 *     48    39 38    30 29    21 20    12 11       0
 *    +--------+--------+--------+--------+----------+
 *    | level4 | level3 | level2 | level1 | page off |
 *    +--------+--------+--------+--------+----------+
 *       pml4     pdpt      pd       pt
 *
 */

#define PIDX(_level, _addr)	(((_addr) >> (12 + ((_level) - 1) * 9)) & 0x1ff)

#define	PAGESIZE	0x1000
#define PAGESHIFT	12

#define PTE_PA_MASK	ULL(0xfffffffff000)

#ifndef __ASSEMBLER__

#include <akari/pteflags.h>

struct VAS;
typedef struct VAS	VAS;

#define PPresent(_pte)		((_pte) & PTE_P)
#define PWritable(_pte)		((_pte) & PTE_W)
#define PUser(_pte)		((_pte) & PTE_U)

#define PTE_PA(_pte)		((ulong)(_pte) & PTE_PA_MASK)

#define ArchSetPtePgt(_pte, _pgtpa)	\
	(*(_pte) = ((_pgtpa) & PTE_PA_MASK) | PTE_P | PTE_W)
#define ArchSetPtePgtUser(_pte, _pgtpa)	\
	(*(_pte) = ((_pgtpa) & PTE_PA_MASK) | PTE_U | PTE_P | PTE_W)

extern bool x86nxe;

static inline ulong
ArchPteFlags(PTEFLAGS flags)
{
	ulong archflags = 0;

	if (flags & PTEFLAG_RW)
	{
		archflags |= PTE_W;
	}
	if (flags & PTEFLAG_USER)
	{
		archflags |= PTE_U;
	}
	if (x86nxe && !(flags & PTEFLAG_X))
	{
		archflags |= PTE_XD;
	}

	return archflags;
}

static inline void
ArchSetPteLeaf(PTE *pte, PHYSADDR pa, PTEFLAGS flags)
{
	ulong archflags = ArchPteFlags(flags);

	*pte = (pa & PTE_PA_MASK) | archflags | PTE_P;
}

void ArchSwitchVas(VAS *vas);

void ArchInitKvas(VAS *kvas);

#endif	// __ASSEMBLER__

#endif	// _X86_MM_H
