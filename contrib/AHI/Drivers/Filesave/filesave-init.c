
#include <config.h>

#include <dos/dos.h>
#include <graphics/gfxbase.h>
#include <libraries/asl.h>

#include "library.h"
#include "DriverData.h"

/******************************************************************************
** Custom driver init *********************************************************
******************************************************************************/

BOOL
DriverInit( struct DriverBase* AHIsubBase )
{
  struct FilesaveBase* FilesaveBase = (struct FilesaveBase*) AHIsubBase;

  FilesaveBase->dosbase = OpenLibrary( DOSNAME, 37 );
  FilesaveBase->gfxbase = OpenLibrary( GRAPHICSNAME, 37 );
  FilesaveBase->aslbase = OpenLibrary( AslName, 37);
  FilesaveBase->dtsbase = OpenLibrary( "datatypes.library" ,39 );

  if( DOSBase == NULL )
  {
    Req( "Unable to open '" DOSNAME "' version 37.\n" );
    return FALSE;
  }

  if( GfxBase == NULL )
  {
    Req( "Unable to open '" GRAPHICSNAME "' version 37.\n" );
    return FALSE;
  }

// Don't fail if these ones don't open!

  if( AslBase == NULL )
  {
    Req( "Unable to open '" AslName "' version 37.\n" );
  }

#if 0
  if( DataTypesBase == NULL )
  {
    Req( "Unable to open 'datatypes.library' version 39.\n" );
  }
#endif

  return TRUE;
}


/******************************************************************************
** Custom driver clean-up *****************************************************
******************************************************************************/

VOID
DriverCleanup( struct DriverBase* AHIsubBase )
{
  struct FilesaveBase* FilesaveBase = (struct FilesaveBase*) AHIsubBase;

  CloseLibrary( FilesaveBase->dosbase );
  CloseLibrary( FilesaveBase->gfxbase );
  CloseLibrary( FilesaveBase->aslbase );
  CloseLibrary( FilesaveBase->dtsbase );
}
