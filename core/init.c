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
#include <akari/init.h>
#include <akari/kalloc.h>
#include <akari/panic.h>
#include <akari/mm.h>
#include <arch/memlayout.h>

#define KPREFIX	"init:"

#include <akari/log.h>

void INIT
ReserveKernelArea(void)
{
	PHYSADDR kstartpa, kendpa;
	ulong ksize;

	kstartpa = V2P(KernelStart());
	kendpa = V2P(KernelEnd());
	ksize = PAGEALIGN(kendpa - kstartpa);

	KDBG("kernel image @%p-%p\n", kstartpa, kendpa);

	ReserveMem(kstartpa, ksize);
}

void INIT NORETURN
KernelMain(void)
{
	ReserveKernelArea();

	/*
	 * Kernel early mapping is 0-1GiB
	 */
	KallocInitEarly(0x0, 1 * GiB);

	/*
	 * Make mapping of Kernel Virtual Address Space
	 */
	KvasMap();

	KallocInit();

	TTYInit();

#ifdef DBGHELLO
	KDBG("Kernel Hello!\n");
#endif	// DBGHELLO
	*(u16 *)0x00ff114514 = 0;

	panic("KernelMain Exit");
}
