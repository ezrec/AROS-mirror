/* $Id$ */

/*
     AHI - Hardware independent audio subsystem
     Copyright (C) 1996-1999 Martin Blom <martin@blom.org>
     
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
     
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
     
     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the
     Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
     MA 02139, USA.
*/

#include <config.h>
#include <CompilerSpecific.h>

#include <stdarg.h>

#include <exec/lists.h>
#include <exec/nodes.h>
#include <powerup/ppclib/memory.h>
#include <powerup/ppclib/interface.h>
#include <powerup/ppclib/object.h>
#include <powerpc/powerpc.h>
#include <powerpc/memoryPPC.h>
#include <intuition/intuition.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/ppc.h>
#include <proto/powerpc.h>

#include "ahi_def.h"
#include "header.h"
#include "elfloader.h"


/******************************************************************************
** Findnode *******************************************************************
******************************************************************************/

// Find a node in a list

struct Node *
FindNode ( struct List *list,
           struct Node *node )
{
  struct Node *currentnode;

  for(currentnode = list->lh_Head;
      currentnode->ln_Succ;
      currentnode = currentnode->ln_Succ)
  {
    if(currentnode == node)
    {
      return currentnode;
    }
  }
  return NULL;
}


/******************************************************************************
** Fixed2Shift ****************************************************************
******************************************************************************/

// Returns host many steps to right-shift a value to approximate
// scaling with the Fixed argument.

int
Fixed2Shift( Fixed f )
{
  int   i   = 0;
  Fixed ref = 0x10000;

  while( f < ref )
  {
    i++;
    ref >>= 1;
  }

  return i;
}

/******************************************************************************
** Req ************************************************************************
******************************************************************************/

void
Req( const char* text, ... )
{
  struct EasyStruct es = 
  {
    sizeof (struct EasyStruct),
    0,
    (STRPTR) DevName,
    (STRPTR) text,
    "OK"
  };

  va_list ap;

  va_start( ap, text );

  EasyRequestArgs( NULL, &es, NULL, ap );
  
  va_end( ap );
}

/******************************************************************************
** AHIAllocVec ****************************************************************
******************************************************************************/

APTR
AHIAllocVec( ULONG byteSize, ULONG requirements )
{
  switch( MixBackend )
  {
    case MB_NATIVE:
      return AllocVec( byteSize, requirements & ~MEMF_PPCMASK );

    case MB_POWERUP:
      return PPCAllocVec( byteSize, requirements );

    case MB_WARPUP:
    {
      ULONG new_requirements;
      void* v;

      new_requirements = requirements & ~MEMF_PPCMASK;

      if( requirements & ( MEMF_WRITETHROUGHPPC | MEMF_WRITETHROUGHM68K ) )
      {
        Req( "Internal error: Illegal memory attribute in AHIAllocVec()." );
      }

      if( requirements & 
          ( MEMF_NOCACHEPPC | MEMF_NOCACHEM68K |
            MEMF_NOCACHESYNCPPC | MEMF_NOCACHESYNCM68K ) )
      {
        new_requirements |= MEMF_CHIP;            // Sucks!
      }

      v = AllocVec32( byteSize, new_requirements );
      CacheClearU();
      return v;
    }
  }

  Req( "Internal error: Unknown MixBackend in AHIAllocVec()." );
  return NULL;
}

/******************************************************************************
** AHIFreeVec *****************************************************************
******************************************************************************/

void
AHIFreeVec( APTR memoryBlock )
{
  switch( MixBackend )
  {
    case MB_NATIVE:
      FreeVec( memoryBlock );
      return;

    case MB_POWERUP:
      PPCFreeVec( memoryBlock );
      return;

    case MB_WARPUP:
      FreeVec32( memoryBlock );
      return;
  }

  Req( "Internal error: Unknown MixBackend in AHIFreeVec()." );
}


/******************************************************************************
** AHILoadObject **************************************************************
******************************************************************************/

void*
AHILoadObject( const char* objname )
{
  switch( MixBackend )
  {
    case MB_NATIVE:
      Req( "Internal error: Illegal MixBackend in AHILoadObject()" );
      return NULL;

    case MB_POWERUP:
      return PPCLoadObject( (char*) objname );

    case MB_WARPUP:
    {
      void* o;

      o = ELFLoadObject( objname );
      CacheClearU();

      return o;
    }
  }    

  Req( "Internal error: Unknown MixBackend in AHILoadObject()." );
  return NULL;
}

/******************************************************************************
** AHIUnLoadObject ************************************************************
******************************************************************************/

void
AHIUnloadObject( void* obj )
{
  switch( MixBackend )
  {
    case MB_NATIVE:
      Req( "Internal error: Illegal MixBackend in AHIUnloadObject()" );
      return;

    case MB_POWERUP:
      PPCUnLoadObject( obj );
      return;

    case MB_WARPUP:
      ELFUnLoadObject( obj );
      return;
  }

  Req( "Internal error: Unknown MixBackend in AHIUnloadObject()" );
}

/******************************************************************************
** AHIGetELFSymbol ************************************************************
******************************************************************************/

BOOL
AHIGetELFSymbol( const char* name,
                 void** ptr )
{
  switch( MixBackend )
  {
    case MB_NATIVE:
      Req( "Internal error: Illegal MixBackend in AHIUnloadObject()" );
      return FALSE;

    case MB_POWERUP:
    {
      struct PPCObjectInfo oi =
      {
        0,
        NULL,
        PPCELFINFOTYPE_SYMBOL,
        STT_SECTION,
        STB_GLOBAL,
        0
      };
      
      struct TagItem tag_done =
      {
        TAG_DONE, 0
      };

      BOOL rc;

      oi.Name = (char*) name;
      rc = PPCGetObjectAttrs( PPCObject, &oi, &tag_done );
      *ptr = (void*) oi.Address;

      return rc;
    }

    case MB_WARPUP:
      return ELFGetSymbol( PPCObject, name, ptr );
  }

  Req( "Internal error: Unknown MixBackend in AHIUnloadObject()" );
  return FALSE;
}
