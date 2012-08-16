/* libvstring/freestrarray.c */

#include "libvstring.h"

#include <proto/exec.h>


void FreeStrArray(STRPTR *array)
{
	STRPTR *p0;

	for (p0 = array; *p0; p0++) FreeVecTaskPooled(*p0);
	FreeVecTaskPooled(array);
}

