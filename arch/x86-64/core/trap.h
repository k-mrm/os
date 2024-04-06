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

#ifndef _X86_CORE_TRAP_H
#define _X86_CORE_TRAP_H

#include <akari/types.h>
#include <akari/compiler.h>

#define NR_INTERRUPT		256

#define DPL_KERNEL	0
#define DPL_USER	3

#define	INT_NMI		2

// Exceptions
#define E_DE		0x0
#define E_DB		0x1
#define E_BP		0x3
#define E_OF		0x4
#define E_BR		0x5
#define E_UD		0x6
#define E_NM		0x7
#define E_DF		0x8
#define E_TS		0xa
#define E_NP		0xb
#define E_SS		0xc
#define E_GP		0xd
#define E_PF		0xe

typedef struct GATEDESC		GATEDESC;
typedef enum GATETYPE		GATETYPE;
typedef struct X86TRAPFRAME	X86TRAPFRAME;

enum GATETYPE
{
	GATEDESC_64_INTR = 0xe,
	GATEDESC_64_TRAP = 0xf,
};

/*
 * x86 Gate Descriptor
 */
struct GATEDESC
{
	u16 Offset_0_15;
	u16 Sel;
	u8 Ist: 3;
	u8 _Rsrv0: 5;
	u8 Gatetype: 4;
	u8 _Zero: 1;
	u8 Dpl: 2;
	u8 P: 1;
	u16 Offset_16_31;
	u32 Offset_32_63;
	u32 _Rsrv1;
} PACKED;

struct X86TRAPFRAME
{
	u64 Rax;
	u64 Rbx;
	u64 Rcx;
	u64 Rdx;
	u64 Rbp;
	u64 Rsi;
	u64 Rdi;
	u64 R8;
	u64 R9;
	u64 R10;
	u64 R11;
	u64 R12;
	u64 R13;
	u64 R14;
	u64 R15;
	u64 Trapno;
	u64 Errcode;
	/* iret */
	u64 Rip;
	u64 Cs;
	u64 Rflags;
	u64 Rsp;
	u64 Ss;
} PACKED;

void TrapInit(void) INIT;

#endif	// _X86_CORE_TRAP_H
