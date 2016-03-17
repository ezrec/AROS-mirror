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

#include <Startup/Startup.h>
#include "LibC.h"
#include <proto/exec.h>
#include <Generic/Types.h>

void free(void* pMem)
{
   void* pPool;
   long  lSize;

   if (NULL == pMem)
      return;

#ifndef DEBUG
   pMem  = &((void**)pMem)[-2];
   pPool = ((void**)pMem)[0];
   lSize = ((IPTR *)pMem)[1];
#else
   pMem  = &((void**)pMem)[-(2+(128>>3))];
   pPool = ((void**)pMem)[0];
   lSize = ((IPTR *)pMem)[1];

   {
      IPTR *pMem1 = &((IPTR *)pMem)[2+(128>>3)];
      IPTR *pMem2 = &((IPTR *)pMem)[lSize>>2];
      uint32 i;

      for (i=1; i<=(128>>3); i++)
      {
         if ((pMem1[-i] != 0xC0DEDBAD) ||
             (pMem2[-i] != 0xC0DEDBAD))
         {
            _error("Freeing corrupted memory block!\nSize: %ld bytes\nMEMORY WILL NOT BE FREED!\nPLEASE REPORT!", lSize-128-8);
            return;
         }
      }
   }
#endif


   if (0 != pPool)
   {
#ifndef __amigaos4__
      FreePooled(pPool, pMem, lSize);
#else
      IExec->FreePooled(pPool, pMem, lSize);
#endif
   }
   else
   {
#ifndef __amigaos4__
      ObtainSemaphore(&__InternalSemaphore);
      FreePooled(__InternalMemPool, pMem, lSize);
      ReleaseSemaphore(&__InternalSemaphore);
#else
      IExec->ObtainSemaphore(&__InternalSemaphore);
      IExec->FreePooled(__InternalMemPool, pMem, lSize);
      IExec->ReleaseSemaphore(&__InternalSemaphore);
#endif
   }
}
