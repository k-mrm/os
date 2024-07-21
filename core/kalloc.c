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

// Physical Memory Buddy Allocator for Kernel

#include <akari/types.h>
#include <akari/compiler.h>
#include <akari/string.h>
#include <akari/kalloc.h>
#include <akari/sysmem.h>
#include <akari/mm.h>
#include <akari/panic.h>

#define KPREFIX		"kalloc:"

#include <akari/log.h>

#define MAX_ORDER	10	

typedef struct FREECHUNK	FREECHUNK;
typedef struct KALLOCBLOCK	KALLOCBLOCK;

PAGEBLOCK PRoot[32];
uint nPRoot = 0;

static ulong Earlystart, Earlyend;

struct FREECHUNK
{
	// SPINLOCK;
	PAGE *Freelist;
	uint nFree;
};

struct KALLOCBLOCK
{
	FREECHUNK Chunk[MAX_ORDER+1];
};

static KALLOCBLOCK kblock;

static PAGEBLOCK *
NewPageBlock(void)
{
	if (nPRoot < 32)
	{
		return &PRoot[nPRoot++];
	}
	else
	{
		return NULL;
	}
}

static void DEBUG
KallocDump(void)
{
	FREECHUNK *c;
	ulong nbytes = 0;

	for (uint order = 0; order < MAX_ORDER + 1; order++)
	{
		c = kblock.Chunk + order;

		KDBG("%d bytes page(order%d): %d Pages\n", PAGESIZE * (1 << order), order, c->nFree);
		nbytes += PAGESIZE * (1 << order) * c->nFree;
	}

	KDBG("Total: %d Bytes\n", nbytes);
}

static PAGE *
ChunkDeletePa(FREECHUNK *chunk, PHYSADDR pa)
{
	PAGE *prev = NULL;

	for (PAGE *p = chunk->Freelist; p; prev = p, p = p->Next)
	{
		if (Page2Pa(p) == pa)
		{
			chunk->nFree--;

			if (prev)
			{
				prev->Next = p->Next;
			}
			else
			{
				chunk->Freelist = p->Next;
			}
			return p;
		}
	}

	return NULL;
}

static void
ChunkAddPage(FREECHUNK *chunk, PAGE *page)
{
	page->Next = chunk->Freelist;
	chunk->Freelist = page;
	chunk->nFree++;
}

static void
SplitPage(KALLOCBLOCK *kb, PAGE *p, uint order, uint blockorder)
{
	FREECHUNK *c;
	PAGE *latter;
	uint pagesize;
	
	while (blockorder > order)
	{
		blockorder--;
		c = kb->Chunk + blockorder;

		pagesize = 1 << blockorder;
		latter = p + pagesize;

		ChunkAddPage(c, latter);
	}
}

static PAGE *
__AllocPages(KALLOCBLOCK *kb, uint order)
{
	FREECHUNK *c;
	PAGE *p;

	for (uint i = order; i < MAX_ORDER; i++)
	{
		c = kb->Chunk + i;
		if (!c->Freelist)
		{
			// empty
			continue;
		}
		p = c->Freelist;
		c->Freelist = p->Next;
		c->nFree--;

		SplitPage(kb, p, order, i);

		return p;
	}

	return NULL;
}

PAGE *
AllocPages(uint order)
{
	return __AllocPages(&kblock, order);
}

void *
AllocZeroPagesVa(uint order)
{
	void *va;

	va = Page2Va(AllocPages(order));

	if (va)
	{
		memset(va, 0, 1 << order << PAGESHIFT);
		return va;
	}
	else
	{
		return NULL;
	}
}

#define BUDDY(pfn, order)	((pfn) ^ (1 << (order)))

static void
MergePage(KALLOCBLOCK *kb, PAGE *page, uint order)
{
	FREECHUNK *chunk;
	ulong pfn, buddypfn;
	PAGE *buddy;
	PAGE *p0;

	if (order > MAX_ORDER)
	{
		return;
	}

	chunk = kb->Chunk + order;

	pfn = PA2PFN(Page2Pa(page));
	buddypfn = BUDDY(pfn, order);

	// Lock chunk

	if (order != MAX_ORDER &&
	    (buddy = ChunkDeletePa(chunk, PFN2PA(buddypfn))) != NULL)
	{
		// mergeable page
		p0 = pfn < buddypfn ? page : buddy;
		MergePage(kb, p0, order + 1);
	}
	else
	{
		ChunkAddPage(chunk, page);
	}

	// Unlock chunk
}

void
FreePages(PAGE *page, uint order)
{
	MergePage(&kblock, page, order);
}

static void INIT
InitPageBlock(MEMBLOCK *block)
{
	PAGEBLOCK *pb;
	PHYSADDR bend = PAGEALIGNDOWN(block->Base + block->Size);

	pb = NewPageBlock();
	if (!pb)
	{
		Panic("null page block");
	}

	pb->Base = PAGEALIGN(block->Base);
	pb->nPages = (bend - pb->Base) >> PAGESHIFT;
	pb->Pages = BootmemAlloc(pb->nPages * sizeof(PAGE), sizeof(PAGE));
	pb->Node = 0;

	if (!pb->Pages)
	{
		Panic("initpageblock failed\n");
	}
}

#define PBLOCKNO(pb)	((pb) - PRoot)

static uint INIT
EarlyFreeBlock(PAGEBLOCK *pb)
{
	PAGE *page;
	PHYSADDR addr = pb->Base;
	uint bno = PBLOCKNO(pb);
	ulong npages = 0;

	KDBG("early free block: %p-%p %d bytes\n", pb->Base, pb->Base + (pb->nPages << PAGESHIFT),
	     pb->nPages << PAGESHIFT);

	for (ulong i = 0; i < pb->nPages; i++, addr += PAGESIZE)
	{
		page = pb->Pages + i;
		page->Blockno = bno;
		// FIXME: naive
		if (!ReservedAddr(addr))
		{
			npages++;
			FreePages(page, 0);
		}
	}

	return npages;
}

void INIT
KallocInitEarly(ulong start, ulong end)
{
	MEMBLOCK *block;
	PAGEBLOCK *pblock;
	ulong npages = 0;

	KDBG("initialize %p-%p\n", start, end);

	memset(&kblock, 0, sizeof kblock);

	Earlystart = start;
	Earlyend = end;

	FOREACH_SYSMEM_AVAIL_BLOCK (block)
	{
		InitPageBlock(block);
	}

	FOREACH_PAGEBLOCK (pblock)
	{
		npages += EarlyFreeBlock(pblock);
	}

	KallocDump();

	if (npages == 0)
	{
		Panic("system has no memory!");
	}
}

void
KallocInit(void)
{
	;
}
