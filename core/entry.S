/*
 * Copyright (c) 2023, akarilab.net
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

#include <akarivmm/asm.h>

#define MULTIBOOT2_MAGIC	0xe85250d6
#define MULTIBOOT2_FLAGS	0		// 32bit i386
#define MULTIBOOT2_LENGTH	(header_end - multiboot_header)
#define CHECKSUM		0x100000000 - (MULTIBOOT2_MAGIC + MULTIBOOT2_FLAGS + MULTIBOOT2_LENGTH)

.p2align 2

.section ".text.head"

multiboot_header:
	.long	MULTIBOOT2_MAGIC
	.long	MULTIBOOT2_FLAGS
	.long	MULTIBOOT2_LENGTH
	.long	CHECKSUM

	.word	0
	.word	0
	.long	8
header_end:

.section ".text.boot"
.code32
.global entry32
entry32:
	cli

	// enable PAE
	movl	%cr4, %eax
	orl	$CR4_PAE, %eax
	movl	%eax, %cr4

	// init sp
	movl	$0x7c00, %eax
	movl	%eax, %esp

	// init pgt

	// enable long mode
	movl	$0xc0000080, %ecx
	rdmsr
	orl	$EFER_LME, %eax
	wrmsr

	movl	entry64, %eax
	jmp	*%eax

.code64
.global entry64
entry64:
	call	vmm_main_bsp

	// never return here!
	jmp	.

zerobss:
	ret
