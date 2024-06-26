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

#ifndef _X86_CPUID_H
#define _X86_CPUID_H

#define CPUID_0		0x0
#define CPUID_1		0x1
#define CPUID_1_ECX_X2APIC	0x200000
#define CPUID_1_EDX_MSR		0x20
#define CPUID_1_EDX_PAE		0x40
#define CPUID_1_EDX_APIC	0x200

#define CPUID_EXT1	0x80000001
#define CPUID_EXT1_EDX_64BIT	0x20000000

#define CPUID_EXT2	0x80000002
#define CPUID_EXT3	0x80000003
#define CPUID_EXT4	0x80000004

#ifndef __ASSEMBLER__

#include <akari/types.h>

static inline void
Cpuid(u32 ax, u32 *a, u32 *b, u32 *c, u32 *d)
{
	asm volatile ("cpuid" : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
			      : "a"(ax));
}

#endif	// __ASSEMBLER__

#endif	// _X86_CPUID_H
