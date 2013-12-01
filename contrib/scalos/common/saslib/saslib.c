// saslib.c
// $Date$

#include <ctype.h>
#include <string.h>
#include "saslib.h"


#ifndef __SASC
// Replacement for SAS/C library functions

char *stpblk(const char *q)
{
	while (q && *q && isspace((int) *q))
		q++;

	return (char *) q;
}

int stccpy(char *dest, const char *src, int MaxLen)
{
	int Count = 0;

	while (*src && MaxLen > 1)
		{
		*dest++ = *src++;
		MaxLen--;
		Count++;
		}
	*dest = '\0';
	Count++;

	return Count;
}

void strins(char *to, const char *from)
{
	size_t fromLength = strlen(from);
	char *dest = to + fromLength;

	memmove(dest, to, 1 + strlen(to));
	strncpy(to, from, fromLength);
}
#endif /* __SASC */
