
#include <config.h>

#include "library.h"
#include "DriverData.h"

/******************************************************************************
** Custom driver init *********************************************************
******************************************************************************/

BOOL
DriverInit( struct DriverBase* AHIsubBase )
{
  struct DeviceBase* DeviceBase = (struct DeviceBase*) AHIsubBase;

  DOSBase = OpenLibrary( "dos.library", 37 );

  if( DOSBase == NULL )
  {
    Req( "Unable to open 'dos.library' version 37.\n" );
    return FALSE;
  }

  return TRUE;
}


/******************************************************************************
** Custom driver clean-up *****************************************************
******************************************************************************/

VOID
DriverCleanup( struct DriverBase* AHIsubBase )
{
  struct DeviceBase* DeviceBase = (struct DeviceBase*) AHIsubBase;

  CloseLibrary( (struct Library*) DOSBase );
}
