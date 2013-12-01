//
// pmmem.h
//
// Operating System Independent Memory Management
//
// $Date$
// $Revision$
//

#ifndef PM_MEM_H
#define PM_MEM_H

#include "pmtypes.h"

#include <proto/exec.h>
#define PM_Mem_Alloc(size)	PM_AllocVecPooled(size)
#define PM_Mem_Free(mem)	PM_FreeVecPooled(mem)
#define PM_Mem_Copy(s, d, l)	CopyMem(s, d, l)

STRPTR PM_String_Copy(STRPTR Source, STRPTR Dest, LONG Len);
ULONG PM_String_Length(STRPTR s);
ULONG PM_String_Compare(STRPTR str1, STRPTR str2);
void PM_StrCat(STRPTR Dst, STRPTR Src);

APTR PM_AllocVecPooled(size_t size);
void PM_FreeVecPooled(APTR mem);

#endif /* PM_MEM_H */
