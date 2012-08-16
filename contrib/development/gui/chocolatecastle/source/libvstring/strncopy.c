/* libvstring/strncopy.c */

#include "libvstring.h"


STRPTR StrNCopy(STRPTR s, STRPTR d, LONG len)
{
	while (len--)
	{
		if (!(*d++ = *s++)) return (--d);
	}

	*d = 0x00;
	return d;
}

