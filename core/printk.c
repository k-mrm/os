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
#include <akari/printk.h>
#include <akari/stdarg.h>
#include <akari/string.h>
#include <akari/console.h>

static uint
sprintiu32(char *p, i32 num, int base, bool sign)
{
	char buf[sizeof(num) * 8 + 1] = {0};
	char *end = buf + sizeof(buf);
	char *cur = end - 1;
	bool neg = false;
	u32 unum;
	uint n = 0;

	if (sign && num < 0) {
		unum = (u32)(-(num + 1)) + 1;
		neg = true;
	} else {
		unum = (u32)num;
	}

	do {
		*--cur = "0123456789abcdef"[unum % base];
		n++;
	} while (unum /= base);

	if (neg) {
		*--cur = '-';
		n++;
	}

	memcpy(p, cur, n);
	return n;
}

static uint
sprintiu64(char *p, i64 num, int base, bool sign)
{
	char buf[sizeof(num) * 8 + 1] = {0};
	char *end = buf + sizeof(buf);
	char *cur = end - 1;
	bool neg = false;
	u64 unum;
	uint n = 0;

	if (sign && num < 0) {
		unum = (u64)(-(num + 1)) + 1;
		neg = true;
	} else {
		unum = (u64)num;
	}

	do {
		*--cur = "0123456789abcdef"[unum % base];
		n++;
	} while (unum /= base);

	if (neg) {
		*--cur = '-';
		n++;
	}

	memcpy (p, cur, n);
	return n;
}

int
vsprintk(char *buf, const char *fmt, va_list ap)
{
	uint n = 0;
	uint len;
	char c;

	for (int i = 0; fmt[i]; i++) {
		c = fmt[i];
		if (c == '%') {
			c = fmt[++i];

			switch (c) {
			case 'd':
				len = sprintiu32(buf + n, va_arg(ap, i32), 10, true);
				n += len;
				break;
			case 'u':
				len = sprintiu32(buf + n, va_arg(ap, u32), 10, false);
				n += len;
				break;
			case 'x':
				len = sprintiu64(buf + n, va_arg(ap, u64), 16, false);
				n += len;
				break;
			case 'p':
				len = sprintiu64(buf + n, va_arg(ap, u64), 16, false);
				n += len;
				break;
			case 'c': {
				int ch = va_arg(ap, int);
				buf[n++] = ch;
				break;
			}
			case 's': {
				char *s = va_arg(ap, char *);
				if (s == NULL)
					s = "(null)";

				len = strlen(s);

				memcpy(buf + n, s, len);
				n += len;
				break;
			}
			case '%':
				buf[n++] = '%';
				break;
			default:
				buf[n++] = '%';
				buf[n++] = c;
				break;
			}
		} else {
			buf[n++] = c;
		}
	}

	return n;
}

int
printk(const char *fmt, ...)
{
	va_list ap;
	char buf[512] = {0};
	int n;

	va_start(ap, fmt);

	n = vsprintk(buf, fmt, ap);

	va_end(ap);

	ConsoleWrite(buf, n);

	return n;
}
