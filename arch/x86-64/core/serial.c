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

// serial port device driver

#include <akari/types.h>
#include <akari/console.h>
#include <arch/asm.h>

#define	COM1	0x3f8
#define	COM2	0x2f8

#define DATA	0x0
#define IER	0x1
#define DLL	0x0
#define DLH	0x1
#define IIR	0x2
#define FCR	0x2
#define LCR	0x3
#define MCR	0x4
#define LSR	0x5
#define MSR	0x6

#define DLAB	0x80

typedef struct COM	COM;
struct COM
{
	u16 Port;
	u32 Baud;
};

static COM com = {
	.Port = COM1,
	.Baud = 115200,
};

static int
cominit(CONSOLE *cs)
{
	COM *com = cs->Priv;
	u16 port = com->Port;
	u16 div = 115200 / com->Baud;

	// Disable Interrupt
	outb(port + IER, 0x0);
	// Disable FIFO
	outb(port + FCR, 0x0);
	// Baudrate
	outb(port + LCR, 0x80);
	outb(port + DLL, div & 0xff);
	outb(port + DLH, (div >> 8) & 0xff);
	// RTS | DTR
	outb(port + MCR, 0x3);

	// 8n1
	outb(port + LCR, 0x3);

	return 0;
}

static bool
comempty(COM *com)
{
	return inb(com->Port + LSR) & 0x20;
}

static void
comsend(COM *com, char c)
{
	while (comempty(com) == 0)
		;

	outb(com->Port + DATA, c);
}

static void
computc(CONSOLE *cs, char c)
{
	COM *com = cs->Priv;

	if (c == '\n')
	{
		comsend(com, '\r');
	}
	comsend(com, c);
}

static void
comwrite(CONSOLE *cs, const char *buf, uint n)
{
	for (uint i = 0; i < n && buf[i]; i++)
	{
		computc(cs, buf[i]);
	}
}

static void
comread(CONSOLE *cs, char *buf, uint n)
{
	;
}

static CONSOLE console = {
	.Name = "COM",
	.Priv = &com,
	.Init = cominit,
	.Write = comwrite,
	.Read = comread,
};

void
SerialInit(void)
{
	ConsoleRegister(&console);
}
