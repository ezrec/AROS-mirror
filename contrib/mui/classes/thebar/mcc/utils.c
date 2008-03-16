/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2007 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

***************************************************************************/

#include "class.h"

/***********************************************************************/

// DoSuperNew()
// Calls parent NEW method within a subclass
#ifdef __MORPHOS__

#elif defined(__AROS__)
Object * DoSuperNew(struct IClass *cl, Object *obj, IPTR tag1, ...)
{
  AROS_SLOWSTACKTAGS_PRE(tag1)
  retval = DoSuperMethod(cl, obj, OM_NEW, AROS_SLOWSTACKTAGS_ARG(tag1));
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

/***********************************************************************/

APTR
allocVecPooled(APTR pool,ULONG size)
{
  ULONG *mem;

  ENTER();

  size += sizeof(ULONG);

  if((mem = AllocPooled(pool, size)) != NULL)
    *mem++ = size;

  RETURN(mem);
  return mem;
}

/****************************************************************************/

void
freeVecPooled(APTR pool, APTR mem)
{
  ENTER();

  if(mem != NULL)
  {
    ULONG *_mem = (ULONG *)mem;

    FreePooled(pool, &_mem[-1], _mem[-1]);
  }

  LEAVE();
}

/****************************************************************************/

APTR
reallocVecPooledNC(APTR pool, APTR mem, ULONG size)
{
  APTR newmem = NULL;

  ENTER();

  if(pool != NULL && size != 0)
  {
    if(mem != NULL)
    {
      ULONG *_mem = (ULONG *)mem;

      if(_mem[-1] - sizeof(ULONG) >= size)
      {
        // the previous allocation has at least the size of the
        // new required space, so we just return the old memory block
        RETURN(mem);
        return(mem);
      }
      else
      {
        // free the old block...
        freeVecPooled(pool, mem);
      }
    }

    // ...and allocate a new one
    newmem = allocVecPooled(pool, size);
  }

  RETURN(newmem);
  return newmem;
}

/****************************************************************************/
