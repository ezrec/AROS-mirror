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

#include "mcc_debug.c"

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

#define CLASS				MUIC_NList
#define SUPERCLASS			MUIC_Group

#define	Data				NLData

#define UserLibID			"$VER: NList.mcc " LIB_REV_STRING " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION	19

#define	PreClassInit
#define	PostClassExit

struct Library *LayersBase = NULL;
struct Library *DiskfontBase = NULL;

struct Device *ConsoleDevice = NULL;
struct IOStdReq ioreq;

#ifdef _AROS
int errno;
#endif

BOOL PreClassInitFunc( void )
{
	if ( ( LayersBase = OpenLibrary( "layers.library", 37L ) ) )
	{
		if ( ( DiskfontBase = OpenLibrary( "diskfont.library", 37L ) ) )
		{
			if ( !OpenDevice( "console.device", -1L, (struct IORequest *)&ioreq, 0L ) )
			{
				ConsoleDevice = (struct Device *)ioreq.io_Device;

				if ( NGR_Create() )
				{
					return( TRUE );
				}

				CloseDevice( (struct IORequest *)&ioreq );
				ConsoleDevice = NULL;
			}

			CloseLibrary( DiskfontBase );
			DiskfontBase = NULL;
		}

		CloseLibrary( LayersBase );
		LayersBase = NULL;
	}

	return ( FALSE );
}


VOID PostClassExitFunc( void )
{
	NGR_Delete();

	if ( ConsoleDevice )
		CloseDevice( (struct IORequest *)&ioreq );

	ConsoleDevice = NULL;

	if ( DiskfontBase )
		CloseLibrary( DiskfontBase );

	DiskfontBase = NULL;

	if ( LayersBase )
		CloseLibrary( LayersBase );

	LayersBase = NULL;
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

