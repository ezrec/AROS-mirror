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

#include "Library.h"
#include "LibWrapper_os4.h"
#include <exec/resident.h>
#include <proto/exec.h>


   /*
    * 1: Function table for library manager
    */

static uint MainFuncTable[] =
{
      (uint) &LIB_Acquire,
      (uint) &LIB_Release,
      (uint) 0,
      (uint) 0,
      (uint) &LIB_Open,    
      (uint) &LIB_Close,
      (uint) &LIB_Expunge,
      (uint) 0,
      (uint) 0xffffffff,
};

static struct TagItem MainManagerTags[] =
{
   { MIT_Name,          (uint)"__library"    },
   { MIT_VectorTable,   (uint)MainFuncTable  },
   { MIT_Version,       1                    },
   { TAG_DONE,          0                    }
};

static struct TagItem LibManagerTags[] =
{
   { MIT_Name,          (uint)"main"         },
   { MIT_VectorTable,   (uint)LIB_FuncTable  },
   { MIT_Version,       1                    },
   { TAG_DONE,          0                    }
};



   /*
    * 3: All interfaces together
    */

static const uint LibInterfaces[] =
{
   (uint)   MainManagerTags,
   (uint)   LibManagerTags,
   (uint)   0
};



   /*
    * 4: Library creation tags
    */
    
struct TagItem LIB_CreateTags[] =
{
   { CLT_DataSize,      (uint)sizeof(struct Library)  },
   { CLT_InitFunc,      (uint)&LIB_Init               },
   { CLT_Interfaces,    (uint)&LibInterfaces          },
   { TAG_DONE,          (uint)0                       }
};

   struct Library      *OurBase  = 0;
   void                *SegList  = 0;

struct Library *LIB_Init(Library *pOurBase, void *pSegList, ExecIFace *pSysBase) 
{
   SysBase = pSysBase->Data.LibBase;
   IExec = pSysBase;
   OurBase = pOurBase;
   SegList = pSegList;

   if (true == Lib_SetUp())
      return OurBase;
   
   DeleteLibrary(pOurBase);
   return 0;
}

uint LIB_Expunge(struct LibraryManagerInterface *pSelf)
{
   if (OurBase->lib_OpenCnt)
   {
      OurBase->lib_Flags |= LIBF_DELEXP;
      return 0;
   }
  
   Lib_CleanUp();

   Forbid();
   Remove(&OurBase->lib_Node);
   Permit();

   DeleteLibrary(pSelf->Data.LibBase);

   return (ULONG)SegList;
}

struct Library *LIB_Open(struct LibraryManagerInterface *pSelf)
{
   if (false == Lib_Acquire())
      return 0;
   OurBase->lib_Flags &= ~LIBF_DELEXP;
   OurBase->lib_OpenCnt++;
   return OurBase;
}

uint LIB_Close(struct LibraryManagerInterface *pSelf)
{
   if (OurBase->lib_OpenCnt == 0)
      return 0;

   Lib_Release();

   if ((--OurBase->lib_OpenCnt) == 0)
   {
      if (OurBase->lib_Flags & LIBF_DELEXP)
         return (LIB_Expunge(pSelf));
   }
   return 0;
}

uint LIB_Acquire(struct LibraryManagerInterface *pSelf)
{
   return (pSelf->Data.RefCount++);
}

uint LIB_Release(struct LibraryManagerInterface *pSelf)
{
   return (pSelf->Data.RefCount--);
}

