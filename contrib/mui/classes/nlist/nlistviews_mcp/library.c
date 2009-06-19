/***************************************************************************

 NListviews.mcp - New Listview MUI Custom Class Preferences
 Registered MUI class, Serial Number: 1d51 (0x9d510001 to 0x9d51001F
                                            and 0x9d510101 to 0x9d51013F)

 Copyright (C) 1996-2001 by Gilles Masson
 Copyright (C) 2001-2005 by NList Open Source Team

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

#include <proto/exec.h>
#include <proto/intuition.h>

/******************************************************************************/
/*                                                                            */
/* MCC/MCP name and version                                                   */
/*                                                                            */
/* ATTENTION:  The FIRST LETTER of NAME MUST be UPPERCASE                     */
/*                                                                            */
/******************************************************************************/

#include "private.h"
#include "version.h"

#define	VERSION				LIB_VERSION
#define	REVISION			LIB_REVISION

#define CLASS				  MUIC_NListviews_mcp
#define SUPERCLASSP		MUIC_Mccprefs

#define	INSTDATAP     NListviews_MCP_Data

#define USERLIBID			CLASS " " LIB_REV_STRING CPU " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION	19

#define	CLASSINIT
#define	CLASSEXPUNGE

#define MIN_STACKSIZE 8192

#include "locale.h"

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

/******************************************************************************/
/* define the functions used by the startup code ahead of including mccinit.c */
/******************************************************************************/
static BOOL ClassInit(UNUSED struct Library *base);
static VOID ClassExpunge(UNUSED struct Library *base);

/******************************************************************************/
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/******************************************************************************/
#define USE_LIST_BODY   1
#define USE_LIST_COLORS 1
#include "icon.bh"
#include "mccinit.c"

/******************************************************************************/
/* define all implementations of our user functions                           */
/******************************************************************************/
static BOOL ClassInit(UNUSED struct Library *base)
{
	if((CxBase = OpenLibrary("commodities.library", 37L)) &&
     GETINTERFACE(ICommodities, struct CommoditiesIFace *, CxBase))
	{
		ioreq.io_Message.mn_Length = sizeof(ioreq);
		if(!OpenDevice("console.device", -1L, (struct IORequest *)&ioreq, 0L))
		{
			ConsoleDevice = (struct Device *)ioreq.io_Device;
      if(GETINTERFACE(IConsole, struct ConsoleIFace *, ConsoleDevice))
      {
  			if((LocaleBase = OpenLibrary( "locale.library", 38)) &&
           GETINTERFACE(ILocale, struct LocaleIFace *, LocaleBase))
        {
          // open the NListviews_mcp catalog
          OpenCat();

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


static VOID ClassExpunge(UNUSED struct Library *base)
{
  // close the catalog
	CloseCat();

  if(LocaleBase)
	{
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

