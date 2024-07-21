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

#ifndef _ARCH_CPU_H
#define _ARCH_CPU_H

#include <akari/types.h>
#include <akari/cpu.h>
#include <akari/compiler.h>

// #define PERCPU_ENABLE

#define INTR_DISABLE	asm volatile ("cli");
#define INTR_ENABLE	asm volatile ("sti");

// PERCPU data

#ifdef PERCPU_ENABLE

#define PERCPU		SECTION(".data.percpu")

extern char __percpu_data[];
extern char __percpu_data_e[];

extern void *__PerCpuPtr[NCPU];

#define PERCPU_VAR_OFFSET(_v)	((ulong)&(_v) - (ulong)__percpu_data)

#define CPU_VAR(_v, _cpu)	(*(typeof(_v) *)(__PerCpuPtr[(_cpu)] + PERCPU_VAR_OFFSET(_v)))

#define MYCPU(_v)		CPU_VAR(_v, currentcpu)

#else

#define PERCPU

#define PERCPU_VAR_OFFSET(_v)
#define CPU_VAR(_v, _cpu)	_v
#define MYCPU(_v)		_v

#endif	// PERCPU_ENABLE

void InitPerCpu(void) INIT;

#endif	// _ARCH_CPU_H
