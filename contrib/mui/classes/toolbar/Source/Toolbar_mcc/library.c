/***************************************************************************

 Toolbar MCC - MUI Custom Class for Toolbar handling

 Copyright (C) 1997-2004 by Benny Kjær Nielsen <floyd@amiga.dk>
                            Darius Brewka <d.brewka@freenet.de>
                            Jens Langner <Jens.Langner@light-speed.de>

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 $Id$

***************************************************************************/

/******************************************************************************/
/*                                                                            */
/* includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include <exec/types.h>
#include <exec/resident.h>
#include <exec/execbase.h>
#include <dos/dosextens.h>
#include <libraries/mui.h>
#include <exec/libraries.h>
#include <proto/exec.h>
#include <proto/muimaster.h>

/******************************************************************************/
/*                                                                            */
/* MCC/MCP name and version                                                   */
/*                                                                            */
/* ATTENTION:  The FIRST LETTER of NAME MUST be UPPERCASE                     */
/*                                                                            */
/******************************************************************************/

#include "Toolbar_mcc.h"

#include "InstanceData.h"
#include "rev.h"

#define VERSION       LIB_VERSION
#define REVISION      LIB_REVISION

#define CLASS         MUIC_Toolbar
#define SUPERCLASS    MUIC_Area

#define INSTDATA      Toolbar_Data

#define UserLibID     "$VER: Toolbar.mcc " LIB_REV_STRING CPU " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION 15

#define USEDCLASSESP  used_classesP
static const STRPTR used_classesP[] = { "Toolbar.mcp", NULL };

#define PreClassInit
#define PostClassExit

struct Library *DataTypesBase = NULL;
struct Library *KeymapBase    = NULL;
struct Library *DiskfontBase  = NULL;
#if defined(__amigaos4__)
struct DataTypesIFace *IDataTypes = NULL;
struct KeymapIFace *IKeymap       = NULL;
struct DiskfontIFace *IDiskfont   = NULL;
#endif

#ifdef USE_IMAGEPOOL
struct Library *ImagePoolBase = NULL;
#if defined(__amigaos4__)
struct ImagePoolIFace *IImagePool = NULL;
#endif
#endif

BOOL PreClassInitFunc(void)
{
  if((DataTypesBase = OpenLibrary("datatypes.library", 37L)) &&
     GETINTERFACE(IDataTypes, DataTypesBase))
  {
    if((KeymapBase = OpenLibrary("keymap.library", 37L)) &&
       GETINTERFACE(IKeymap, KeymapBase))
    {
      if((DiskfontBase = OpenLibrary("diskfont.library", 37L)) &&
         GETINTERFACE(IDiskfont, DiskfontBase))
      {
        #ifdef USE_IMAGEPOOL
        // ImagePool is no MUST
        if((ImagePoolBase = OpenLibrary("imagepool.library", 37L)) &&
           GETINTERFACE(IImagePool, ImagePoolBase)) { }
        #endif

        return TRUE;
      }

      DROPINTERFACE(IKeymap);
      CloseLibrary(KeymapBase);
      KeymapBase = NULL;
    }

    DROPINTERFACE(IDataTypes);
    CloseLibrary(DataTypesBase);
    DataTypesBase = NULL;
  }

  return FALSE;
}

VOID PostClassExitFunc(void)
{
  #ifdef USE_IMAGEPOOL
  if(ImagePoolBase) { DROPINTERFACE(IImagePool); CloseLibrary(ImagePoolBase); ImagePoolBase = NULL; }
  #endif

  if(DiskfontBase)  { DROPINTERFACE(IDiskfont);  CloseLibrary(DiskfontBase);  DiskfontBase  = NULL; }
  if(KeymapBase)    { DROPINTERFACE(IKeymap);    CloseLibrary(KeymapBase);    KeymapBase    = NULL; }
  if(DataTypesBase) { DROPINTERFACE(IDataTypes); CloseLibrary(DataTypesBase); DataTypesBase = NULL; }
}

/******************************************************************************/
/*                                                                            */
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/*                                                                            */
/******************************************************************************/

#include "mccheader.c"
