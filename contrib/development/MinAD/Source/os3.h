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

#ifndef _OS3_FUNCS_H
#define _OS3_FUNCS_H

#ifndef EXEC_TYPES_H
	#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#ifndef INTUITION_CLASSES_H
	#include <intuition/classes.h>
#endif /* INTUITION_CLASSES_H */

/*------------------------------------------------------------------------*/

typedef unsigned long long UQUAD;
typedef signed long long QUAD;

/*------------------------------------------------------------------------*/

ULONG DoSuperNew(struct IClass * cl, Object * obj, ULONG tag1, ...);

/*------------------------------------------------------------------------*/

APTR AllocVecPooled(APTR pool, ULONG size);
void FreeVecPooled(APTR pool, APTR memory);

/*------------------------------------------------------------------------*/

#endif /* _OS3_FUNCS_H */

