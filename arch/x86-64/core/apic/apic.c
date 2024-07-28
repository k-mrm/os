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
#include <akari/timer.h>
#include <akari/sysmem.h>
#include <akari/irqsource.h>

#define KPREFIX		"apic:"

#include <akari/log.h>
#include <cpuid.h>

#include <arch/cpu.h>

#include "apic.h"

#define ID		0x020

#define VER		0x030

#define TPR		0x080

#define EOI		0x0b0

#define SPIV		0x0f0
#define SPIV_APIC_ENABLED	(1 << 8)

#define ESR		0x280

#define ICR_LOW		0x300

#define ICR_HIGH	0x310

#define LVT_TIMER	0x320
#define LVT_TIMER_INT_MASK		(1 << 16)
#define LVT_TIMER_ONE_TIME		(0 << 17)
#define LVT_TIMER_PERIODIC		(1 << 17)
#define LVT_TIMER_TSC_DEADLINE		(2 << 17)

#define TM_INIT		0x380

#define TM_CURRENT	0x390

#define TM_DIV		0x3e0


typedef struct LAPICTIMER	LAPICTIMER;

struct LAPICTIMER
{
	APIC *Apic;

	uint Freq;
	uint Periodms;
};

APIC *Apic PERCPU;

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

static int
ApicTimerMeasureFreq(LAPICTIMER *lt)
{
	APIC *apic = lt->Apic;
	u32 cnt, cnt2;

	cnt = apic->Read(TM_CURRENT);

	// sleep 1s
	mSleep(1000);

	cnt2 = apic->Read(TM_CURRENT);

	if (cnt <= cnt2)
	{
		KWARN("lapic timer?");
		return -1;
	}

	lt->Freq = cnt - cnt2;

	KDBG ("lapic timer %x -> %x freq:%d\n", cnt, cnt2, lt->Freq);

	return 0;
}

static int
ApicTimerProbe(EVENTTIMER *et)
{
	LAPICTIMER *lt = et->Device;
	APIC *apic = lt->Apic;
	int err;
	u32 lvt = 0;

	KDBG ("Probe lapic timer: %s\n", et->Name);

	lvt |= LVT_TIMER_INT_MASK;
	lvt |= 0x40;

	apic->Write(LVT_TIMER, lvt);

	// Enable Timer
	apic->Write(TM_INIT, 0xffffffff);

	err = ApicTimerMeasureFreq(lt);

	if (err)
	{
		return -1;
	}

	lvt = LVT_TIMER_PERIODIC;
	// lvt |= LVT_TIMER_INT_MASK;
	lvt |= 0x40;		// periodic mode, vector is 0x20

	apic->Write(LVT_TIMER, lvt);

	// test
	apic->Write(TM_INIT, lt->Freq * 3);

	err = NewEventTimerIrq(et, 0x40);

	if (err)
	{
		KDBG("neweventtimerirq\n");
		return -1;
	}

	return 0;
}

static void
ApicTimerOn(EVENTTIMER *et)
{
	LAPICTIMER *lt = et->Device;
	APIC *apic = lt->Apic;
	u32 lvt;

	lvt = apic->Read(LVT_TIMER);

	lvt &= ~LVT_TIMER_INT_MASK;	// MASK bit

	apic->Write(LVT_TIMER, lvt);
}

static void
ApicTimerOff(EVENTTIMER *et)
{
	LAPICTIMER *lt = et->Device;
	APIC *apic = lt->Apic;
	u32 lvt;

	lvt = apic->Read(LVT_TIMER);

	lvt |= LVT_TIMER_INT_MASK;	// MASK bit

	apic->Write(LVT_TIMER, lvt);
}

static uint
ApicTimerGetPeriod(EVENTTIMER *et)
{
	LAPICTIMER *lt = et->Device;

	return lt->Periodms;
}

static void
ApicTimerSetPeriod(EVENTTIMER *et, uint ms)
{
	LAPICTIMER *lt = et->Device;

	lt->Periodms = ms;
}

static int
ApicTimerIrq(EVENTTIMER *et)
{
	KLOG("APIC TIMER IRQ\n");

	// TODO
	return -1;
}

static EVENTTIMER lapictimer = {
	.Global = true,
	.GetPeriod = ApicTimerGetPeriod,
	.SetPeriod = ApicTimerSetPeriod,
	.Probe = ApicTimerProbe,
	.On = ApicTimerOn,
	.Off = ApicTimerOff,
	.IRQHandler = ApicTimerIrq,
};

static void
EnableApic(void)
{
	u32 spiv;

	spiv = Apic->Read(SPIV);

	spiv |= SPIV_APIC_ENABLED;

	Apic->Write(SPIV, spiv);
}

static void
DisableApic(void)
{
	u32 spiv;

	spiv = Apic->Read(SPIV);

	spiv &= ~SPIV_APIC_ENABLED;

	Apic->Write(SPIV, spiv);
}

static void INIT
ApicSetSpiv(void)
{
	u32 spiv;

	spiv = 0x20;	// vector is 0x20

	Apic->Write(SPIV, spiv);
}

void INIT
ApicInit0(void)
{
	LAPICTIMER *timer;

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

	// Set Sprious Interrupt Vector
	ApicSetSpiv();

	// Clear Error Status
	Apic->Write(ESR, 0);

	Apic->Write(TPR, 0);

	EnableApic();

	timer = BootmemAlloc(sizeof *timer, _Alignof(*timer));

	if (!timer)
	{
		KWARN("cannot initialize lapic timer\n");
		return;
	}

	timer->Apic = Apic;

	lapictimer.Device = timer;
	sprintf(lapictimer.Name, "LAPICTimer%d", 0);	// TODO: CPUID

	NewEventTimer(&lapictimer);
}

void INIT
ApicInitAp(void)
{
	;
}
