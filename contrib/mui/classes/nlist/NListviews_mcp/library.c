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

//$$$Sensei: return 0 if someone is tring to run us.
int	main( void )
{
	return( 0 );
}

#include <mcc_debug.c>

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

#define CLASS				MUIC_NListviews_mcp
#define SUPERCLASSP			MUIC_Mccprefs

#define	DataP				NListviews_MCP_Data

#define UserLibID			"$VER: NListviews.mcp " LIB_REV_STRING " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION	19

#define	ClassInit
#define	ClassExit


struct Library *CxBase = NULL;
struct Library *LocaleBase = NULL;
struct Catalog *catalog = NULL;

struct ConsoleDevice *ConsoleDevice = NULL;
struct IOStdReq ioreq;



BOOL ClassInitFunc( struct Library *base )
{
	if ( CxBase = OpenLibrary( "commodities.library", 37L ) )
	{
		if ( !OpenDevice( "console.device", -1L, (struct IORequest *)&ioreq, 0L ) )
		{
			ConsoleDevice = (struct ConsoleDevice *)ioreq.io_Device;

			LocaleBase = OpenLibrary( "locale.library", 38 );
/*
			if ( LocaleBase )
				catalog = OpenCatalogA( NULL, "NListviews.catalog", NULL );
*/
			return( TRUE );
		}

		CloseLibrary( CxBase );
		CxBase = NULL;
	}

	return ( FALSE );
}


VOID ClassExitFunc( struct Library *base )
{
	if ( LocaleBase )
	{
/*
		if ( catalog )
			CloseCatalog( catalog );

		catalog = NULL;
*/

		CloseLibrary( LocaleBase );

		LocaleBase	= NULL;
	}

	if ( ConsoleDevice )
		CloseDevice( (struct IORequest *)&ioreq );

	ConsoleDevice = NULL;

	if ( CxBase )
		CloseLibrary( CxBase );

	CxBase = NULL;
}


/******************************************************************************/
/*                                                                            */
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/*                                                                            */
/******************************************************************************/

/* define that if you want the mcc class to be expunge at last close */
/* which can be very nice for tests (can avoid lot of avail flush) ! */
/*#define EXPUNGE_AT_LAST_CLOSE*/

#include "mccheader.c"

#include <proto/muimaster.h>

