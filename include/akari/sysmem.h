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

#ifndef _SYSMEM_H
#define _SYSMEM_H

#include <akari/types.h>

typedef struct MEMBLOCK		MEMBLOCK;
typedef struct MEMCHUNK		MEMCHUNK;
typedef struct SYSMEM		SYSMEM;

struct MEMBLOCK
{
	PHYSADDR Base;
	ulong Size;
};

struct MEMCHUNK
{
	char *Name;
	uint nBlock;
	MEMBLOCK Block[32];
};

struct SYSMEM
{
	MEMCHUNK Avail;
	MEMCHUNK Rsrv;
};

extern SYSMEM Sysmem;

#define FOREACH_SYSMEM_AVAIL_BLOCK(block)	\
	for (block = Sysmem.Avail.Block;	\
	     block < &Sysmem.Avail.Block[Sysmem.Avail.nBlock];	\
	     block++)

void NewMem(PHYSADDR base, u64 size);
void ReserveMem(PHYSADDR base, u64 size);

#endif	// _SYSMEM_H
