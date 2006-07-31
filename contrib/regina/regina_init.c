/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Regina code for initialization during opening and closing of the library
*/

#include <exec/types.h>
#include <exec/memory.h>
#include <aros/libcall.h>
#include <aros/debug.h>
#include <proto/exec.h>
#include <proto/alib.h>

#include <stddef.h>
#include "rexx.h"

#include <aros/symbolsets.h>
#include LC_LIBDEFS_FILE

struct ExecBase* SysBase;
struct DosLibrary* DOSBase;
struct Library* aroscbase;
struct MinList *__regina_tsdlist = NULL;


AROS_SET_LIBFUNC(InitLib, LIBBASETYPE, LIBBASE)
{
    AROS_SET_LIBFUNC_INIT;
    
    D(bug("Inside Init func of regina.library\n"));

    if (!(DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",0))) return FALSE;

    if (!(aroscbase = OpenLibrary("arosc.library",41)))
    {
        CloseLibrary((struct Library *)DOSBase);
        return FALSE;
    }

    __regina_semaphorepool = CreatePool(MEMF_PUBLIC, 1024, 256);
  
    __regina_tsdlist = (struct MinList *)AllocPooled (__regina_semaphorepool, sizeof(struct MinList));
    NewList((struct List *)__regina_tsdlist);
    
    return TRUE;

    AROS_SET_LIBFUNC_EXIT;
}

AROS_SET_LIBFUNC(ExpungeLib, LIBBASETYPE, LIBBASE)
{
    AROS_SET_LIBFUNC_INIT;
    
    D(bug("Inside Expunge func of regina.library\n"));

    DeletePool(__regina_semaphorepool);
    CloseLibrary(aroscbase);
    CloseLibrary((struct Library *)DOSBase);
    
    return TRUE;
    
    AROS_SET_LIBFUNC_EXIT;
}

ADD2INITLIB(InitLib, 0);
ADD2EXPUNGELIB(ExpungeLib, 0);
