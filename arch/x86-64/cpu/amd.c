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
#include <x86cpu.h>
#include <cpuid.h>

#define KPREFIX		"AMD:"

#include <akari/log.h>

static void
X86AMDInit(X86CPU *cpu)
{
	char procstr[48] = {0};

	Cpuid(CPUID_EXT2,
	      (u32 *)procstr, (u32 *)(procstr+4), (u32 *)(procstr+8), (u32 *)(procstr+12));
	Cpuid(CPUID_EXT3,
	      (u32 *)(procstr+16), (u32 *)(procstr+20), (u32 *)(procstr+24), (u32 *)(procstr+28));
	Cpuid(CPUID_EXT4,
	      (u32 *)(procstr+32), (u32 *)(procstr+36), (u32 *)(procstr+40), (u32 *)(procstr+44));

	KLOG("Processor: %s\n", procstr);
}

static X86CPUOPS amdops = {
	.Init = X86AMDInit,
};

static const char *amdid[] = {
	"AuthenticAMD",
	"AMDisbetter!",
	NULL,
};

DEFINE_X86_CPU(AMD, amdid, &amdops);
