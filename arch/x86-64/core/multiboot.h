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

#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#include <akari/compiler.h>

// MultiBoot2

typedef struct MULTIBOOTINFO	MULTIBOOTINFO;

struct MULTIBOOTINFO {
	u32 	Flags;			// offset 0
	u32	MemLower;		// offset 4
	u32	MemUpper;		// offset 8
	u32	BootDevice;		// offset 12
	u32	Cmdline;		// offset 16
	u32	ModsCount;		// offset 20
	u32	ModsAddr;		// offset 24
	u32	Syms[4];		// offset 28-40
	u32	MmapLen;		// offset 44
	u32	MmapAddr;		// offset 48
	u32	DrivesLen;		// offset 52
	u32	DrivesAddr;		// offset 56
	u32	ConfigTable;		// offset 60
	u32	BootLoaderName;		// offset 64
	u32	ApmTable;		// offset 68
	u32	VbeControlInfo;		// offset 72
	u32	VbeModeInfo;		// offset 76
	u16	VbeMode;		// offset 80
	u16	VbeInterfaceSeg;	// offset 82
	u16	VbeInterfaceOff;	// offset 84
	u16	VbeInterfaceLen;	// offset 86
} __packed;

#endif	// _MULTIBOOT_H
