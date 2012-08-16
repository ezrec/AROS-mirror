/* libvstring/strcopy.c */

#include "libvstring.h"


STRPTR StrCopy(STRPTR s, STRPTR d)
{
	while (*d++ = *s++);

	return (--d);
}

