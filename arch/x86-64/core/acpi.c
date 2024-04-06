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

#include <akari/types.h>
#include <akari/compiler.h>
#include <akari/panic.h>
#include <akari/string.h>
#include <arch/memlayout.h>
#include <acpi.h>

#define KPREFIX		"acpi:"

#include <akari/log.h>

void *Multiboot2Xsdp(void) INIT;
void *Multiboot2Rsdp(void) INIT;

static int rsdpver;
static RSDT *rsdt;
static XSDT *xsdt;

static bool
RsdpCheck(RSDP *rsdp)
{
	KDBG("%s\n", rsdp->Signature);
	return !strncmp(rsdp->Signature, "RSD PTR ", 8);
}

static void *
FindRsdt(char *sig)
{
	SDTHEADER *header = (SDTHEADER *)rsdt;
	SDTHEADER *ent;
	uint nentry;

	nentry = (header->Length - sizeof(*header)) / sizeof(u32);

	for (uint i = 0; i < nentry; i++)
	{
		ent = (SDTHEADER *)P2V(rsdt->Entry[i]);

		if (!strncmp(ent->Signature, sig, 4))
		{
			return ent;
		}
	}

	return NULL;
}

static void *
FindXsdt(char *sig)
{
	SDTHEADER *header = (SDTHEADER *)xsdt;
	SDTHEADER *ent;
	uint nentry;

	nentry = (header->Length - sizeof(*header)) / sizeof(u64);

	for (uint i = 0; i < nentry; i++)
	{
		ent = (SDTHEADER *)P2V(xsdt->Entry[i]);

		if (!strncmp(ent->Signature, sig, 4))
		{
			return ent;
		}
	}

	return NULL;
}

static void
RsdtDump(void)
{
	SDTHEADER *header = (SDTHEADER *)rsdt;
	SDTHEADER *ent;
	uint nentry;

	nentry = (header->Length - sizeof(*header)) / sizeof(u32);

	for (uint i = 0; i < nentry; i++)
	{
		ent = (SDTHEADER *)P2V(rsdt->Entry[i]);
		KDBG("%s (%s %s)\n", ent->Signature,
				     ent->Oemid,
				     ent->OemTableId);
	}
}

static void
XsdtDump(void)
{
	;
}

static void
AcpiDump(void)
{
	if (rsdpver == 1)
	{
		return RsdtDump();
	}
	else if (rsdpver == 2)
	{
		return XsdtDump();
	}
}

static void *
AcpiFind(char *sig)
{
	if (rsdpver == 1)
	{
		return FindRsdt(sig);
	}
	else if (rsdpver == 2)
	{
		return FindXsdt(sig);
	}
	else
	{
		return NULL;
	}
}

void INIT
AcpiInit(void)
{
	RSDP *rsdp;
	XSDP *xsdp;

	rsdp = Multiboot2Xsdp();
	if (rsdp && RsdpCheck(rsdp))
	{
		rsdpver = 2;
		goto found;
	}

	rsdp = Multiboot2Rsdp();
	if (rsdp && RsdpCheck(rsdp))
	{
		rsdpver = 1;
		goto found;
	}

	panic("no acpi table");
found:
	KLOG("found acpi table version%d\n", rsdpver);
	KLOG("acpi (%s %s) rsdt %p\n", rsdp->Signature, rsdp->Oemid, rsdp->RsdtAddress);

	if (rsdpver == 1)
	{
		rsdt = P2V(rsdp->RsdtAddress);
	}
	else if (rsdpver == 2)
	{
		xsdp = (XSDP *)rsdp;
		xsdt = P2V(xsdp->XsdtAddress);
	}

	AcpiDump();
}
