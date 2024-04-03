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

#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned long 	u64;
typedef long 		i64;
typedef unsigned int 	u32;
typedef signed int 	i32;
typedef unsigned short 	u16;
typedef signed short 	i16;
typedef unsigned char 	u8;
typedef signed char 	i8;
typedef unsigned int	uint;
typedef unsigned long 	ulong;

/* physical address */
typedef ulong 	PHYSADDR;
typedef ulong	DMAADDR;

#define NULL 	((void *)0)

typedef ulong	*PAGETABLE;
typedef ulong	PTE;

typedef _Bool	bool;

#define true 	1
#define false 	0

#define offsetof(st, m) ((u64)((char *)&((st *)0)->m - (char *)0))

#define container_of(ptr, st, m)  \
  ({ const typeof(((st *)0)->m) *_mptr = (ptr); \
     (st *)((char *)_mptr - offsetof(st, m)); })

#define MAX(a, b)	((a) < (b) ? (b) : (a))
#define MIN(a, b)	((a) > (b) ? (b) : (a))

#define	KiB	(1024)
#define	MiB	(1024 * 1024)
#define	GiB	(1024 * 1024 * 1024)

#endif	// _TYPES_H
