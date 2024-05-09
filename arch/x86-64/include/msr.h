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

#ifndef _X86_MSR_H
#define _X86_MSR_H

#define IA32_EFER	0xc0000080	
#define IA32_EFER_SCE		(1 << 0)
#define IA32_EFER_LME		(1 << 8)
#define IA32_EFER_LMA		(1 << 10)
#define IA32_EFER_NXE		(1 << 11)
#define IA32_EFER_SVME		(1 << 12)
#define IA32_EFER_LMSLE		(1 << 13)
#define IA32_EFER_FFXSR		(1 << 14)
#define IA32_EFER_TCE		(1 << 15)

#define IA32_APIC_BASE		0x1b
#define IA32_APIC_BASE_ENABLE_X2APIC		0x400
#define IA32_APIC_BASE_APIC_GLOBAL_ENABLE	0x800
#define IA32_APIC_BASE_APIC_BASE_MASK		0xfffffffffffff000ull

#ifndef __ASSEMBLER__

#include <akari/types.h>

static inline void
Rdmsr(u32 reg, u32 *a, u32 *d)
{
	asm volatile ("rdmsr" : "=a"(*a), "=d"(*d) : "c"(reg));
}

static inline u32
Rdmsr32(u32 reg)
{
	u32 a, d;

	Rdmsr(reg, &a, &d);

	return a;
}

static inline u64
Rdmsr64(u32 reg)
{
	u32 a, d;

	Rdmsr(reg, &a, &d);

	return (u64)a | ((u64)d << 32);
}

static inline void
Wrmsr(u32 reg, u32 a, u32 d)
{
	asm volatile ("wrmsr" :: "c"(reg), "a"(a), "d"(d));
}

static inline void
Wrmsr32(u32 reg, u32 val)
{
	Wrmsr(reg, val, 0);
}

static inline void
Wrmsr64(u32 reg, u64 val)
{
	u32 a, d;

	a = (u32)val;
	d = (u32)(val >> 32);

	Wrmsr(reg, a, d);
}

#endif	// __ASSEMBLER__

#endif	// _X86_MSR_H
