/* $Id$ */

/*
     AHI - Hardware independent audio subsystem
     Copyright (C) 1996-1999 Martin Blom <martin@blom.org>
     
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
     
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
     
     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the
     Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
     MA 02139, USA.
*/

#include <config.h>
#include <CompilerSpecific.h>

#include <exec/resident.h>
#include <exec/alerts.h>
#include <exec/execbase.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include <powerup/ppclib/object.h>
#include <powerup/ppclib/interface.h>
#include <proto/ppc.h>

#include "ahi_def.h"
#include "header.h"
#include "device.h"
#include "localize.h"
#include "addroutines.h"
#include "misc.h"

#include "version.h"

static BOOL
OpenLibs ( void );

static void
CloseLibs ( void );

#define GetSymbol( name ) AHIGetELFSymbol( #name, (void*) &name ## Ptr )

/******************************************************************************
** Device entry ***************************************************************
******************************************************************************/

int Start( void )
{
  return -1;
}


/******************************************************************************
** Device residend strcuture **************************************************
******************************************************************************/

extern void _etext;
extern const char DevName[];
extern const char IDString[];
static const APTR InitTable[4];

static const struct Resident RomTag =
{
  RTC_MATCHWORD,
  (struct Resident *) &RomTag,
  (APTR) &_etext,
  RTF_AUTOINIT,
  VERSION,
  NT_DEVICE,
  0,                      /* priority */
  (BYTE *) DevName,
  (BYTE *) IDString,
  (APTR) &InitTable
};


/******************************************************************************
** Globals ********************************************************************
******************************************************************************/

struct ExecBase           *SysBase        = NULL;
struct AHIBase            *AHIBase        = NULL;
struct DosLibrary         *DOSBase        = NULL;
struct Library            *GadToolsBase   = NULL;
struct GfxBase            *GfxBase        = NULL;
struct Library            *IFFParseBase   = NULL;
struct IntuitionBase      *IntuitionBase  = NULL;
struct LocaleBase         *LocaleBase     = NULL;
struct Device             *TimerBase      = NULL;
struct UtilityBase        *UtilityBase    = NULL;
struct Library            *PowerPCBase    = NULL;
struct Library            *PPCLibBase     = NULL;
void                      *PPCObject      = NULL;

ADDFUNC* AddByteMonoPtr                   = NULL;
ADDFUNC* AddByteStereoPtr                 = NULL;
ADDFUNC* AddBytesMonoPtr                  = NULL;
ADDFUNC* AddBytesStereoPtr                = NULL;
ADDFUNC* AddWordMonoPtr                   = NULL;
ADDFUNC* AddWordStereoPtr                 = NULL;
ADDFUNC* AddWordsMonoPtr                  = NULL;
ADDFUNC* AddWordsStereoPtr                = NULL;
ADDFUNC* AddByteMonoBPtr                  = NULL;
ADDFUNC* AddByteStereoBPtr                = NULL;
ADDFUNC* AddBytesMonoBPtr                 = NULL;
ADDFUNC* AddBytesStereoBPtr               = NULL;
ADDFUNC* AddWordMonoBPtr                  = NULL;
ADDFUNC* AddWordStereoBPtr                = NULL;
ADDFUNC* AddWordsMonoBPtr                 = NULL;
ADDFUNC* AddWordsStereoBPtr               = NULL;

ADDFUNC* AddLofiByteMonoPtr               = NULL;
ADDFUNC* AddLofiByteStereoPtr             = NULL;
ADDFUNC* AddLofiBytesMonoPtr              = NULL;
ADDFUNC* AddLofiBytesStereoPtr            = NULL;
ADDFUNC* AddLofiWordMonoPtr               = NULL;
ADDFUNC* AddLofiWordStereoPtr             = NULL;
ADDFUNC* AddLofiWordsMonoPtr              = NULL;
ADDFUNC* AddLofiWordsStereoPtr            = NULL;
ADDFUNC* AddLofiByteMonoBPtr              = NULL;
ADDFUNC* AddLofiByteStereoBPtr            = NULL;
ADDFUNC* AddLofiBytesMonoBPtr             = NULL;
ADDFUNC* AddLofiBytesStereoBPtr           = NULL;
ADDFUNC* AddLofiWordMonoBPtr              = NULL;
ADDFUNC* AddLofiWordStereoBPtr            = NULL;
ADDFUNC* AddLofiWordsMonoBPtr             = NULL;
ADDFUNC* AddLofiWordsStereoBPtr           = NULL;

/* linker can use symbol b for symbol a if a is not defined */
#define ALIAS(a,b) asm(".stabs \"_" #a "\",11,0,0,0\n.stabs \"_" #b "\",1,0,0,0")

ALIAS( __UtilityBase, UtilityBase );

const ULONG			           DriverVersion  = 2;
const ULONG			           Version        = VERSION;
const ULONG			           Revision       = REVISION;

const char DevName[]   = AHINAME;
const char IDString[]  = AHINAME " " VERS "\r\n";

static const char VersTag[] =
 "$VER: " AHINAME " " VERS " ©1994-1999 Martin Blom. "
 CPU 
 "/PPC"
 " version.\r\n";

enum MixBackend_t          MixBackend     = MB_NATIVE;

/******************************************************************************
** Device code ****************************************************************
******************************************************************************/

static struct AHIBase * ASMCALL
initRoutine( REG( d0, struct AHIBase* device ),
	     REG( a0, APTR seglist ),
	     REG( a6, struct ExecBase* sysbase ) )
{
  SysBase = sysbase;
  AHIBase = device;

  AHIBase->ahib_Library.lib_Node.ln_Type = NT_DEVICE;
  AHIBase->ahib_Library.lib_Node.ln_Name = (STRPTR) DevName;
  AHIBase->ahib_Library.lib_Flags        = LIBF_SUMUSED | LIBF_CHANGED;
  AHIBase->ahib_Library.lib_Version      = VERSION;
  AHIBase->ahib_Library.lib_Revision     = REVISION;
  AHIBase->ahib_Library.lib_IdString     = (STRPTR) IDString;
  AHIBase->ahib_SysLib  = sysbase;
  AHIBase->ahib_SegList = (ULONG) seglist;

#ifdef MC68020_PLUS
  if( ( SysBase->AttnFlags & AFF_68020 ) == 0 )
  {
    Alert( ( AN_Unknown | ACPU_InstErr ) & (~AT_DeadEnd) );
    return NULL;
  }
#endif

  InitSemaphore( &AHIBase->ahib_Lock );

  if( !OpenLibs() )
  {
    return NULL;
  }

  return AHIBase;
}


BPTR ASMCALL
DevExpunge( REG( a6, struct AHIBase* device ) )
{
  BPTR seglist = 0;

  device->ahib_Library.lib_Flags |= LIBF_DELEXP;

  if( device->ahib_Library.lib_OpenCnt == 0)
  {
    seglist = device->ahib_SegList;

    Remove( (struct Node *) device );

    CloseLibs();

    FreeMem( (APTR) ( ( (char*) device ) - device->ahib_Library.lib_NegSize ),
	     device->ahib_Library.lib_NegSize + device->ahib_Library.lib_PosSize );
  }

  return seglist;
}


int Null( void )
{
  return 0;
}


extern APTR DevOpen;
extern APTR DevClose;

extern APTR DevBeginIO;
extern APTR DevAbortIO;

extern APTR AllocAudioA;
extern APTR FreeAudio;
extern APTR KillAudio;
extern APTR ControlAudioA;
extern APTR SetVol;
extern APTR SetFreq;
extern APTR SetSound;
extern APTR SetEffect;
extern APTR LoadSound;
extern APTR UnloadSound;
extern APTR NextAudioID;
extern APTR GetAudioAttrsA;
extern APTR BestAudioIDA;
extern APTR AllocAudioRequestA;
extern APTR AudioRequestA;
extern APTR FreeAudioRequest;
extern APTR PlayA;
extern APTR SampleFrameSize;
extern APTR AddAudioMode;
extern APTR RemoveAudioMode;
extern APTR LoadModeFile;


static const APTR funcTable[] =
{
  &DevOpen,
  &DevClose,
  &DevExpunge,
  &Null,

  &DevBeginIO,
  &DevAbortIO,

  &AllocAudioA,
  &FreeAudio,
  &KillAudio,
  &ControlAudioA,
  &SetVol,
  &SetFreq,
  &SetSound,
  &SetEffect,
  &LoadSound,
  &UnloadSound,
  &NextAudioID,
  &GetAudioAttrsA,
  &BestAudioIDA,
  &AllocAudioRequestA,
  &AudioRequestA,
  &FreeAudioRequest,
  &PlayA,
  &SampleFrameSize,
  &AddAudioMode,
  &RemoveAudioMode,
  &LoadModeFile,
  (APTR) -1
};


static const APTR InitTable[4] =
{
  (APTR) sizeof( struct AHIBase ),
  (APTR) &funcTable,
  0,
  (APTR) initRoutine
};


static struct timerequest *TimerIO        = NULL;
static struct timeval     *timeval        = NULL;

/******************************************************************************
** OpenLibs *******************************************************************
******************************************************************************/

// This function is called by the device startup code when the device is
// first loaded into memory.

static BOOL
OpenLibs ( void )
{
  /* Intuition Library */

  IntuitionBase = (struct IntuitionBase *) OpenLibrary( "intuition.library", 37 );

  if( IntuitionBase == NULL)
  {
    Alert(AN_Unknown|AG_OpenLib|AO_Intuition);
    return FALSE;
  }

  /* DOS Library */

  DOSBase = (struct DosLibrary *) OpenLibrary( "dos.library", 37 );

  if( DOSBase == NULL)
  {
    Req( "Unable to open 'dos.library'." );
    return FALSE;
  }

  /* Graphics Library */

  GfxBase = (struct GfxBase *) OpenLibrary( "graphics.library", 37 );

  if( GfxBase == NULL)
  {
    Req( "Unable to open 'graphics.library'." );
    return FALSE;
  }

  /* GadTools Library */

  GadToolsBase = OpenLibrary( "gadtools.library", 37 );

  if( GadToolsBase == NULL)
  {
    Req( "Unable to open 'gadtools.library'." );
    return FALSE;
  }

  /* IFFParse Library */

  IFFParseBase = OpenLibrary( "iffparse.library", 37 );

  if( IFFParseBase == NULL)
  {
    Req( "Unable to open 'iffparse.library'." );
    return FALSE;
  }

  /* Locale Library */

  LocaleBase = (struct LocaleBase*) OpenLibrary( "locale.library", 38 );

  /* Timer Device */

  TimerIO = (struct timerequest *) AllocVec( sizeof(struct timerequest),
                                             MEMF_PUBLIC | MEMF_CLEAR );

  if( TimerIO == NULL)
  {
    Req( "Out of memory." );
    return FALSE;
  }

  timeval = (struct timeval *) AllocVec( sizeof(struct timeval),
                                         MEMF_PUBLIC | MEMF_CLEAR);

  if( timeval == NULL)
  {
    Req( "Out of memory." );
    return FALSE;
  }

  if( OpenDevice( "timer.device",
                  UNIT_MICROHZ,
                  (struct IORequest *)
                  TimerIO,
                  0) != 0 )
  {
    Req( "Unable to open 'timer.device'." );
    return FALSE;
  }

  TimerBase = (struct Device *) TimerIO->tr_node.io_Device;

  /* Utility Library */

  UtilityBase = (struct UtilityBase *) OpenLibrary( "utility.library", 37 );

  if( UtilityBase == NULL)
  {
    Req( "Unable to open 'utility.library'." );
    return FALSE;
  }

  // Fill in some defaults...

  MixBackend = MB_NATIVE;

  AddByteMonoPtr         = AddByteMono;
  AddByteStereoPtr       = AddByteStereo;
  AddBytesMonoPtr        = AddBytesMono;
  AddBytesStereoPtr      = AddBytesStereo;
  AddWordMonoPtr         = AddWordMono;
  AddWordStereoPtr       = AddWordStereo;
  AddWordsMonoPtr        = AddWordsMono;
  AddWordsStereoPtr      = AddWordsStereo;
  AddByteMonoBPtr        = AddByteMonoB;
  AddByteStereoBPtr      = AddByteStereoB;
  AddBytesMonoBPtr       = AddBytesMonoB;
  AddBytesStereoBPtr     = AddBytesStereoB;
  AddWordMonoBPtr        = AddWordMonoB;
  AddWordStereoBPtr      = AddWordStereoB;
  AddWordsMonoBPtr       = AddWordsMonoB;
  AddWordsStereoBPtr     = AddWordsStereoB;

  AddLofiByteMonoPtr     = AddLofiByteMono;
  AddLofiByteStereoPtr   = AddLofiByteStereo;
  AddLofiBytesMonoPtr    = AddLofiBytesMono;
  AddLofiBytesStereoPtr  = AddLofiBytesStereo;
  AddLofiWordMonoPtr     = AddLofiWordMono;
  AddLofiWordStereoPtr   = AddLofiWordStereo;
  AddLofiWordsMonoPtr    = AddLofiWordsMono;
  AddLofiWordsStereoPtr  = AddLofiWordsStereo;
  AddLofiByteMonoBPtr    = AddLofiByteMonoB;
  AddLofiByteStereoBPtr  = AddLofiByteStereoB;
  AddLofiBytesMonoBPtr   = AddLofiBytesMonoB;
  AddLofiBytesStereoBPtr = AddLofiBytesStereoB;
  AddLofiWordMonoBPtr    = AddLofiWordMonoB;
  AddLofiWordStereoBPtr  = AddLofiWordStereoB;
  AddLofiWordsMonoBPtr   = AddLofiWordsMonoB;
  AddLofiWordsStereoBPtr = AddLofiWordsStereoB;

  /* PPC/PowerPC library loading

     Strategy:

      1) If WarpUp is running, use it.
      2) If WarpUp is is not running, but PowerUp is, use it
      3) If neither of them are running, try WarpUp.
      4) Finally, try PowerUp.

     Result:

      If both kernels are running, WarpUp will be used, since the PowerUp
      kernel is the ppc.library emulation. (This is going to work until
      somebody writes a WarpUp emulation for ppc.library....)

      If only one kernel is already running, it will be used.

      WarpUp will be used if no kernel is loaded, and WarpUp exists, since
      WarpUp was selected for OS 3.5.  If WarpUp was not found, PowerUp
      will be used.

  */

  // Check if WarpUp or PowerUp are already installed...

  Forbid();
  PowerPCBase = (struct Library *) FindName( &SysBase->LibList,
                                             "powerpc.library" );
  PPCLibBase  = (struct Library *) FindName( &SysBase->LibList,
                                             "ppc.library" );
  Permit();

  if( PowerPCBase != NULL 
      || ( PowerPCBase == NULL && PPCLibBase == NULL ) )
  {
    // Open WarpUp
    PowerPCBase = OpenLibrary( "powerpc.library", 14 );
    PPCLibBase  = NULL;
  }

  if( PPCLibBase != NULL 
      || ( PPCLibBase == NULL && PowerPCBase == NULL ) )
  {
    // Open PoweUp
    PPCLibBase  = OpenLibrary( "ppc.library", 46 );
    PowerPCBase = NULL;
  }

  if( PPCLibBase != NULL 
      && PPCLibBase->lib_Version == 46 
      && PPCLibBase->lib_Revision < 24 )
  {
    Req( "Need at least version 46.24 of 'ppc.library'." );
    return FALSE;
  }

  if( PPCLibBase != NULL )
  {
    MixBackend  = MB_POWERUP;
  }
  
  if( PowerPCBase != NULL )
  {
    MixBackend  = MB_WARPUP;
  }

  if( PPCLibBase != NULL || PowerPCBase != NULL )
  {
    ULONG* version = NULL;
    ULONG* revision = NULL;

    /* Load our code to PPC..  */

    PPCObject = AHILoadObject( "DEVS:ahi.elf" );

    if( PPCObject != NULL )
    {
      int r = ~0;

      AHIGetELFSymbol( "__LIB_Version", (void*) &version );
      AHIGetELFSymbol( "__LIB_Revision", (void*) &revision );
    
      if( version == NULL || revision == NULL )
      {
        Req( "Unable to fetch version information from 'ahi.elf'." );
      }
      if( *version != Version || *revision != Revision )
      {
        Req( "'ahi.elf' version %ld.%ld doesn't match 'ahi.device' %ld.%ld.",
             *version, *revision, Version, Revision );

        return FALSE;
      }

      r &= GetSymbol( AddByteMono     );
      r &= GetSymbol( AddByteStereo   );
      r &= GetSymbol( AddBytesMono    );
      r &= GetSymbol( AddBytesStereo  );
      r &= GetSymbol( AddWordMono     );
      r &= GetSymbol( AddWordStereo   );
      r &= GetSymbol( AddWordsMono    );
      r &= GetSymbol( AddWordsStereo  );
      r &= GetSymbol( AddByteMonoB    );
      r &= GetSymbol( AddByteStereoB  );
      r &= GetSymbol( AddBytesMonoB   );
      r &= GetSymbol( AddBytesStereoB );
      r &= GetSymbol( AddWordMonoB    );
      r &= GetSymbol( AddWordStereoB  );
      r &= GetSymbol( AddWordsMonoB   );
      r &= GetSymbol( AddWordsStereoB );

      r &= GetSymbol( AddLofiByteMono     );
      r &= GetSymbol( AddLofiByteStereo   );
      r &= GetSymbol( AddLofiBytesMono    );
      r &= GetSymbol( AddLofiBytesStereo  );
      r &= GetSymbol( AddLofiWordMono     );
      r &= GetSymbol( AddLofiWordStereo   );
      r &= GetSymbol( AddLofiWordsMono    );
      r &= GetSymbol( AddLofiWordsStereo  );
      r &= GetSymbol( AddLofiByteMonoB    );
      r &= GetSymbol( AddLofiByteStereoB  );
      r &= GetSymbol( AddLofiBytesMonoB   );
      r &= GetSymbol( AddLofiBytesStereoB );
      r &= GetSymbol( AddLofiWordMonoB    );
      r &= GetSymbol( AddLofiWordStereoB  );
      r &= GetSymbol( AddLofiWordsMonoB   );
      r &= GetSymbol( AddLofiWordsStereoB );

      if( r == 0 )
      {
        Req( "Unable to fetch all symbols from ELF object." );
        return FALSE;
      }
    }
    else
    {
      // PPC kernel found, but no ELF object. m68k mixing will be used.
    }
  }

  OpenahiCatalog(NULL, NULL);

  {
    char buf[2];
    
    if( GetVar( "AHINoBetaRequester", buf, sizeof( buf ), 0 ) == -1 )
    {
      Req( "This is a beta release of AHI. The latest supported \n"
           "version is 4.180, which can be found at\n"
           "<URL:http://www.lysator.liu.se/~lcs/ahi.html>.\n"
           "\n"
           "Detailed bug reports and patches are welcome.\n"
           "Sound kernel in use: %s.\n"
           "\n"
           "/Martin Blom <martin@blom.org>\n",
           PPCObject == NULL ? CPU :
             ( PPCLibBase == NULL ? "WarpUp" : "PowerUp" ) );
           
    }
  }

  return TRUE;
}


/******************************************************************************
** CloseLibs *******************************************************************
******************************************************************************/

// This function is called by DevExpunge() when the device is about to be
// flushed

static void
CloseLibs ( void )
{
  CloseahiCatalog();

  if( PPCObject != NULL )
  {
    AHIUnloadObject( PPCObject );
  }

  CloseLibrary( PPCLibBase );
  CloseLibrary( PowerPCBase );

  CloseLibrary( (struct Library *) UtilityBase );
  if( TimerIO  != NULL )
  {
    CloseDevice( (struct IORequest *) TimerIO );
  }
  FreeVec( timeval );
  FreeVec( TimerIO );
  CloseLibrary( (struct Library *) LocaleBase );
  CloseLibrary( (struct Library *) IntuitionBase );
  CloseLibrary( IFFParseBase );
  CloseLibrary( GadToolsBase );
  CloseLibrary( (struct Library *) GfxBase );
  CloseLibrary( (struct Library *) DOSBase );

}
