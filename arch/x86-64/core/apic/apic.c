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

#define KPREFIX		"apic:"

#include <akari/log.h>
#include <cpuid.h>

#include "apic.h"

#define ID		0x020
#define VER		0x030
#define TPR		0x080
#define EOI		0x0b0
#define ESR		0x280
#define ICR_LOW		0x300
#define ICR_HIGH	0x310

APIC *Apic;

static bool
XapicSupported(void)
{
	u32 a, b, c, d;

	Cpuid(CPUID_1, &a, &b, &c, &d);

	return !!(d & CPUID_1_EDX_APIC);
}

static bool
X2apicSupported(void)
{
	u32 a, b, c, d;

	Cpuid(CPUID_1, &a, &b, &c, &d);

	return !!(c & CPUID_1_ECX_X2APIC);
}

static void INIT
ApicClockInit(void)
{
	;
}

void INIT
ApicInitBsp(void)
{
	if (X2apicSupported())
	{
		KDBG("Kernel use x2apic\n");

		// Apic = X2apicInit();
	}

	if (!Apic && XapicSupported())
	{
		KDBG("Kernel use xapic\n");

		Apic = XapicInit();
	}

	if (!Apic)
	{
		Panic("No apic");
	}

	// Clear Error Status
	Apic->Write(ESR, 0);

	Apic->Write(TPR, 0);

	ApicClockInit();
}

void INIT
ApicInitAp(void)
{
	;
}
