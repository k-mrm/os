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
#include <akari/string.h>

void *
memcpy(void *dst, const void *src, ulong n)
{
	return memmove(dst, src, n);
}

void *
memmove(void *dst, const void *src, ulong n)
{
	char *d = dst;
	const char *s = src;

	if (s > d)
	{
		while (n-- > 0)
			*d++ = *s++;
	}
	else
	{
		d += n;
		s += n;
		while (n-- > 0)
			*--d = *--s;
	}

	return dst;
}

void *
memset(void *dst, int c, ulong n)
{
	char *d = dst;

	while (n-- > 0)
		*d++ = c;

	return dst;
}

char *
strcpy(char *dst, const char *src)
{
	char *r = dst;

	while ((*dst++ = *src++) != 0)
		;

	return r;
}

int
strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s1 == *s2)
	{
		s1++;
		s2++;
	}

	return *s1 - *s2;
}

int
strncmp(const char *s1, const char *s2, ulong len)
{
	while (*s1 && *s1 == *s2 && len > 0)
	{
		s1++;
		s2++;
		len--;
	}
	if (len == 0)
		return 0;

	return *s1 - *s2;
}

uint
strlen(const char *s)
{
	uint i = 0;
	while (*s++)
		i++;

	return i;
}
