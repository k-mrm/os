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
#include <akari/printk.h>
#include <akari/panic.h>
#include <akari/param.h>
#include <akari/sysmem.h>
#include <x86-64/asm.h>

#include "serial.h"
#include "multiboot.h"
#include "mm.h"

static void
ParseBoot(MULTIBOOT_INFO *mb)
{
	MULTIBOOT_TAG *tag;

	if (!mb)
		return;

	for (tag = (MULTIBOOT_TAG *)((char *)mb + 8);
	     tag->Type != MULTIBOOT_TAG_TYPE_END;
	     tag = (MULTIBOOT_TAG *)((char *)tag + ((tag->Size + 7) & ~7))) {
		switch (tag->Type) {
		case MULTIBOOT_TAG_TYPE_CMDLINE: {
			MULTIBOOT_TAG_STRING *cmd =
				(MULTIBOOT_TAG_STRING *)tag;
			printk("cmdline: %s\n", cmd->String);
			SetParam(cmd->String);
			break;
		}
		case MULTIBOOT_TAG_TYPE_BOOTDEV: {
			MULTIBOOT_TAG_BOOTDEV *dev =
				(MULTIBOOT_TAG_BOOTDEV *)tag;
			printk("Boot dev: 0x%x\n", dev->BiosDev);
			break;
		}
		case MULTIBOOT_TAG_TYPE_MMAP: {
			MULTIBOOT_MMAP_ENTRY *e;
			MULTIBOOT_TAG_MMAP *mmap =
				(MULTIBOOT_TAG_MMAP *)tag;

			for (e = mmap->Entries;
			     (char *)e < (char *)mmap + mmap->Size;
			     e = (MULTIBOOT_MMAP_ENTRY *)((char *)e + mmap->EntrySize)) {
				printk ("base: %p-%p type:%x\n", e->Addr, e->Addr+e->Len,
							         e->Type);
				if (e->Type == MULTIBOOT_MEMORY_AVAILABLE) {
					NewMemblock(e->Addr, e->Len);
				}
			}
			break;	
		}
		default:
			break;
		}
	}
}

// bsp main
void __noreturn
kmain0(MULTIBOOT_INFO *mb)
{
	KillEarlyMap();
	SerialInit();
	printk("Hello %dbit\n", 64);

	ParseBoot(mb);
	KernelRemap();

	for (;;)
		HLT;

	panic("kmain exit!");
}

// ap main
void __noreturn
kmainap(void)
{
	for (;;)
		HLT;
}
