/***************************************************************************

 NList.mcc - New List MUI Custom Class
 Registered MUI class, Serial Number:

 Copyright (C) 1996-2004 by Gilles Masson,
                            Carsten Scholling <aphaso@aphaso.de>,
                            Przemyslaw Grunchala,
                            Sebastian Bauer <sebauer@t-online.de>,
                            Jens Langner <Jens.Langner@light-speed.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

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

#include "private.h"
#include "rev.h"

#define VERSION       LIB_VERSION
#define REVISION      LIB_REVISION

#define CLASS         MUIC_NList
#define SUPERCLASS    MUIC_Group

#define	INSTDATA      NLData

#define UserLibID			"$VER: NList.mcc " LIB_REV_STRING CPU " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION	19

#define USEDCLASSESP  used_classesP
static const STRPTR used_classesP[] = { "NListviews.mcp", NULL };

#define	PreClassInit
#define	PostClassExit

#if defined(__MORPHOS__)
struct Library *LayersBase = NULL;
struct Library *DiskfontBase = NULL;
struct Library *ConsoleDevice = NULL;
#else
struct Library *LayersBase = NULL;
struct Library *DiskfontBase = NULL;
struct Device *ConsoleDevice = NULL;
#endif

#if defined(__amigaos4__)
struct LayersIFace *ILayers = NULL;
struct DiskfontIFace *IDiskfont = NULL;
struct ConsoleIFace *IConsole = NULL;
#endif

struct IOStdReq ioreq;

BOOL PreClassInitFunc(void)
{
	if((LayersBase = OpenLibrary("layers.library", 37L)) &&
     GETINTERFACE(ILayers, LayersBase))
	{
		if((DiskfontBase = OpenLibrary("diskfont.library", 37L)) &&
       GETINTERFACE(IDiskfont, DiskfontBase))
		{
			ioreq.io_Message.mn_Length = sizeof(ioreq);

			if(!OpenDevice("console.device", -1L, (struct IORequest *)&ioreq, 0L))
			{
				ConsoleDevice = (APTR)ioreq.io_Device;

        if(GETINTERFACE(IConsole, ConsoleDevice))
        {
				  if(NGR_Create())
				  {
					  return(TRUE);
				  }
       }

        DROPINTERFACE(IConsole);
				CloseDevice((struct IORequest *)&ioreq);
				ConsoleDevice = NULL;
			}

      DROPINTERFACE(IDiskfont);
			CloseLibrary(DiskfontBase);
			DiskfontBase = NULL;
		}

    DROPINTERFACE(ILayers);
		CloseLibrary(LayersBase);
		LayersBase = NULL;
	}

	return(FALSE);
}


VOID PostClassExitFunc( void )
{
	NGR_Delete();

	if(ConsoleDevice)
  {
    DROPINTERFACE(IConsole);
		CloseDevice((struct IORequest *)&ioreq);
	  ConsoleDevice = NULL;
  }

	if(DiskfontBase)
  {
    DROPINTERFACE(IDiskfont);
		CloseLibrary(DiskfontBase);
  	DiskfontBase = NULL;
  }

	if(LayersBase)
  {
    DROPINTERFACE(ILayers);
		CloseLibrary(LayersBase);
	  LayersBase = NULL;
  }
}

/******************************************************************************/
/*                                                                            */
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/*                                                                            */
/******************************************************************************/

#include "mccheader.c"
