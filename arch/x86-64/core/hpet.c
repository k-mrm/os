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

#define HPET_MMIO_SIZE		1024

#define HPET_ID			0x0
#define HPET_CLK_PERIOD		0x4
#define HPET_GCR		0x10
#define HPET_MCR		0xf0

typedef struct HPETDEV	HPETDEV;

/*
 * HPET Device
 */
struct HPETDEV
{
	void *Base;
	PHYSADDR BasePa;
	
	bool Cnt64;
	uint nChannel;
	uint Periodfs;	// 10(^-15) s	
};

static inline void
HpetWr32(HPETDEV *hpet, ulong offset, u32 val)
{
	*(volatile u32 *)(hpet->Base + offset) = val;
}

static inline u32
HpetRd32(HPETDEV *hpet, ulong offset)
{
	return *(volatile u32 *)(hpet->Base + offset);
}

static inline u64
HpetRd64(HPETDEV *hpet, ulong offset)
{
	return *(volatile u64 *)(hpet->Base + offset);
}

static void
HpetCtrl(HPETDEV *hpet, bool en)
{
	u32 gcr;

	gcr = HpetRd32(hpet, HPET_GCR);

	if (!!(gcr & 1) == en)
	{
		return;
	}

	gcr |= en ? 1 : 0;

	HpetWr32(hpet, HPET_GCR, gcr);
}

#define USEC2FSEC	1000000000
static ulong
HpetuSec2Period(TIMER *tm, uint usec)
{
	HPETDEV *hpet = tm->Device;
	ulong fsec = (ulong)usec * USEC2FSEC;
	ulong period = fsec / hpet->Periodfs;

	return period;
}

static ulong
HpetGetCounter(HPETDEV *hpet)
{
	return HpetRd64(hpet, HPET_MCR);
}

static ulong
HpetReadCounterRaw(TIMER *tm)
{
	return HpetGetCounter(tm->Device);
}

static int
HpetIrq(TIMER *tm)
{
	return 1;
}

static bool
HpetIsDead(HPETDEV *hpet)
{
	u64 now, after;

	now = HpetGetCounter(hpet);

	for (int i = 0; i < 1000; i++)	// busy loop
		;

	after = HpetGetCounter(hpet);

	return now >= after;
}

int INIT
HpetProbe(TIMER *tm)
{
	HPETDEV *hpet = tm->Device;
	u32 id;
	u32 clkperiod;
	bool dead;
	void *base;

	base = KIOmap(hpet->BasePa, HPET_MMIO_SIZE);
	if (!base)
	{
		return -1;
	}

	hpet->Base = base;

	HpetCtrl(hpet, false);

	id = HpetRd32(hpet, HPET_ID);
	clkperiod = HpetRd32(hpet, HPET_CLK_PERIOD);

	hpet->Cnt64 = !!(id & (1 << 13));
	hpet->nChannel = (id >> 8) & 0x1f;
	hpet->Periodfs = clkperiod;

	KLOG("%s: %d channel(s) clock period: %d ns %d bit counter\n",
	     tm->Name, hpet->nChannel, clkperiod / 1000000, hpet->Cnt64 ? 64 : 32);

	HpetCtrl(hpet, true);

	KDBG("HPET %p\n", HpetGetCounter(hpet));

	dead = HpetIsDead(hpet);

	if (dead)
	{
		KLOG("%s: Dead HPET\n", tm->Name);
		goto err;
	}

	return 0;

err:
	HpetCtrl(hpet, false);
	return -1;
}

static TIMER tmhpet = {
	.Probe = HpetProbe,
	.ReadCounterRaw = HpetReadCounterRaw,
	.uSec2Period = HpetuSec2Period,
	.IrqHandler = HpetIrq,
};

void INIT
HpetInit(PHYSADDR baseaddr, int n)
{
	HPETDEV *dev;

	ReserveMem(baseaddr, HPET_MMIO_SIZE);

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
