/*
 * Copyright (c) 2024, akarilab.net
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * 1. Redistributions of source code must retain the above copyright notice, this
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
#include <akari/timer.h>
#include <akari/panic.h>
#include <akari/cpu.h>

#include <arch/cpu.h>

#define MSEC2USEC	1000

TIMER *STimer;

TIMER *Timer PERCPU;
EVENTTIMER *EventTimer PERCPU;

static TIMER *tdbGlobal[16];
static int ntdb = 0;

static EVENTTIMER *etdbGlobal[16];
static int netdb = 0;

static inline TIMER *
SysTimer(void)
{
	return STimer;
}

void
mSleep(uint msec)
{
	uint usec = msec * MSEC2USEC;

	uSleep(usec);
}

void
uSleep(uint usec)
{
	TIMER *timer = SysTimer();
	ulong now, after;

	now = timer->ReadCounterRaw(timer);
	after = now + timer->uSec2Period(timer, usec);

	while (timer->ReadCounterRaw(timer) < after)
		;
}

static void
GlobalEventTimerInit(void)
{
	EVENTTIMER *et;
	int err;

	for (int i = 0; i < netdb; i++)
	{
		et = etdbGlobal[i];
		if (et && et->Probe)
		{
			err = et->Probe(et);
			if (!err)
			{
				break;
			}
		}
	}
}

static void
GlobalTimerInit(void)
{
	TIMER *t;
	int err;

	for (int i = 0; i < ntdb; i++)
	{
		t = tdbGlobal[i];
		if (t && t->Probe)
		{
			err = t->Probe(t);
			if (!err)
			{
				STimer = t;
				break;
			}
		}
	}
}

void INIT
TimerInit(void)
{
	GlobalTimerInit();
	GlobalEventTimerInit();
}

void INIT
TimerInitCpu(void)
{
	;
}

void INIT
NewEventTimer(EVENTTIMER *et)
{
	if (et->Global)
	{
		etdbGlobal[netdb++] = et;
	}
	else
	{
		// TODO
	}
}

void INIT
NewTimer(TIMER *tm)
{
	if (tm->Global)
	{
		tdbGlobal[ntdb++] = tm;
	}
	else
	{
		// TODO
	}
}
