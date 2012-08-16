/* vstring.h */

#ifndef __NOLIBBASE__
#define __NOLIBBASE__
#endif

#include <exec/types.h>
#include <stdarg.h>

extern struct Library *SysBase;


ULONG FmtLen(STRPTR fmt, ...);
void FmtPut(STRPTR dest, STRPTR fmt, ...);
LONG FmtNPut(STRPTR dest, STRPTR fmt, LONG maxlen, ...);
STRPTR VFmtNew(STRPTR fmt, va_list args);
STRPTR FmtNew(STRPTR fmt, ...);
ULONG StrLen(STRPTR str);
STRPTR StrCopy(STRPTR src, STRPTR dest);
STRPTR StrNCopy(STRPTR src, STRPTR dest, LONG maxlen);
STRPTR StrNew(STRPTR str);
STRPTR* CloneStrArray(STRPTR *array);
VOID FreeStrArray(STRPTR *array);
ULONG StrArrayLen(STRPTR *array);

#define StrFree(s) FreeVecTaskPooled(s)


#ifdef __AROS__
extern APTR MPool;
#include <proto/exec.h>
#define  AllocVecTaskPooled(size) AllocVecPooled(MPool, size)
#define FreeVecTaskPooled(ptr) FreeVecPooled(MPool, ptr)
#endif
