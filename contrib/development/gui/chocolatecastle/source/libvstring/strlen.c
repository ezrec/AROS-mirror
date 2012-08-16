/* libvstring/strlen.c */

#include "libvstring.h"


ULONG StrLen(STRPTR s)
{
	STRPTR v = s;

	while (*v) v++;
	return (ULONG)(v - s);  // will fail for strings longer than 4 GB ;-)
}

