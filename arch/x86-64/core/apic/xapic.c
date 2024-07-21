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
#include <akari/panic.h>
#include <akari/kalloc.h>
#include <akari/mm.h>

#define KPREFIX		"xapic:"

#include <akari/log.h>
#include <cpuid.h>
#include <msr.h>

#include "apic.h"

#define XAPIC_ID		0x020
#define XAPIC_VER		0x030
#define XAPIC_TPR		0x080
#define XAPIC_EOI		0x0b0
#define XAPIC_ESR		0x280
#define XAPIC_ICR_LOW		0x300
#define XAPIC_ICR_HIGH		0x310

static PHYSADDR XapicBasePa;
static volatile void *Xapic;

static inline u32
XapicReadRaw(u32 off)
{
	return *(volatile u32 *)(Xapic + off);
}

static inline void
XapicWriteRaw(u32 off, u32 val)
{
	*(volatile u32 *)(Xapic + off) = val;
}

static u32
XapicRead(u32 reg)
{
	u32 offset = reg;

	return XapicReadRaw(offset);
}

static void
XapicWrite(u32 reg, u32 val)
{
	u32 offset = reg;

	XapicWriteRaw(offset, val);

	// wait for completion
	XapicReadRaw(XAPIC_ID);
}

static void
EnableXapic(void)
{
	ulong apicbase;

	apicbase = Rdmsr64(IA32_APIC_BASE);

	if (apicbase & IA32_APIC_BASE_APIC_GLOBAL_ENABLE)
	{
		return;
	}

	apicbase |= IA32_APIC_BASE_APIC_GLOBAL_ENABLE;

	Wrmsr64(IA32_APIC_BASE, apicbase);
}

static void
XapicSendIPI(uint n)
{
	;
}

static u32
ApicBaseAddr(void)
{
	ulong apicbase;

	apicbase = Rdmsr64(IA32_APIC_BASE);

	return (apicbase & IA32_APIC_BASE_APIC_BASE_MASK);
}

static APIC XapicOps = {
	.Read = XapicRead,
	.Write = XapicWrite,
};

APIC *
XapicInit(void)
{
	EnableXapic();
	XapicBasePa = ApicBaseAddr();

	Xapic = KIOmap(XapicBasePa, PAGESIZE);

	if (!Xapic)
	{
		return NULL;
	}

	return &XapicOps;
}
