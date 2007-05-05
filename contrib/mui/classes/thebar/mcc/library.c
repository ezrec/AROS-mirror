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

 $Id$

***************************************************************************/

#include <proto/exec.h>
#include <proto/muimaster.h>

/******************************************************************************/
/*                                                                            */
/* MCC/MCP name and version                                                   */
/*                                                                            */
/* ATTENTION:  The FIRST LETTER of NAME MUST be UPPERCASE                     */
/*                                                                            */
/******************************************************************************/

#include "class.h"
#include "private.h"
#include "rev.h"

#include "mcc_common.h"

#include "Debug.h"

#define VERSION       LIB_VERSION
#define REVISION      LIB_REVISION

#if !defined(VIRTUAL)
#define SUPERCLASS    MUIC_Group
#define CLASS         MUIC_TheBar
#else
#define SUPERCLASS    MUIC_Virtgroup
#define CLASS         MUIC_TheBarVirt
#endif

#define INSTDATA      InstData

#define UserLibID     "$VER: " CLASS " " LIB_REV_STRING CPU " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION 19

#define USEDCLASSESP  used_classesP
static const char * const used_classesP[] = { "TheBar.mcp", NULL };

#define ClassInit
#define ClassExit

struct Library *DataTypesBase = NULL;
struct Library *CyberGfxBase = NULL;

#if defined(__amigaos4__)
struct DataTypesIFace *IDataTypes = NULL;
struct CyberGfxIFace *ICyberGfx = NULL;
#endif

// some variables we carry for the whole lifetime of the lib
struct MUI_CustomClass *lib_spacerClass = NULL;
struct MUI_CustomClass *lib_dragBarClass = NULL;
ULONG lib_flags = 0;

BOOL ClassInitFunc(UNUSED struct Library *base)
{
  ENTER();

  if((DataTypesBase = OpenLibrary("datatypes.library", 37)) &&
     GETINTERFACE(IDataTypes, DataTypesBase))
  {
    // make sure to initialize our subclasses
    if(initSpacerClass() && initDragBarClass())
    {
      // we open the cybgraphics.library but without failing if
      // it doesn't exist
      if((CyberGfxBase = OpenLibrary("cybergraphics.library", 41)) &&
         GETINTERFACE(ICyberGfx, CyberGfxBase))
      { }

      // check the version of MUI)
      if(MUIMasterBase->lib_Version >= MUIVER20)
      {
        lib_flags |= BASEFLG_MUI20;

        if(MUIMasterBase->lib_Version > MUIVER20 || MUIMasterBase->lib_Revision >= 5341)
          lib_flags |= BASEFLG_MUI4;
      }

      lib_flags |= BASEFLG_Init;

      RETURN(TRUE);
      return(TRUE);
    }
  }

  RETURN(FALSE);
  return(FALSE);
}


VOID ClassExitFunc(UNUSED struct Library *base)
{
  ENTER();

  // clear the data of our subclasses
  if(lib_spacerClass)
  {
    MUI_DeleteCustomClass(lib_spacerClass);
    lib_spacerClass = NULL;
  }

  if(lib_dragBarClass)
  {
    MUI_DeleteCustomClass(lib_dragBarClass);
    lib_dragBarClass = NULL;
  }

  if(CyberGfxBase)
  {
    DROPINTERFACE(ICyberGfx);
    CloseLibrary(CyberGfxBase);
    CyberGfxBase = NULL;
  }

  if(DataTypesBase)
  {
    DROPINTERFACE(IDataTypes);
    CloseLibrary(DataTypesBase);
    DataTypesBase = NULL;
  }

  lib_flags &= ~(BASEFLG_Init|BASEFLG_MUI20|BASEFLG_MUI4);

  LEAVE();
}

/******************************************************************************/
/*                                                                            */
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/*                                                                            */
/******************************************************************************/

#define USE_UTILITYBASE
#include "mccheader.c"
