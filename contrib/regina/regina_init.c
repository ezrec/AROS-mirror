/*
    Copyright © 1995-2006, The AROS Development Team. All rights reserved.
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

struct DosLibrary* DOSBase;
struct Library* aroscbase;
struct MinList *__regina_tsdlist = NULL;


static int InitLib(LIBBASETYPEPTR LIBBASE)
{
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
}

static int ExpungeLib(LIBBASETYPEPTR LIBBASE)
{
    D(bug("Inside Expunge func of regina.library\n"));

    DeletePool(__regina_semaphorepool);
    CloseLibrary(aroscbase);
    CloseLibrary((struct Library *)DOSBase);

    return TRUE;
}

ADD2INITLIB(InitLib, 0);
ADD2EXPUNGELIB(ExpungeLib, 0);
