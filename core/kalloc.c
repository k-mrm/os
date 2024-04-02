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
#include <akari/kalloc.h>
#include <akari/sysmem.h>
#include <akari/mm.h>
#include <akari/panic.h>

#define KPREFIX		"kalloc:"

#include <akari/log.h>

PAGEBLOCK PRoot[32];
static uint nPRoot;
static ulong Earlystart, Earlyend;

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

PAGE *
Kpalloc(uint order)
{
	return NULL;
}

void
Kpfree(PAGE *page, uint order)
{
	;
}

static uint
EarlyFreeBlock(MEMBLOCK *block)
{
	PAGEBLOCK *pb;
	PHYSADDR addr;
	PHYSADDR bend = PAGEALIGNDOWN(block->Base + block->Size);

	pb = NewPageBlock();
	if (!pb)
	{
		panic("null page block");
	}

	pb->Base = PAGEALIGN(block->Base);
	pb->nPages = (bend - pb->Base) >> PAGESHIFT;
	// pb->Pages = BootmemAlloc(pb->nPages * sizeof(PAGE), sizeof(PAGE));
	pb->Node = 0;

	for (addr = pb->Base; addr < bend; addr += PAGESIZE)
	{
		;
		return 0;
	}

	return pb->nPages;
}

void
KallocInitEarly(ulong start, ulong end)
{
	MEMBLOCK *block;
	uint npages = 0;

	KDBG("initialize %p-%p\n", start, end);

	Earlystart = start;
	Earlyend = end;

	FOREACH_SYSMEM_AVAIL_BLOCK (block)
	{
		npages += EarlyFreeBlock(block);
	}

	if (npages == 0)
	{
		panic("system has no memory!");
	}
}

void
KallocInit(void)
{
	;
}
