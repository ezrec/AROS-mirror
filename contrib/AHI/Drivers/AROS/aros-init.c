
#include <config.h>

#include "library.h"
#include "DriverData.h"

/******************************************************************************
** Custom driver init *********************************************************
******************************************************************************/

BOOL
DriverInit( struct DriverBase* AHIsubBase )
{
  struct AROSBase* AROSBase = (struct AROSBase*) AHIsubBase;

  DOSBase = OpenLibrary( DOSNAME, 37 );

  if( DOSBase == NULL )
  {
    Req( "Unable to open 'dos.library' version 37.\n" );
    return FALSE;
  }

  OSSBase = OpenLibrary( "oss.library", 0 );

  if( OSSBase == NULL )
  {
    Req( "Unable to open 'oss.library'.\n" );
    return FALSE;
  }
  
  // Fail if no hardware is present (this check prevents the audio
  // modes from being added to the database if the driver cannot be
  // used).

  if( ! OSS_Open( "/dev/dsp", FALSE, TRUE, FALSE ) )
  {
    Req( "No sound card present.\n" );
    return FALSE;
  }

  OSS_Close();

  return TRUE;
}


/******************************************************************************
** Custom driver clean-up *****************************************************
******************************************************************************/

VOID
DriverCleanup( struct DriverBase* AHIsubBase )
{
  struct AROSBase* AROSBase = (struct AROSBase*) AHIsubBase;

  CloseLibrary( (struct Library*) DOSBase );
  CloseLibrary( OSSBase );
}
