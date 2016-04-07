/***************************************************************************

 Toolbar MCP - MUI Custom Class Preferences for Toolbar handling

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

#include "InstanceData.h"
#include "rev.h"

#define VERSION       LIB_VERSION
#define REVISION      LIB_REVISION

#define CLASS         "Toolbar.mcp"
#define SUPERCLASSP   MUIC_Mccprefs

#define INSTDATAP     Toolbar_DataP

#define UserLibID     "$VER: Toolbar.mcp " LIB_REV_STRING CPU " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION 15

#define PreClassInit
#define PostClassExit

struct Library *LocaleBase = NULL;
#if defined(__amigaos4__)
struct LocaleIFace *ILocale = NULL;
#endif

BOOL PreClassInitFunc(void)
{
  if((LocaleBase = OpenLibrary("locale.library", 37L)) &&
     GETINTERFACE(ILocale, LocaleBase))
  {
    return TRUE;
  }

  return FALSE;
}

VOID PostClassExitFunc(void)
{
  if(LocaleBase)
  {
    DROPINTERFACE(ILocale);
    CloseLibrary(LocaleBase);
    LocaleBase = NULL;
  }
}

/******************************************************************************/
/*                                                                            */
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/*                                                                            */
/******************************************************************************/

#define USE_PREFSIMAGE_COLORS
#define USE_PREFSIMAGE_BODY
#include "PrefsImage.h"

#define PREFSIMAGEOBJECT \
  BodychunkObject,\
    MUIA_FixWidth,              PREFSIMAGE_WIDTH,\
    MUIA_FixHeight,             PREFSIMAGE_HEIGHT,\
    MUIA_Bitmap_Width,          PREFSIMAGE_WIDTH ,\
    MUIA_Bitmap_Height,         PREFSIMAGE_HEIGHT,\
    MUIA_Bodychunk_Depth,       PREFSIMAGE_DEPTH,\
    MUIA_Bodychunk_Body,        (UBYTE *)PrefsImage_body,\
    MUIA_Bodychunk_Compression, PREFSIMAGE_COMPRESSION,\
    MUIA_Bodychunk_Masking,     PREFSIMAGE_MASKING,\
    MUIA_Bitmap_SourceColors,   (ULONG *)PrefsImage_colors,\
    MUIA_Bitmap_Transparent,    0,\
  End

#include "mccheader.c"
