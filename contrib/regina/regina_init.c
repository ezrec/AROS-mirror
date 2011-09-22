/*
  Copyright © 1995-2011, The AROS Development Team. All rights reserved.
  $Id$

  Desc: Regina code for initialization during opening and closing of the library
*/

#define DEBUG 0
#include <exec/types.h>
#include <exec/memory.h>
#include <aros/libcall.h>
#include <aros/debug.h>
#include <proto/exec.h>
#include <proto/alib.h>
#include <proto/dos.h>

#include <stddef.h>
#include "rexx.h"

#include <aros/symbolsets.h>
#include LC_LIBDEFS_FILE

struct MinList *__regina_tsdlist = NULL;

IPTR aroscbase_offset;

static int InitLib(LIBBASETYPEPTR LIBBASE)
{
   D(bug("Inside Init func of regina.library\n"));

   aroscbase_offset = offsetof(LIBBASETYPE, _aroscbase);
 
   __regina_semaphorepool = CreatePool(MEMF_PUBLIC, 1024, 256);
  
   __regina_tsdlist = (struct MinList *)AllocPooled (__regina_semaphorepool, sizeof(struct MinList));
   NewList((struct List *)__regina_tsdlist);
    
   return TRUE;
}

static int OpenLib(LIBBASETYPEPTR LIBBASE)
{
   D(bug("Opening regina.library\n"));

   LIBBASE->_aroscbase = OpenLibrary("arosc.library", 0);

   D(bug("[regina.library::OpenLib] aroscbase=%p\n", LIBBASE->_aroscbase));

   return LIBBASE->_aroscbase != NULL;
}

static int CloseLib(LIBBASETYPEPTR LIBBASE)
{
   D(bug("Closing regina.library\n"));

   CloseLibrary(LIBBASE->_aroscbase);

   return TRUE;
}

static int ExpungeLib(LIBBASETYPEPTR LIBBASE)
{
   D(bug("Inside Expunge func of regina.library\n"));

   DeletePool(__regina_semaphorepool);
   
   return TRUE;
}

ADD2INITLIB(InitLib, 0);
ADD2OPENLIB(OpenLib, 0);
ADD2CLOSELIB(CloseLib, 0);
ADD2EXPUNGELIB(ExpungeLib, 0);
