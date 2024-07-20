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
#include "seg.h"

const ulong gdt[] = {
	0x0,			// NULL
	0x00cf9b000000ffff,	// KCODE32
	0x00cf93000000ffff,	// KDATA32
	0x00af9b000000ffff,	// KCODE64
	0x00af93000000ffff,	// KDATA64
};

static void
ConfigGdt(void)
{
	;
}

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

void INIT
GdtInit(void)
{
	ConfigGdt();

	LoadGdt(gdt, sizeof gdt);
}
