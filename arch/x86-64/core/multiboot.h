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

// MultiBoot1/MultiBoot2

typedef struct MULTIBOOT_INFO	MULTIBOOT_INFO;
typedef struct MULTIBOOT1_INFO	MULTIBOOT1_INFO;

// MultiBoot1
struct MULTIBOOT1_INFO
{
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
} PACKED;

#define MULTIBOOT_TAG_ALIGN                  8
#define MULTIBOOT_TAG_TYPE_END               0
#define MULTIBOOT_TAG_TYPE_CMDLINE           1
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME  2
#define MULTIBOOT_TAG_TYPE_MODULE            3
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO     4
#define MULTIBOOT_TAG_TYPE_BOOTDEV           5
#define MULTIBOOT_TAG_TYPE_MMAP              6
#define MULTIBOOT_TAG_TYPE_VBE               7
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER       8
#define MULTIBOOT_TAG_TYPE_ELF_SECTIONS      9
#define MULTIBOOT_TAG_TYPE_APM               10
#define MULTIBOOT_TAG_TYPE_EFI32             11
#define MULTIBOOT_TAG_TYPE_EFI64             12
#define MULTIBOOT_TAG_TYPE_SMBIOS            13
#define MULTIBOOT_TAG_TYPE_ACPI_OLD          14
#define MULTIBOOT_TAG_TYPE_ACPI_NEW          15
#define MULTIBOOT_TAG_TYPE_NETWORK           16
#define MULTIBOOT_TAG_TYPE_EFI_MMAP          17
#define MULTIBOOT_TAG_TYPE_EFI_BS            18
#define MULTIBOOT_TAG_TYPE_EFI32_IH          19
#define MULTIBOOT_TAG_TYPE_EFI64_IH          20
#define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR    21

typedef struct MULTIBOOT_TAG		MULTIBOOT_TAG;
typedef struct MULTIBOOT_TAG_STRING	MULTIBOOT_TAG_STRING;
typedef struct MULTIBOOT_TAG_MMAP	MULTIBOOT_TAG_MMAP;
typedef struct MULTIBOOT_TAG_BOOTDEV	MULTIBOOT_TAG_BOOTDEV;
typedef struct MULTIBOOT_MMAP_ENTRY	MULTIBOOT_MMAP_ENTRY;
typedef struct MULTIBOOT_TAG_ACPI_OLD	MULTIBOOT_TAG_ACPI_OLD;
typedef struct MULTIBOOT_TAG_ACPI_NEW	MULTIBOOT_TAG_ACPI_NEW;

struct MULTIBOOT_INFO
{
	u32	TotalSize;
	u32	_Rsvd;
} PACKED;

struct MULTIBOOT_TAG
{
	u16	Type;
	u16	Flags;
	u32	Size;
} PACKED;

struct MULTIBOOT_TAG_STRING
{
	u32	Type;
	u32	Size;
	char	String[0];
} PACKED;

struct MULTIBOOT_TAG_BOOTDEV
{
	u32	Type;
	u32	Size;
	u32	BiosDev;
	u32	Slice;
	u32	Part;
} PACKED;

struct MULTIBOOT_MMAP_ENTRY
{
	u64	Addr;
	u64	Len;
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
	u32	Type;
	u32	Zero;
} PACKED;

struct MULTIBOOT_TAG_MMAP
{
	u32	Type;
	u32	Size;
	u32	EntrySize;
	u32	EntryVersion;

	MULTIBOOT_MMAP_ENTRY	Entries[0];
} PACKED;

struct MULTIBOOT_TAG_ACPI_OLD
{
	u32	Type;
	u32	Size;
	u8	Rsdp;
} PACKED;

struct MULTIBOOT_TAG_ACPI_NEW
{
	u32	Type;
	u32	Size;
	u8	Xsdp;
} PACKED;

#endif	// _MULTIBOOT_H
