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

#define CLASS				MUIC_Floattext
#define SUPERCLASS			MUIC_NFloattext

#define	Data				FTData

#define UserLibID			"$VER: Floattext.mui " LIB_REV_STRING " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION	19

#define	CLASS_VERSIONFAKE

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

