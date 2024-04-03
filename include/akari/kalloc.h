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

#ifndef _KALLOC_H
#define _KALLOC_H

#include <akari/types.h>
#include <akari/compiler.h>
#include <arch/mm.h>
#include <arch/memlayout.h>

typedef struct PAGE		PAGE;
typedef struct PAGEBLOCK	PAGEBLOCK;

struct PAGE
{
	PAGE *Next;
	u8 Blockno;
};

struct PAGEBLOCK
{
	PAGE *Pages;
	PHYSADDR Base;
	ulong nPages;
	u8 Node;	// NUMA Node
};

extern PAGEBLOCK PRoot[32];
extern uint nPRoot;

// FIXME: naive
static inline PAGEBLOCK *
Pa2Block(PHYSADDR pa)
{
	PAGEBLOCK *b;
	ulong size;

	for (b = PRoot; b < &PRoot[nPRoot]; b++)
	{
		size = b->nPages << PAGESHIFT;
		if (b->Base <= pa && pa < b->Base + size)
		{
			return b;
		}
	}

	return NULL;
}

static inline PAGE *
Pa2Page(PHYSADDR pa)
{
	PAGEBLOCK *block;

	block = Pa2Block(pa);

	return block->Pages + ((pa - block->Base) >> PAGESHIFT);
}

static inline PHYSADDR
Page2Pa(PAGE *page)
{
	PAGEBLOCK *block;

	block = &PRoot[page->Blockno];

	return block->Base + ((page - block->Pages) << PAGESHIFT);
}

static inline PAGE *
Va2Page(void *va)
{
	return Pa2Page(V2P(va));
}

static inline void *
Page2Va(PAGE *page)
{
	return (void *)P2V(Page2Pa(page));
}

PAGE *Kpalloc(uint order);
void Kpfree(PAGE *p, uint order);

#define Kalloc()	Page2Va(Kpalloc(0))
#define Kfree(addr)	Kpfree(Va2Page(addr), 0)

#define PA2PFN(pa)		((pa) >> PAGESHIFT)
#define PFN2PA(pfn)		((pfn) << PAGESHIFT)

void KallocInitEarly(ulong start, ulong end) INIT;
void KallocInit(void) INIT;

#endif	// _KALLOC_H
