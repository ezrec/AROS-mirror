/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <exec/types.h>
#include <exec/libraries.h>
#include <aros/libcall.h>

#include <proto/exec.h>

#include "mystic_global.h"
#include LC_LIBDEFS_FILE

/****************************************************************************************/

#undef SysBase

/* Customize libheader.c */
#define LC_SYSBASE_FIELD(lib)   (((struct MysticBase *)(lib))->ExecBase)
#define LC_SEGLIST_FIELD(lib)   (((struct MysticBase *)(lib))->LibSegment)
#define LC_LIBBASESIZE		sizeof(struct MysticBase)
#define LC_LIBHEADERTYPEPTR	struct MysticBase *
#define LC_LIB_FIELD(lib)	(((struct MysticBase *)(lib))->LibNode)

/* #define LC_NO_INITLIB    */
/* #define LC_NO_OPENLIB    */
/* #define LC_NO_CLOSELIB   */
/* #define LC_NO_EXPUNGELIB */

#include <libcore/libheader.c>

#undef DEBUG
#define DEBUG 1
#include <aros/debug.h>

struct ExecBase      *SysBase;

#define GGFXB(x)     ((struct MysticBase *)(x))

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_InitLib) (LC_LIBHEADERTYPEPTR MysticBase)
{
    SysBase = LC_SYSBASE_FIELD(MysticBase);

    D(bug("Inside Init func of mysticview.library\n"));

    InitSemaphore(&MysticBase->LockSemaphore);

    return TRUE;
}

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_OpenLib) (LC_LIBHEADERTYPEPTR MysticBase)
{
    ULONG retval;
    
    D(bug("Inside Open func of mysticview.library\n"));

    ObtainSemaphore(&MysticBase->LockSemaphore);    
    retval = MysticBase->LibNode.lib_OpenCnt ? TRUE : MYSTIC_Init();
    ReleaseSemaphore(&MysticBase->LockSemaphore);
    
    return retval;
}

/****************************************************************************************/

void  SAVEDS STDARGS LC_BUILDNAME(L_CloseLib) (LC_LIBHEADERTYPEPTR MysticBase)
{
    ObtainSemaphore(&MysticBase->LockSemaphore);
    if (!MysticBase->LibNode.lib_OpenCnt) MYSTIC_Exit();
    ReleaseSemaphore(&MysticBase->LockSemaphore);
    
    D(bug("Inside Close func of mysticview.library\n"));
}

/****************************************************************************************/

void  SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib) (LC_LIBHEADERTYPEPTR MysticBase)
{
    D(bug("Inside Expunge func of mysticview.library\n"));
}

/****************************************************************************************/
