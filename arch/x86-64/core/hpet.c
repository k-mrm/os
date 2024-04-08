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
#include <akari/sysmem.h>
#include <akari/timer.h>
#include <akari/printk.h>
#include <akari/kalloc.h>
#include <akari/mm.h>

#define KPREFIX		"HPET:"

#include <akari/log.h>

#include "hpet.h"

typedef struct HPETDEV	HPETDEV;

/*
 * HPET Device
 */
struct HPETDEV
{
	void *Base;
	PHYSADDR BasePa;
};

int INIT
HpetProbe(TIMER *tm)
{
	HPETDEV *hpet = tm->Device;	
	void *base;

	base = KIOmap(hpet->BasePa, 1024);
	if (!base)
	{
		return -1;
	}

	hpet->Base = base;

	KLOG("%s Probed\n", tm->Name);

	return 0;
}

static TIMER tmhpet = {
	.Probe = HpetProbe,
};

void INIT
HpetInit(PHYSADDR baseaddr, int n)
{
	HPETDEV *dev;

	ReserveMem(baseaddr, 1024);

	dev = BootmemAlloc(sizeof *dev, _Alignof(*dev));

	if (!dev)
	{
		return;
	}

	dev->BasePa = baseaddr;

	tmhpet.Device = dev;
	sprintf(tmhpet.Name, "HPET%d", n);	

	NewTimer(&tmhpet);
}
