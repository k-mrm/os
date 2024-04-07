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
#include <akari/string.h>
#include <akari/panic.h>

#define KPREFIX		"x86cpu:"

#include <akari/log.h>

#include <x86cpu.h>
#include <cpuid.h>

extern X86CPU __initdata_x86cpu_s[];
extern X86CPU __initdata_x86cpu_e[];

static X86CPU *X86cpu;

static X86CPU *
FindX86Cpu(char *vendor)
{
	const char **id;

	for (X86CPU *c = __initdata_x86cpu_s;
	     c < __initdata_x86cpu_e;
	     c++)
	{
		id = c->Id;
		while (*id)
		{
			if (!strcmp(*id, vendor))
			{
				return c;
			}
			id++;
		}
	}

	panic("cannot find processor");
}

void INIT
X86CpuInit(void)
{
	char cpuvendor[13] = {0};
	u32 a;

	Cpuid(CPUID_0, &a, (u32 *)cpuvendor, (u32 *)(cpuvendor+8), (u32 *)(cpuvendor+4));

	KLOG("CPU Vendor: %s\n", cpuvendor);

	X86cpu = FindX86Cpu(cpuvendor);

	KLOG("Vendor: %s\n", X86cpu->Vendor);

	if (X86cpu->Ops->Init)
	{
		X86cpu->Ops->Init(X86cpu);
	}
}
