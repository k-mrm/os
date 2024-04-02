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

#include <akari/types.h>
#include <akari/sysmem.h>
#include <akari/compiler.h>
#include <akari/panic.h>
#include <akari/mm.h>
#include <akari/string.h>
#include <arch/mm.h>
#include <arch/memlayout.h>

#define KPREFIX		"system memory:"

#include <akari/log.h>

SYSMEM Sysmem = {
	.Avail.Name = "Available",
	.Avail.nBlock = 0,
	.Rsrv.Name = "Reserved",
	.Rsrv.nBlock = 0,
};

/*
 * __MemblockOverlap
 * true: overlapped
 * false: ∅
 */
static bool
__MemblockOverlap(PHYSADDR astart, PHYSADDR aend, PHYSADDR bstart, PHYSADDR bend,
		  PHYSADDR *pstart, PHYSADDR *pend)
{
	PHYSADDR s, e;

	s = MAX(astart, bstart);
	e = MIN(aend, bend);

	if (s < e)
	{
		if (pstart)
		{
			*pstart = s;
		}
		if (pend)
		{
			*pend = e;
		}
		return true;
	}
	else
	{
		return false;
	}
}

static bool
MemblockOverlap(MEMBLOCK *a, MEMBLOCK *b, PHYSADDR *pstart, PHYSADDR *pend)
{
	return __MemblockOverlap(a->Base, a->Base + a->Size, b->Base, b->Base + b->Size,
				 pstart, pend);
}

static bool
MemblockMerge(MEMBLOCK *a, MEMBLOCK *b, PHYSADDR *pstart, PHYSADDR *pend)
{
	PHYSADDR s, e;

	if (!MemblockOverlap(a, b, NULL, NULL))
	{
		return false;
	}

	s = MIN(a->Base, b->Base);
	e = MAX(a->Base + a->Size, b->Base + b->Size);

	if (pstart)
	{
		*pstart = s;
	}
	if (pend)
	{
		*pend = e;
	}

	return true;
}

/*
 * BootmemFind
 * true: found memory, start physaddr is @pa
 * false: memory is not found
 */
static bool INIT
BootmemFind(uint nbytes, uint align, PHYSADDR *pa)
{
	MEMBLOCK *ab, *rb;
	PHYSADDR start, end;	/* [start, end) */
	PHYSADDR rstart, rend;	/* [rstart, rend) */
	PHYSADDR ms, me;
	bool overlap;

	FOREACH_SYSMEM_AVAIL_BLOCK (ab)
	{
		start = ab->Base;
		end = ab->Base + ab->Size;

		for (uint i = 0; i < Sysmem.Rsrv.nBlock + 1; i++)
		{
			rb = Sysmem.Rsrv.Block + i;

			if (i)
			{
				rstart = rb[-1].Base + rb[-1].Size;
			}
			else
			{
				rstart = 0x0;
			}

			if (i == Sysmem.Rsrv.nBlock)
			{
				rend = (PHYSADDR)-1ll;
			}
			else
			{
				rend = rb->Base;
			}
			
			overlap = __MemblockOverlap(start, end, rstart, rend, &ms, &me);
			if (overlap)
			{
				ms = ALIGN(ms, align);
				if (me - ms >= nbytes)
				{
					*pa = ms;
					return true;
				}
			}
		}
	}

	return false;
}

void * INIT
BootmemAlloc(uint nbytes, uint align)
{
	PHYSADDR pa;
	void *va;

	if (nbytes == 0)
	{
		return NULL;
	}
	if (!BootmemFind(nbytes, align, &pa))
	{
		return NULL;
	}

	ReserveMem(pa, nbytes);

	KDBG("alloc bootmem: %p-%p\n", pa, pa + nbytes - 1);

	va = P2V(pa);
	memset(va, 0, nbytes);

	return va;
}

static void DEBUG
MemchunkDump(MEMCHUNK *c)
{
	MEMBLOCK *b;
	uint i;

	KDBG("memchunk %s:\n", c->Name);
	FOREACH_MEMCHUNK_BLOCK (c, i, b)
	{
		KDBG("\t[%p-%p]\n", b->Base, b->Base + b->Size - 1);
	}
}

static void DEBUG
SysmemDump(void)
{
	MemchunkDump(&Sysmem.Avail);
	MemchunkDump(&Sysmem.Rsrv);
}

static void
MemRemoveBlock(MEMCHUNK *c, uint idx)
{
	if (idx >= c->nBlock)
	{
		return;
	}

	memmove(c->Block + idx, c->Block + idx + 1, (c->nBlock - idx - 1) * sizeof(MEMBLOCK));
	c->nBlock--;
}

static void
MemInsertBlock(MEMCHUNK *c, uint idx, PHYSADDR start, ulong size)
{
	MEMBLOCK *block;

	if (c->nBlock >= 32)
	{
		panic("nBlock > 32");
	}

	memmove(c->Block + idx + 1, c->Block + idx, (c->nBlock - idx) * sizeof(MEMBLOCK));

	block = &c->Block[idx];
	block->Base = start;
	block->Size = size;

	c->nBlock++;

	MemchunkDump(c);
}

static void
MemchunkMerge(MEMCHUNK *c)
{
	MEMBLOCK *block, *next;
	PHYSADDR start, end;
	bool mergeable;

	for (int i = 0; i < c->nBlock - 1; )
	{
		block = c->Block + i;
		next = c->Block + i + 1;

		mergeable = MemblockMerge(block, next, &start, &end);

		if (mergeable)
		{
			MemRemoveBlock(c, i);
			block->Base = start;
			block->Size = end - start;
			continue;
		}

		i++;
	}

	MemchunkDump(c);
}

static void
MemNewBlock(MEMCHUNK *c, PHYSADDR start, ulong size)
{
	MEMBLOCK *block;
	PHYSADDR end = start + size;
	uint idx = 0;

	KLOG("%s [%p-%p]\n", c->Name, start, end);

	FOREACH_MEMCHUNK_BLOCK (c, idx, block)
	{
		if (start <= block->Base)
		{
			break;
		}
	}

	MemInsertBlock(c, idx, start, size);
	MemchunkMerge(c);
}

void
NewMem(PHYSADDR base, ulong size)
{
	MemNewBlock(&Sysmem.Avail, base, size);
}

void
ReserveMem(PHYSADDR base, ulong size)
{
	MemNewBlock(&Sysmem.Rsrv, base, size);
}
