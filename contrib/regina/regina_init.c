/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Regina code for initialization during opening and closing of the library
*/

#include <stddef.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <aros/libcall.h>
#include <aros/debug.h>
#include <libcore/base.h>
#include <libcore/compiler.h>

#include <proto/exec.h>
#include <proto/alib.h>

#include "rexx.h"

struct ExecBase* SysBase;
struct DosLibrary* DOSBase;
struct Library* aroscbase;
struct MinList *__regina_tsdlist = NULL;

/****************************************************************************************/

ULONG SAVEDS STDARGS Regina_L_InitLib (struct LibHeader *ReginaBase)
{
    D(bug("Inside Init func of regina.library\n"));

    SysBase = ReginaBase->lh_SysBase;
    if (!(DOSBase = OpenLibrary("dos.library",0))) return FALSE;

    if (!(aroscbase = OpenLibrary("arosc.library",41)))
    {
        CloseLibrary((struct Library *)DOSBase);
        return FALSE;
    }

    __regina_semaphorepool = CreatePool(MEMF_PUBLIC, 1024, 256);
  
    __regina_tsdlist = (struct MinList *)AllocPooled (__regina_semaphorepool, sizeof(struct MinList));
    NewList((struct List *)__regina_tsdlist);

    return TRUE;
}

void  SAVEDS STDARGS Regina_L_ExpungeLib (struct LibHeader *ReginaBase)
{
    D(bug("Inside Expunge func of regina.library\n"));

    DeletePool(__regina_semaphorepool);
    CloseLibrary(aroscbase);
    CloseLibrary((struct Library *)DOSBase);
}

