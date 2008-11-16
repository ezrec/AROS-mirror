/***************************************************************************

 NListviews.mcp - New Listview MUI Custom Class Preferences
 Registered MUI class, Serial Number: 1d51

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

#define	VERSION				LIB_VERSION
#define	REVISION			LIB_REVISION

#define CLASS				  MUIC_NListviews_mcp
#define SUPERCLASSP		MUIC_Mccprefs

#define	INSTDATAP     NListviews_MCP_Data

#define UserLibID			"$VER: NListviews.mcp " LIB_REV_STRING CPU " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION	19

#define	ClassInit
#define	ClassExit


struct Library *CxBase = NULL;
struct Library *LocaleBase = NULL;
struct Device *ConsoleDevice = NULL;
//struct Catalog *catalog = NULL;

#if defined(__amigaos4__)
struct CommoditiesIFace *ICommodities = NULL;
struct LocaleIFace *ILocale = NULL;
struct ConsoleIFace *IConsole = NULL;
#endif

struct IOStdReq ioreq;

BOOL ClassInitFunc(struct Library *base)
{
	if((CxBase = OpenLibrary("commodities.library", 37L)) &&
     GETINTERFACE(ICommodities, CxBase))
	{
	    	ioreq.io_Message.mn_Length = sizeof(ioreq);
		
		if(!OpenDevice("console.device", -1L, (struct IORequest *)&ioreq, 0L))
		{
			ConsoleDevice = (struct Device *)ioreq.io_Device;
      if(GETINTERFACE(IConsole, ConsoleDevice))
      {
  			if((LocaleBase = OpenLibrary( "locale.library", 38)) &&
           GETINTERFACE(ILocale, LocaleBase))
        {
			    //if ( LocaleBase )
				  //  catalog = OpenCatalogA( NULL, "NListviews.catalog", NULL );

  			  return(TRUE);
        }

        DROPINTERFACE(IConsole);
      }

  		CloseDevice((struct IORequest *)&ioreq);
		}

    DROPINTERFACE(ICommodities);
		CloseLibrary(CxBase);
		CxBase = NULL;
	}

	return(FALSE);
}


VOID ClassExitFunc( struct Library *base )
{
	if(LocaleBase)
	{
		//if ( catalog )
		//	CloseCatalog( catalog );
		//catalog = NULL;

    DROPINTERFACE(ILocale);
		CloseLibrary(LocaleBase);
		LocaleBase	= NULL;
	}

	if(ConsoleDevice)
	{
    DROPINTERFACE(IConsole);
    CloseDevice((struct IORequest *)&ioreq);
  	ConsoleDevice = NULL;
  }

	if(CxBase)
  {
    DROPINTERFACE(ICommodities);
		CloseLibrary(CxBase);
  	CxBase = NULL;
  }
}


/******************************************************************************/
/*                                                                            */
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/*                                                                            */
/******************************************************************************/

#define USE_LIST_BODY   1
#define USE_LIST_COLORS 1
#include "icon.bh"

#include "mccheader.c"

