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
#include <akari/compiler.h>
#include <akari/mm.h>
#include <akari/pteflags.h>
#include <akari/panic.h>
#include <akari/sysmem.h>
#include <akari/kalloc.h>
#include <akari/string.h>
#include <arch/mm.h>
#include <arch/memlayout.h>

#define KPREFIX		"mm:"

#include <akari/log.h>

/*
 *  Kernel address space
 */
static VAS kernvas;

#define SwitchVas	ArchSwitchVas

void
SwitchKvas(void)
{
	SwitchVas(&kernvas);
}

static PTE *
VasPageWalk(VAS *vas, ulong va, bool allocpgt)
{
	PAGETABLE pgt = vas->Pgdir;
	uint level;
	uint vlevel = vas->Level;
	uint vll = vas->LowestLevel;
	PHYSADDR pgtpa;
	PTE *pte;

	for (level = vlevel; level > vll; level--)
	{
		pte = &pgt[PIDX(level, va)];

		if (PPresent(*pte))
		{
			pgtpa = PTE_PA(*pte);
			pgt = (PAGETABLE)P2V(pgtpa);
		}
		else if (allocpgt)
		{
			pgt = Zalloc();
			if (!pgt)
			{
				return NULL;
			}
			pgtpa = V2P(pgt);

			ArchSetPtePgt(pte, pgtpa);
		}
		else
		{
			// unmapped
			return NULL;
		}
	}

	return &pgt[PIDX(level, va)];
}

static void
VasMapPages(VAS *vas, ulong va, PHYSADDR pa, ulong size, PTEFLAGS flags)
{
	PTE *pte;

	for (ulong p = 0; p < size; p += PAGESIZE, va += PAGESIZE, pa += PAGESIZE)
	{
		pte = VasPageWalk(vas, va, true);

		if (!pte)
		{
			panic("null pte %p", va);
		}
		if (PPresent(*pte))
		{
			panic("this entry has been used: va %p", va);
		}

		ArchSetPteLeaf(pte, pa, flags);
	}
}

static PHYSADDR
Addrwalk(VAS *vas, ulong va)
{
	PTE *pte;

	pte = VasPageWalk(vas, va, false);

	if (pte && PPresent(*pte))
	{
		return PTE_PA(*pte);
	}
	else
	{
		return 0;
	}
}

static PHYSADDR
KAddrwalk(ulong va)
{
	return Addrwalk(&kernvas, va);
}

void
KvasMapPage(void *va, PHYSADDR pa, PTEFLAGS flags)
{
	VasMapPages(&kernvas, (ulong)va, pa, PAGESIZE, flags);
}

static void INIT
InitKvas(void)
{
	ArchInitKvas(&kernvas);

	kernvas.User = false;

	if (!kernvas.Pgdir)
	{
		panic("NULL pgdir");
	}
	if (!PAGEALIGNED(kernvas.Pgdir))
	{
		panic("pgdir must be page-aligned");
	}

	memset(kernvas.Pgdir, 0, PAGESIZE);
}

void INIT
KvasMap(void)
{
	PHYSADDR pstart, pend;
	void *va;
	PTEFLAGS flags;

	InitKvas();

	pstart = SysmemStart();
	pend = SysmemEnd();

	for (PHYSADDR addr = pstart; addr < pend; addr += PAGESIZE)
	{
		va = P2V(addr);
		flags = PTEFLAG_NORMAL;

		if (IS_KERN_TEXT(va))
		{
			flags |= PTEFLAG_RO | PTEFLAG_X;
		}
		else if (IS_KERN_RODATA(va))
		{
			flags |= PTEFLAG_RO;
		}
		else
		{
			flags |= PTEFLAG_RW;
		}

		// KDBG("make map: %p to %p\n", va, addr);
		KvasMapPage(va, addr, flags);
	}

	SwitchKvas();

	KDBG("Switched to kernel virtual address space\n");
}
