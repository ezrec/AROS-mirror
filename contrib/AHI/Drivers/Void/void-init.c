
#include <config.h>

#include "library.h"
#include "DriverData.h"

/******************************************************************************
** Custom driver init *********************************************************
******************************************************************************/

BOOL
DriverInit( struct DriverBase* AHIsubBase )
{
  struct VoidBase* VoidBase = (struct VoidBase*) AHIsubBase;

  VoidBase->dosbase = OpenLibrary( DOSNAME, 37 );

  if( DOSBase == NULL )
  {
    Req( "Unable to open 'dos.library' version 37.\n" );
    return FALSE;
  }
  
  // Fail if no hardware is present (this check prevents the audio
  // modes from being added to the database if the driver cannot be
  // used).

/*
  if( unable_to_find_hardware )
  {
    Req( "No sound card present.\n" );
    return FALSE;
  }
*/
  return TRUE;
}


/******************************************************************************
** Custom driver clean-up *****************************************************
******************************************************************************/

VOID
DriverCleanup( struct DriverBase* AHIsubBase )
{
  struct VoidBase* VoidBase = (struct VoidBase*) AHIsubBase;

  CloseLibrary( (struct Library*) DOSBase );
}
