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
#include <akari/mm.h>
#include <akari/kalloc.h>
#include <arch/mm.h>
#include <arch/memlayout.h>
#include <msr.h>

#include "mm.h"

/* Kernel Page Directory */
static PTE kpml4[512] ALIGNED(PAGESIZE);

extern PTE __boot_pml4[];
extern PTE __boot_pdpt[];

bool x86nxe;

const ulong gdt[] = {
	0x0,			// NULL
	0x00cf9b000000ffff,	// KCODE32
	0x00cf93000000ffff,	// KDATA32
	0x00af9b000000ffff,	// KCODE64
	0x00af93000000ffff,	// KDATA64
};

static inline void
LoadGdt(const ulong *gdt, ulong gdtsize)
{
	volatile u16 t[5];

	t[0] = (u16)gdtsize - 1;
	t[1] = (u16)(ulong)gdt;
	t[2] = (u16)((ulong)gdt >> 16);
	t[3] = (u16)((ulong)gdt >> 32);
	t[4] = (u16)((ulong)gdt >> 48);

	asm volatile ("lgdt (%0)" :: "r"(t));
}

static void INIT
GdtInit(void)
{
	LoadGdt(gdt, sizeof gdt);
}

void
ArchSwitchVas(VAS *vas)
{
	PHYSADDR pgtpa = V2P(vas->Pgdir);

	if (vas->User)
	{
		// TODO
	}

	asm volatile ("mov %0, %%cr3" :: "r"(pgtpa));
}

void
ArchInitKvas(VAS *kvas)
{
	kvas->Pgdir = kpml4;
	kvas->Level = 4;
	kvas->LowestLevel = 1;
}

void INIT
X86mmInit(void)
{
	u32 efer = Rdmsr32(IA32_EFER);

	x86nxe = !!(efer & IA32_EFER_NXE);

	GdtInit();
}

void INIT
KillIdmap(void)
{
	__boot_pml4[PIDX(4, KERNLINK_PA)] = 0;
}
