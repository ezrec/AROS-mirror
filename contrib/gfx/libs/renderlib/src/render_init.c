/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <exec/types.h>
#include <exec/libraries.h>
#include <aros/libcall.h>

#include <proto/exec.h>

#include "lib_init.h"
#include LC_LIBDEFS_FILE

/****************************************************************************************/

#undef SysBase

/* Customize libheader.c */
#define LC_SYSBASE_FIELD(lib)   (((LIB_BASE_T *)(lib))->ExecBase)
#define LC_SEGLIST_FIELD(lib)   (((LIB_BASE_T *)(lib))->LibSegment)
#define LC_LIBBASESIZE		sizeof(LIB_BASE_T)
#define LC_LIBHEADERTYPEPTR	LIB_BASE_T *
#define LC_LIB_FIELD(lib)	(((LIB_BASE_T *)(lib))->LibNode)

/* #define LC_NO_INITLIB    */
/* #define LC_NO_OPENLIB    */
/* #define LC_NO_CLOSELIB   */
/* #define LC_NO_EXPUNGELIB */

#include <libcore/libheader.c>

#undef DEBUG
#define DEBUG 1
#include <aros/debug.h>

struct ExecBase      *SysBase;

#define RNDB(x)     ((LIB_BASE_T *)(x))

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_InitLib) (LC_LIBHEADERTYPEPTR RenderBase)
{
    SysBase = LC_SYSBASE_FIELD(RenderBase);

    D(bug("Inside Init func of render.library\n"));
    
    InitSemaphore(&RenderBase->LockSemaphore);

    return TRUE;
}

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_OpenLib) (LC_LIBHEADERTYPEPTR RenderBase)
{
    ULONG retval;
    
    D(bug("Inside Open func of render.library\n"));

    ObtainSemaphore(&RenderBase->LockSemaphore);    
    retval = RenderBase->LibNode.lib_OpenCnt ? TRUE : Library_Init(RNDB(RenderBase));
    ReleaseSemaphore(&RenderBase->LockSemaphore);
    
    return retval;
}

/****************************************************************************************/

void  SAVEDS STDARGS LC_BUILDNAME(L_CloseLib) (LC_LIBHEADERTYPEPTR RenderBase)
{
    ObtainSemaphore(&RenderBase->LockSemaphore);
    if (!RenderBase->LibNode.lib_OpenCnt) Library_Exit(RenderBase);
    ReleaseSemaphore(&RenderBase->LockSemaphore);
    
    D(bug("Inside Close func of render.library\n"));
}

/****************************************************************************************/

void  SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib) (LC_LIBHEADERTYPEPTR RenderBase)
{
    D(bug("Inside Expunge func of render.library\n"));
}

/****************************************************************************************/
