/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <exec/types.h>
#include <exec/libraries.h>
#include <aros/libcall.h>

#include <proto/exec.h>

#include "guigfx_global.h"
#include LC_LIBDEFS_FILE

/****************************************************************************************/

#undef SysBase

/* Customize libheader.c */
#define LC_SYSBASE_FIELD(lib)   (((struct GuiGFXBase *)(lib))->ExecBase)
#define LC_SEGLIST_FIELD(lib)   (((struct GuiGFXBase *)(lib))->LibSegment)
#define LC_LIBBASESIZE		sizeof(struct GuiGFXBase)
#define LC_LIBHEADERTYPEPTR	struct GuiGFXBase *
#define LC_LIB_FIELD(lib)	(((struct GuiGFXBase *)(lib))->LibNode)

/* #define LC_NO_INITLIB    */
/* #define LC_NO_OPENLIB    */
/* #define LC_NO_CLOSELIB   */
/* #define LC_NO_EXPUNGELIB */

#include <libcore/libheader.c>

#undef DEBUG
#define DEBUG 1
#include <aros/debug.h>

struct ExecBase      *SysBase;

#define GGFXB(x)     ((struct GuiGFXBase *)(x))

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_InitLib) (LC_LIBHEADERTYPEPTR GuiGFXBase)
{
    SysBase = LC_SYSBASE_FIELD(GuiGFXBase);

    D(bug("Inside Init func of guigfx.library\n"));

    InitSemaphore(&GuiGFXBase->LockSemaphore);

    return TRUE;
}

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_OpenLib) (LC_LIBHEADERTYPEPTR GuiGFXBase)
{
    ULONG retval;
    
    D(bug("Inside Open func of guigfx.library\n"));

    ObtainSemaphore(&GuiGFXBase->LockSemaphore);    
    retval = GuiGFXBase->LibNode.lib_OpenCnt ? TRUE : GGFX_Init();
    ReleaseSemaphore(&GuiGFXBase->LockSemaphore);
    
    return retval;
}

/****************************************************************************************/

void  SAVEDS STDARGS LC_BUILDNAME(L_CloseLib) (LC_LIBHEADERTYPEPTR GuiGFXBase)
{
    ObtainSemaphore(&GuiGFXBase->LockSemaphore);
    if (!GuiGFXBase->LibNode.lib_OpenCnt) GGFX_Exit();
    ReleaseSemaphore(&GuiGFXBase->LockSemaphore);
    
    D(bug("Inside Close func of guigfx.library\n"));
}

/****************************************************************************************/

void  SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib) (LC_LIBHEADERTYPEPTR GuiGFXBase)
{
    D(bug("Inside Expunge func of guigfx.library\n"));
}

/****************************************************************************************/
