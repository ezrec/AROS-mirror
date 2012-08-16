/* libvstring/strnew.c */

#include "libvstring.h"

#include <proto/exec.h>


STRPTR StrNew(STRPTR str)
{
	STRPTR n = NULL;

	if (n = AllocVecTaskPooled(StrLen(str) + 1))
	{
		StrCopy(str, n);
	}

	return n;
}

