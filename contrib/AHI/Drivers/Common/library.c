
/*
 *  Shared library code
 */

#include <config.h>
//#include <CompilerSpecific.h>

#include <exec/alerts.h>
#include <exec/libraries.h>
#include <exec/resident.h>
#include <utility/utility.h>

#include <clib/alib_protos.h>
#include <proto/ahi_sub.h>
#include <proto/exec.h>
#include <proto/utility.h>

#include "gatestubs1.h"
#include "gatestubs2.h"
#include "library.h"

#include "DriverBase.h"
#include "DriverData.h"
#include "version.h"

#ifndef INTUITIONNAME
#define INTUITIONNAME "intuition.library"
#endif

#if !defined( __AROS__ )
extern void _etext;
#else
# define _etext RomTag // Fake it
#endif

/******************************************************************************
** Function prototypes ********************************************************
******************************************************************************/

struct DriverBase*
_LibInit( struct DriverBase* AHIsubBase,
	  BPTR               seglist,
	  struct ExecBase*   sysbase );

BPTR
_LibExpunge( struct DriverBase* AHIsubBase );

struct DriverBase*
_LibOpen( ULONG              version,
	  struct DriverBase* AHIsubBase );

BPTR
_LibClose( struct DriverBase* AHIsubBase );

ULONG
_LibNull( struct DriverBase* AHIsubBase );


/******************************************************************************
** Driver entry ***************************************************************
******************************************************************************/

int
_start( void )
{
  return -1;
}

/******************************************************************************
** Globals ********************************************************************
******************************************************************************/

struct ExecBase* SysBase = NULL;

const char  LibName[]     = DRIVER;
const char  LibIDString[] = DRIVER " " VERS "\r\n";
const UWORD LibVersion    = VERSION;
const UWORD LibRevision   = REVISION;

#if defined( __morphos__ ) || defined( __MORPHOS__ )
ULONG   __abox__=1;
#endif


/******************************************************************************
** Driver resident structure **************************************************
******************************************************************************/

static const APTR FuncTable[] =
{
#if defined( __morphos__ ) || defined( __MORPHOS__ ) || defined( __amithlon__ )
  (APTR) FUNCARRAY_32BIT_NATIVE,
#endif

  gwLibOpen,
  gwLibClose,
  gwLibExpunge,
  gwLibNull,

  gwAHIsub_AllocAudio,
  gwAHIsub_FreeAudio,
  gwAHIsub_Disable,
  gwAHIsub_Enable,
  gwAHIsub_Start,
  gwAHIsub_Update,
  gwAHIsub_Stop,
  gwAHIsub_SetVol,
  gwAHIsub_SetFreq,
  gwAHIsub_SetSound,
  gwAHIsub_SetEffect,
  gwAHIsub_LoadSound,
  gwAHIsub_UnloadSound,
  gwAHIsub_GetAttr,
  gwAHIsub_HardwareControl,
  (APTR) -1
};


static const APTR InitTable[4] =
{
  (APTR) DRIVERBASE_SIZEOF,
  (APTR) &FuncTable,
  NULL,
  (APTR) gwLibInit
};


// This structure must reside in the text segment or the read-only
// data segment!  "const" makes it happen.

static const struct Resident RomTag =
{
  RTC_MATCHWORD,
  (struct Resident *) &RomTag,
  (struct Resident *) &_etext,
  RTF_AUTOINIT,
  VERSION,
  NT_LIBRARY,
  0,                      /* priority */
  (BYTE *) LibName,
  (BYTE *) LibIDString,
  (APTR) &InitTable
};


/******************************************************************************
** Message requester **********************************************************
******************************************************************************/

void
ReqA( const char*        text,
      APTR               args,
      struct DriverBase* AHIsubBase )
{
  struct EasyStruct es = 
  {
    sizeof (struct EasyStruct),
    0,
    (STRPTR) LibName,
    (STRPTR) text,
    "OK"
  };

  EasyRequestArgs( NULL, &es, NULL, args );
}

/******************************************************************************
** Library init ***************************************************************
******************************************************************************/

struct DriverBase*
_LibInit( struct DriverBase* AHIsubBase,
	  BPTR               seglist,
	  struct ExecBase*   sysbase )
{
  SysBase = sysbase;

  AHIsubBase->library.lib_Node.ln_Type = NT_LIBRARY;
  AHIsubBase->library.lib_Node.ln_Name = (STRPTR) LibName;
  AHIsubBase->library.lib_Flags        = LIBF_SUMUSED | LIBF_CHANGED;
  AHIsubBase->library.lib_Version      = VERSION;
  AHIsubBase->library.lib_Revision     = REVISION;
  AHIsubBase->library.lib_IdString     = (STRPTR) LibIDString;
  AHIsubBase->seglist                  = seglist;

  AHIsubBase->intuitionbase = OpenLibrary( INTUITIONNAME, 37 );
  AHIsubBase->utilitybase   = OpenLibrary( UTILITYNAME, 37 );

  if( IntuitionBase == NULL )
  {
    Alert( AN_Unknown|AG_OpenLib|AO_Intuition );
    goto error;
  }
  
  if( UtilityBase == NULL )
  {
    Req( "Unable to open 'utility.library' version 37.\n" );
    goto error;
  }

  if( ! DriverInit( AHIsubBase ) )
  {
    goto error;
  }


  return AHIsubBase;

error:
  _LibExpunge( AHIsubBase );
  return NULL;
}


/******************************************************************************
** Library clean-up ***********************************************************
******************************************************************************/

BPTR
_LibExpunge( struct DriverBase* AHIsubBase )
{
  BPTR seglist = 0;

  if( AHIsubBase->library.lib_OpenCnt == 0 )
  {
    seglist = AHIsubBase->seglist;

    /* Since LibInit() calls us on failure, we have to check if we're
       really added to the library list before removing us. */

    if( AHIsubBase->library.lib_Node.ln_Succ != NULL )
    {
      Remove( (struct Node *) AHIsubBase );
    }

    DriverCleanup( AHIsubBase );
    
    /* Close libraries */
    CloseLibrary( (struct Library*) IntuitionBase );
    CloseLibrary( (struct Library*) UtilityBase );

    FreeMem( (APTR) ( ( (char*) AHIsubBase ) -
		      AHIsubBase->library.lib_NegSize ),
             ( AHIsubBase->library.lib_NegSize +
	       AHIsubBase->library.lib_PosSize ) );
  }
  else
  {
    AHIsubBase->library.lib_Flags |= LIBF_DELEXP;
  }

  return seglist;
}


/******************************************************************************
** Library opening ************************************************************
******************************************************************************/

struct DriverBase*
_LibOpen( ULONG              version,
	  struct DriverBase* AHIsubBase )
{
  AHIsubBase->library.lib_Flags &= ~LIBF_DELEXP;
  AHIsubBase->library.lib_OpenCnt++;

  return AHIsubBase;
}


/******************************************************************************
** Library closing ************************************************************
******************************************************************************/

BPTR
_LibClose( struct DriverBase* AHIsubBase )
{
  BPTR seglist = 0;

  AHIsubBase->library.lib_OpenCnt--;

  if( AHIsubBase->library.lib_OpenCnt == 0 )
  {
    if( AHIsubBase->library.lib_Flags & LIBF_DELEXP )
    {
      seglist = _LibExpunge( AHIsubBase );
    }
  }

  return seglist;
}


/******************************************************************************
** Unused function ************************************************************
******************************************************************************/

ULONG
_LibNull( struct DriverBase* AHIsubBase )
{
  return 0;
}
