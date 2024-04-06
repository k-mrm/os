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
#include <akari/param.h>
#include <akari/sysmem.h>
#include <akari/init.h>
#include <arch/asm.h>
#include <arch/memlayout.h>

#define KPREFIX	"x86-64/main:"

#include <akari/log.h>

#include <cpuid.h>
#include <x86cpu.h>
#include "serial.h"
#include "multiboot.h"
#include "mm.h"
#include "trap.h"

static void INIT
ParseBootInfo(MULTIBOOT_INFO *mb)
{
	MULTIBOOT_TAG *tag;

	if (!mb)
	{
		return;
	}

	for (tag = (MULTIBOOT_TAG *)((char *)mb + 8);
	     tag->Type != MULTIBOOT_TAG_TYPE_END;
	     tag = (MULTIBOOT_TAG *)((char *)tag + ((tag->Size + 7) & ~7)))
	{
		switch (tag->Type)
		{
		case MULTIBOOT_TAG_TYPE_CMDLINE: {
			MULTIBOOT_TAG_STRING *cmd =
				(MULTIBOOT_TAG_STRING *)tag;
			KDBG("cmdline: %s\n", cmd->String);
			SetParam(cmd->String);
			break;
		}
		case MULTIBOOT_TAG_TYPE_BOOTDEV: {
			MULTIBOOT_TAG_BOOTDEV *dev =
				(MULTIBOOT_TAG_BOOTDEV *)tag;
			KDBG("Boot dev: 0x%x\n", dev->BiosDev);
			break;
		}
		case MULTIBOOT_TAG_TYPE_MMAP: {
			MULTIBOOT_MMAP_ENTRY *e;
			MULTIBOOT_TAG_MMAP *mmap =
				(MULTIBOOT_TAG_MMAP *)tag;

			for (e = mmap->Entries;
			     (char *)e < (char *)mmap + mmap->Size;
			     e = (MULTIBOOT_MMAP_ENTRY *)((char *)e + mmap->EntrySize))
			{
				if (e->Type == MULTIBOOT_MEMORY_AVAILABLE)
				{
					NewMem(e->Addr, e->Len);
				}
				else if (e->Type == MULTIBOOT_MEMORY_RESERVED)
				{
					ReserveMem(e->Addr, e->Len);
				}
			}
			break;	
		}
		default:
			break;
		}
	}
}

static void INIT
X86_64Init(void)
{
	X86CpuInit();
	X86mmInit();
	TrapInit();
}

// bsp main
void NORETURN INIT
x86Main(MULTIBOOT_INFO *mb)
{
	KillIdmap();

	SerialInit();	// earlyconsole

	KLOG("Booting %dbit Kernel...\n", 64);

	ParseBootInfo(mb);

	X86_64Init();

	/*
	 * In x86-64, First 1MB is reserved
	 */
	ReserveMem(0x0, 0x100000);

	KernelMain();
}

// ap main
void NORETURN
kmainap(void)
{
	for (;;)
	{
		HLT;
	}
}
