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
} PACKED;

struct XSDT
{
	SDTHEADER header;
	u64 Entry[];
} PACKED;

/*
 * FACP
 */
typedef struct FADT	FADT;

struct FADT
{
	SDTHEADER header;
} PACKED;

/*
 * MADT
 */

typedef struct MADTENTRY		MADTENTRY;
typedef struct MADT_LOCALAPIC		MADT_LOCALAPIC;
typedef struct MADT_IOAPIC		MADT_IOAPIC;
typedef struct MADT_LOCAL_X2APIC	MADT_LOCAL_X2APIC;
typedef struct MADT			MADT;

#define APIC_TYPE_LOCALAPIC			0
#define APIC_TYPE_IOAPIC			1
#define APIC_TYPE_LOCAL_X2APIC			9

struct MADTENTRY
{
	u8 Type;
	u8 Length;
};

struct MADT_LOCALAPIC
{
	MADTENTRY Header;

	u8 ProcId;	// Processor ID
	u8 ApicId;	// Local APIC ID
	u32 Flags;
};

struct MADT_IOAPIC
{
	MADTENTRY Header;

	u8 IoapicId;
	u8 _Zero;
	u32 IoapicAddr;
	u32 IntrBase;
};

struct MADT_LOCAL_X2APIC
{
	MADTENTRY Header;

	u16 _Rsrv;
	u32 X2apicId;
	u32 Flags;
	u32 AcpiId;
};

struct MADT
{
	SDTHEADER Header;
	u32 LapicAddr;
	u32 Flags;

	MADTENTRY Table[];
} PACKED;

/*
 * General Address Structure
 */
#define ACPI_SPACE_SYSTEM_MEMORY	0x0
#define ACPI_SPACE_SYSTEM_IO		0x1
#define ACPI_SPACE_PCI_CONFIG		0x2
#define ACPI_SPACE_PCI_BAR		0x6

typedef struct GAS		GAS;

struct GAS
{
	u8 Space;
	u8 BitWidth;
	u8 BitOffset;
	u8 AccessSize;
	u64 Address;
} PACKED;

/*
 * HPET
 */
typedef struct HPET		HPET;

struct HPET
{
	SDTHEADER Header;
	u32 HardwareId;
	GAS Address;
	u8 Number;
	u16 MinTick;
	u8 Flags;
} PACKED;

void AcpiInit(void) INIT;

#endif	// _X86_ACPI_H
