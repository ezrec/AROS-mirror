
#include <config.h>

#include <dos/dos.h>
#include <graphics/gfxbase.h>
#include <libraries/asl.h>
#include <proto/exec.h>
#include <proto/datatypes.h>

#ifndef __AMIGAOS4__
#include "library.h"
#else
#include "library_card.h"
struct DOSIFace*            IDOS    = NULL;
struct UtilityIFace*        IUtility = NULL;
struct AHIsubIFace*         IAHIsub = NULL;
struct ExecIFace*           IExec = NULL;
struct AslIFace*            IAsl = NULL;
struct DataTypesIFace*      IDataTypes = NULL;
#endif

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

#ifdef __AMIGAOS4__
  DOSBase  = (struct DosLibrary*) OpenLibrary( DOSNAME, 37 );

  if( DOSBase == NULL )
  {
    Req( "Unable to open 'dos.library' version 37.\n" );
    return FALSE;
  }

  if ((IDOS = (struct DOSIFace *) GetInterface((struct Library *) DOSBase, "main", 1, NULL)) == NULL)
  {
       Req("Couldn't open IDOS interface!\n");
       return FALSE;
  }

  if ((IAHIsub = (struct AHIsubIFace *) GetInterface((struct Library *) AHIsubBase, "main", 1, NULL)) == NULL)
  {
       Req("Couldn't open IAHIsub interface!\n");
       return FALSE;
  }
  
  if ((IUtility = (struct UtilityIFace *) GetInterface((struct Library *) UtilityBase, "main", 1, NULL)) == NULL)
  {
       Req("Couldn't open IUtility interface!\n");
       return FALSE;
  }
  
  if ((IAsl = (struct AslIFace *) GetInterface((struct Library *) AslBase, "main", 1, NULL)) == NULL)
  {
      Req("Couldn't open IAsl interface!\n");
      return FALSE;
  }
  
  if ((IDataTypes = (struct DataTypesIFace *) GetInterface((struct Library *) DataTypesBase, "main", 1, NULL)) == NULL)
  {
      Req("Couldn't open IDataTypes interface!\n");
      return FALSE;
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

#ifdef __AMIGAOS4__
  if (IUtility)
    DropInterface( (struct Interface *) IUtility);
  if (IAHIsub)
    DropInterface( (struct Interface *) IAHIsub);
  if (IDOS)
    DropInterface( (struct Interface *) IDOS);
  if (IAsl)
    DropInterface( (struct Interface *) IAsl);
  if (IDataTypes)
    DropInterface( (struct Interface *) IDataTypes);
#endif

  CloseLibrary( FilesaveBase->dosbase );
  CloseLibrary( FilesaveBase->gfxbase );
  CloseLibrary( FilesaveBase->aslbase );
  CloseLibrary( FilesaveBase->dtsbase );
}
