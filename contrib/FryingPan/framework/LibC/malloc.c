/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "LibC.h"
#include <Startup/Startup.h>
#include <proto/exec.h>
#include <exec/memory.h>
#include <Generic/Types.h>

// REMEMBER:
// the resulting stuff is used by
// free and realloc!
// you may want to change these if you modify anything here.

void* malloc(size_t lSize)
{
   IPTR *pMem = 0;

   if (0 == lSize)
      return 0;

   lSize = ((lSize + 7) & ~7) + 8;

   if (0 != __InternalMemPool)
   {
#ifndef DEBUG
#ifndef __amigaos4__
      ObtainSemaphore(&__InternalSemaphore);                   // we wait only for our tasks!
      pMem = (IPTR*)AllocPooled(__InternalMemPool, lSize);   // Get memory
      ReleaseSemaphore(&__InternalSemaphore);                  // and free sem.
#else
      IExec->ObtainSemaphore(&__InternalSemaphore);                   // we wait only for our tasks!
      pMem = (IPTR*)IExec->AllocPooled(__InternalMemPool, lSize);   // Get memory
      IExec->ReleaseSemaphore(&__InternalSemaphore);                  // and free sem.
#endif

      if (0 != pMem)
      {
         pMem[0] = 0;                                          // set mem pool
         pMem[1] = lSize;                                      // set mem size
         pMem    = &pMem[2];                                   // set mem handle
      }      
#else
      uint32 i;
      IPTR *pMem2 = 0;
      lSize += 128;

#ifndef __amigaos4__
      ObtainSemaphore(&__InternalSemaphore);                   // we wait only for our tasks!
      pMem = (IPTR*)AllocPooled(__InternalMemPool, lSize);   // Get memory
      ReleaseSemaphore(&__InternalSemaphore);                  // and free sem.
#else
      IExec->ObtainSemaphore(&__InternalSemaphore);                   // we wait only for our tasks!
      pMem = (IPTR*)IExec->AllocPooled(__InternalMemPool, lSize);   // Get memory
      IExec->ReleaseSemaphore(&__InternalSemaphore);                  // and free sem.
#endif
      if (0 != pMem)
      {
         pMem2   = &pMem[lSize>>2];
         pMem[0] = 0;                                          // set mem pool
         pMem[1] = lSize;                                      // set mem size
         pMem    = &pMem[2+(128>>3)];                          // set mem handle
         for (i=1; i<=(128>>3); i++)
         {
            pMem[-i]    = 0xC0DEDBAD;
            pMem2[-i]   = 0xC0DEDBAD;
         }
      }      

      lSize -= 128;
#endif
   }   

   if (pMem == 0)
   {
      _error("Unable to allocate %ld bytes of memory!!!\nClick 'OK' to retry", ARRAY(lSize));
      return malloc(lSize-8);
   }
   return pMem;                                                // return mem :)
}


void* malloc_pooled(void* pPool, size_t lSize)
{
   IPTR *pMem = 0;

   if (0 == lSize)
      return 0;

   lSize = ((lSize + 7) & ~7) + 8;

   if (0 != pPool)
   {      
#ifndef DEBUG
#ifndef __amigaos4__
      pMem = (IPTR*)AllocPooled(pPool, lSize+8);             // Get memory. I assume we dont need syncing here.
#else
      pMem = (IPTR*)IExec->AllocPooled(pPool, lSize+8);             // Get memory. I assume we dont need syncing here.
#endif

      if (0 != pMem)
      {
         pMem[0] = (IPTR)pPool;                              // set mem pool
         pMem[1] = lSize;                                      // set mem size
         pMem    = &pMem[2];                                   // set mem handle
      }      
#else
      uint32 i;
      IPTR *pMem2 = 0;
      lSize += 128;

#ifndef __amigaos4__
      pMem = (IPTR*)AllocPooled(pPool, lSize);   // Get memory
#else
      pMem = (IPTR*)IExec->AllocPooled(pPool, lSize);   // Get memory
#endif

      if (0 != pMem)
      {
         pMem2   = &pMem[lSize>>2];
         pMem[0] = (IPTR)pPool;                              // set mem pool
         pMem[1] = lSize;                                      // set mem size
         pMem    = &pMem[2+(128>>3)];                          // set mem handle
      }      

      for (i=1; i<=(128>>3); i++)
      {
         pMem[-i]    = 0xC0DEDBAD;
         pMem2[-i]   = 0xC0DEDBAD;
      }
#endif
   }   
   return pMem;                                                // return mem :)
}

