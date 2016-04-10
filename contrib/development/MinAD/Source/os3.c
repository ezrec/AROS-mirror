/*--------------------------------------------------------------------------

 Missing functions for m68k-amigaos

 Copyright (C) 2012 Rupert Hausberger <naTmeg@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

--------------------------------------------------------------------------*/

#include "os3.h"

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/intuition.h>

/*------------------------------------------------------------------------*/

ULONG DoSuperNew(struct IClass * cl, Object * obj, ULONG tag1, ...)
{
	return DoSuperMethod(cl, obj, OM_NEW, &tag1, NULL);
}

/*------------------------------------------------------------------------*/

APTR AllocVecPooled(APTR pool, ULONG size)
{
	ULONG *mem = NULL;

	if (pool) {
		size += sizeof(ULONG);
		if ((mem = (ULONG *)AllocPooled(pool, size)))
			*mem++ = size;
	}
	return mem;
}

void FreeVecPooled(APTR pool, APTR mem)
{
	if (mem) {
		ULONG *addr = (ULONG *)mem;
		ULONG size  = *--addr;

		FreePooled(pool, (APTR)addr, size);
	}
}

/*------------------------------------------------------------------------*/

