/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2013 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id$

***************************************************************************/

#include "class.h"

/***********************************************************************/

// DoSuperNew()
// Calls parent NEW method within a subclass
#if !defined(__MORPHOS__)
#ifdef __AROS__
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, Tag tag1, ...)
{
    AROS_SLOWSTACKTAGS_PRE_AS(tag1, Object *)
    retval = (Object *)DoSuperMethod(cl, obj, OM_NEW, AROS_SLOWSTACKTAGS_ARG(tag1), NULL);
    AROS_SLOWSTACKTAGS_POST
}
#else
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...)
{
  Object *rc;
  VA_LIST args;

  VA_START(args, obj);
  rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, VA_ARG(args, ULONG), NULL);
  VA_END(args);

  return rc;
}
#endif
#endif

/***********************************************************************/
static APTR sharedPool;
#if !defined(__amigaos4__) && !defined(__MORPHOS__)
static struct SignalSemaphore sharedPoolSema;
#endif

BOOL CreateSharedPool(void)
{
  BOOL success = FALSE;

  ENTER();

  #if defined(__amigaos4__)
  sharedPool = AllocSysObjectTags(ASOT_MEMPOOL, ASOPOOL_MFlags, MEMF_SHARED,
                                                ASOPOOL_Puddle, 2048,
                                                ASOPOOL_Threshold, 1024,
                                                #if defined(VIRTUAL)
                                                ASOPOOL_Name, "TheBarVirt.mcc shared pool",
                                                #else
                                                ASOPOOL_Name, "TheBar.mcc shared pool",
                                                #endif
                                                ASOPOOL_Protected, TRUE,
                                                ASOPOOL_LockMem, FALSE,
                                                TAG_DONE);
  #elif defined(__MORPHOS__)
  sharedPool = CreatePool(MEMF_SEM_PROTECTED, 2048, 1024);
  #else
  sharedPool = CreatePool(MEMF_ANY, 2048, 1024);
  memset(&sharedPoolSema, 0, sizeof(sharedPoolSema));
  InitSemaphore(&sharedPoolSema);
  #endif

  if(sharedPool != NULL)
    success = TRUE;

  RETURN(success);
  return(success);
}

void DeleteSharedPool(void)
{
  ENTER();

  if(sharedPool != NULL)
  {
    #if defined(__amigaos4__)
    FreeSysObject(ASOT_MEMPOOL, sharedPool);
    #else
    DeletePool(sharedPool);
    #endif

    sharedPool = NULL;
  }
}

APTR SharedAlloc(ULONG size)
{
  ULONG *mem;

  ENTER();

  #if defined(__amigaos4__) || defined(__MORPHOS__)
  mem = AllocVecPooled(sharedPool, size);
  #else // __amigaos4__ || __MORPHOS__
  size += sizeof(ULONG);

  ObtainSemaphore(&sharedPoolSema);

  if((mem = AllocPooled(sharedPool, size)) != NULL)
    *mem++ = size;

  ReleaseSemaphore(&sharedPoolSema);
  #endif // __amigaos4__ || __MORPHOS__

  RETURN(mem);
  return mem;
}

/****************************************************************************/

void SharedFree(APTR mem)
{
  ENTER();

  if(mem != NULL)
  {
    #if defined(__amigaos4__) || defined(__MORPHOS__)
    FreeVecPooled(sharedPool, mem);
    #else // __amigaos4__ || __MORPHOS__
    ULONG *_mem = (ULONG *)mem;

    ObtainSemaphore(&sharedPoolSema);

    FreePooled(sharedPool, &_mem[-1], _mem[-1]);

    ReleaseSemaphore(&sharedPoolSema);
    #endif // __amigaos4__ || __MORPHOS__
  }

  LEAVE();
}

/****************************************************************************/
