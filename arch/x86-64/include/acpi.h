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

#ifndef _X86_ACPI_H
#define _X86_ACPI_H

#include <akari/types.h>
#include <akari/compiler.h>

typedef struct RSDP		RSDP;
typedef struct XSDP		XSDP;
typedef struct SDTHEADER	SDTHEADER;
typedef struct RSDT		RSDT;
typedef struct XSDT		XSDT;

struct RSDP
{
	char Signature[8];
	u8 Checksum;
	char Oemid[6];
	u8 Revision;
	u32 RsdtAddress;
} PACKED;

struct XSDP
{
	RSDP V1;

	u32 Length;
	u64 XsdtAddress;
	u8 XChecksum;
	u8 _Rsrv[3];
} PACKED;

struct SDTHEADER
{
	char Signature[4];
	u32 Length;
	u8 Revision;
	u8 Checksum;
	char Oemid[6];
	char OemTableId[8];
	u32 OemRevision;
	u32 CreatorId;
	u32 CreatorRevision;
} PACKED;

struct RSDT
{
	SDTHEADER header;
	u32 Entry[];
};

struct XSDT
{
	SDTHEADER header;
	u64 Entry[];
};

#endif	// _X86_ACPI_H
