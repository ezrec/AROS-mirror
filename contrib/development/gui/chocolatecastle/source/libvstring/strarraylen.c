/* libvstring/strarraylen.c */

#include "libvstring.h"


ULONG StrArrayLen(STRPTR *array)
{
	STRPTR *p;
	ULONG len;

	for (p = array, len = 0; *p; p++) len++;

	return len;
}

