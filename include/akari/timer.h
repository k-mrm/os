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

#ifndef _TIMER_H
#define _TIMER_H

#include <akari/types.h>
#include <akari/compiler.h>
#include <akari/irqsource.h>

typedef struct TIMER		TIMER;
typedef struct EVENTTIMER	EVENTTIMER;

struct TIMER
{
	void *Device;
	char Name[16];
	bool Global;

	int (*Probe)(TIMER *tm);
	void (*Disable)(TIMER *tm);

	ulong (*uSec2Period)(TIMER *tm, uint usec);
	ulong (*ReadCounterRaw)(TIMER *tm);
};

struct EVENTTIMER
{
	void *Device;
	char Name[16];
	bool Global;

	int (*Probe)(EVENTTIMER *et);

	uint (*GetPeriod)(EVENTTIMER *et);
	void (*SetPeriod)(EVENTTIMER *et, uint ms);

	void (*On)(EVENTTIMER *et);
	void (*Off)(EVENTTIMER *et);

	int (*IRQHandler)(EVENTTIMER *et);

	IRQSOURCE *Irq;
};

void mSleep(uint msec);
void uSleep(uint usec);

void TimerInit(void) INIT;
void NewTimer(TIMER *tm) INIT;
void NewEventTimer(EVENTTIMER *et) INIT;

int NewEventTimerIrq(EVENTTIMER *et, int irqno);

#endif	// _TIMER_H
