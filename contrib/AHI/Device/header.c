/* $Id$ */

/*
     AHI - Hardware independent audio subsystem
     Copyright (C) 1996-2003 Martin Blom <martin@blom.org>
     
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

#include <exec/memory.h>
#include <exec/resident.h>
#include <exec/alerts.h>
#include <exec/execbase.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include "ahi_def.h"

#include "header.h"
#include "gateway.h"
#include "localize.h"
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

#if defined( __amithlon__ )
#error blah
__asm(
"         .text;\n"
"         .byte 0x4e, 0xfa, 0x00, 0x03\n"
"         jmp _start" );
#endif

int
_start( void )
{
  return -1;
}

#if defined( __morphos__ ) || defined( __MORPHOS__ )
ULONG   __abox__=1;
ULONG   __amigappc__=1;  // deprecated
#endif

/******************************************************************************
** Device resident structure **************************************************
******************************************************************************/

extern const char DevName[];
extern const char IDString[];
static const APTR InitTable[4];

// This structure must reside in the text segment or the read-only data segment!
// "const" makes it happen.
static const struct Resident RomTag =
{
  RTC_MATCHWORD,
  (struct Resident *) &RomTag,
  (struct Resident *) &RomTag + 1,
#if defined( __morphos__ ) || defined( __MORPHOS__ ) || defined( __amithlon__ )
  RTF_PPC | RTF_AUTOINIT,
#else
  RTF_AUTOINIT,
#endif
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
struct Resident           *MorphOSRes     = NULL;

#if defined( ENABLE_WARPUP )
struct Library            *PowerPCBase    = NULL;
void                      *PPCObject      = NULL;
#endif

ADDFUNC* AddByteMonoPtr                   = NULL;
ADDFUNC* AddByteStereoPtr                 = NULL;
ADDFUNC* AddBytesMonoPtr                  = NULL;
ADDFUNC* AddBytesStereoPtr                = NULL;
ADDFUNC* AddWordMonoPtr                   = NULL;
ADDFUNC* AddWordStereoPtr                 = NULL;
ADDFUNC* AddWordsMonoPtr                  = NULL;
ADDFUNC* AddWordsStereoPtr                = NULL;
ADDFUNC* AddLongMonoPtr                   = NULL;
ADDFUNC* AddLongStereoPtr                 = NULL;
ADDFUNC* AddLongsMonoPtr                  = NULL;
ADDFUNC* AddLongsStereoPtr                = NULL;
ADDFUNC* AddByteMonoBPtr                  = NULL;
ADDFUNC* AddByteStereoBPtr                = NULL;
ADDFUNC* AddBytesMonoBPtr                 = NULL;
ADDFUNC* AddBytesStereoBPtr               = NULL;
ADDFUNC* AddWordMonoBPtr                  = NULL;
ADDFUNC* AddWordStereoBPtr                = NULL;
ADDFUNC* AddWordsMonoBPtr                 = NULL;
ADDFUNC* AddWordsStereoBPtr               = NULL;
ADDFUNC* AddLongMonoBPtr                  = NULL;
ADDFUNC* AddLongStereoBPtr                = NULL;
ADDFUNC* AddLongsMonoBPtr                 = NULL;
ADDFUNC* AddLongsStereoBPtr               = NULL;

ADDFUNC* AddLofiByteMonoPtr               = NULL;
ADDFUNC* AddLofiByteStereoPtr             = NULL;
ADDFUNC* AddLofiBytesMonoPtr              = NULL;
ADDFUNC* AddLofiBytesStereoPtr            = NULL;
ADDFUNC* AddLofiWordMonoPtr               = NULL;
ADDFUNC* AddLofiWordStereoPtr             = NULL;
ADDFUNC* AddLofiWordsMonoPtr              = NULL;
ADDFUNC* AddLofiWordsStereoPtr            = NULL;
ADDFUNC* AddLofiLongMonoPtr               = NULL;
ADDFUNC* AddLofiLongStereoPtr             = NULL;
ADDFUNC* AddLofiLongsMonoPtr              = NULL;
ADDFUNC* AddLofiLongsStereoPtr            = NULL;
ADDFUNC* AddLofiByteMonoBPtr              = NULL;
ADDFUNC* AddLofiByteStereoBPtr            = NULL;
ADDFUNC* AddLofiBytesMonoBPtr             = NULL;
ADDFUNC* AddLofiBytesStereoBPtr           = NULL;
ADDFUNC* AddLofiWordMonoBPtr              = NULL;
ADDFUNC* AddLofiWordStereoBPtr            = NULL;
ADDFUNC* AddLofiWordsMonoBPtr             = NULL;
ADDFUNC* AddLofiWordsStereoBPtr           = NULL;
ADDFUNC* AddLofiLongMonoBPtr              = NULL;
ADDFUNC* AddLofiLongStereoBPtr            = NULL;
ADDFUNC* AddLofiLongsMonoBPtr             = NULL;
ADDFUNC* AddLofiLongsStereoBPtr           = NULL;

const ULONG  DriverVersion  = 2;
const ULONG  Version        = VERSION;
const ULONG  Revision       = REVISION;

const ULONG	 __LIB_Version  = VERSION;
const ULONG	 __LIB_Revision = REVISION;

const char DevName[]   = AHINAME;
const char IDString[]  = AHINAME " " VERS "\r\n";

static const char VersTag[] =
 "$VER: " AHINAME " " VERS " ©1994-2003 Martin Blom. "
 CPU 
 " version.\r\n";

enum MixBackend_t          MixBackend     = MB_NATIVE;

/* linker can use symbol b for symbol a if a is not defined */
#define ALIAS(a,b) asm(".stabs \"_" #a "\",11,0,0,0\n.stabs \"_" #b "\",1,0,0,0")

ALIAS( __UtilityBase, UtilityBase );

/******************************************************************************
** Device code ****************************************************************
******************************************************************************/

struct AHIBase*
initRoutine( struct AHIBase*  device,
             APTR             seglist,
             struct ExecBase* sysbase )
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
  AHIBase->ahib_SegList = (BPTR) seglist;

#if defined( __mc68000__ )
  // Make sure we're running on a M68020 or better
  
  if( ( SysBase->AttnFlags & AFF_68020 ) == 0 )
  {
    Alert( ( AN_Unknown | ACPU_InstErr ) & (~AT_DeadEnd) );

    FreeMem( (APTR) ( ( (char*) device ) - device->ahib_Library.lib_NegSize ),
             device->ahib_Library.lib_NegSize + device->ahib_Library.lib_PosSize );
    return NULL;
  }
#endif

  InitSemaphore( &AHIBase->ahib_Lock );

  if( !OpenLibs() )
  {
    CloseLibs();
    FreeMem( (APTR) ( ( (char*) device ) - device->ahib_Library.lib_NegSize ),
             device->ahib_Library.lib_NegSize + device->ahib_Library.lib_PosSize );
    return NULL;
  }

  return AHIBase;
}


BPTR
DevExpunge( struct AHIBase* device )
{
  BPTR seglist = 0;

  if( device->ahib_Library.lib_OpenCnt == 0)
  {
    seglist = device->ahib_SegList;

    Remove( (struct Node *) device );

    CloseLibs();

    FreeMem( (APTR) ( ( (char*) device ) - device->ahib_Library.lib_NegSize ),
             device->ahib_Library.lib_NegSize + device->ahib_Library.lib_PosSize );
  }
  else
  {
    device->ahib_Library.lib_Flags |= LIBF_DELEXP;
  }

  return seglist;
}

static const APTR funcTable[] =
{

#if defined( __morphos__ ) || defined( __MORPHOS__ ) || defined( __amithlon__ )
  (APTR) FUNCARRAY_32BIT_NATIVE,
#endif

  &AROS_SLIB_ENTRY( gw_DevOpen, Ahi ),
  &AROS_SLIB_ENTRY( gw_DevClose, Ahi ),
  &AROS_SLIB_ENTRY( gw_DevExpunge, Ahi ),
  &AROS_SLIB_ENTRY( gw_Null, Ahi ),

  &AROS_SLIB_ENTRY( gw_DevBeginIO, Ahi ),
  &AROS_SLIB_ENTRY( gw_DevAbortIO, Ahi ),

  &AROS_SLIB_ENTRY( gw_AllocAudioA, Ahi ),
  &AROS_SLIB_ENTRY( gw_FreeAudio, Ahi ),
  &AROS_SLIB_ENTRY( gw_KillAudio, Ahi ),
  &AROS_SLIB_ENTRY( gw_ControlAudioA, Ahi ),
  &AROS_SLIB_ENTRY( gw_SetVol, Ahi ),
  &AROS_SLIB_ENTRY( gw_SetFreq, Ahi ),
  &AROS_SLIB_ENTRY( gw_SetSound, Ahi ),
  &AROS_SLIB_ENTRY( gw_SetEffect, Ahi ),
  &AROS_SLIB_ENTRY( gw_LoadSound, Ahi ),
  &AROS_SLIB_ENTRY( gw_UnloadSound, Ahi ),
  &AROS_SLIB_ENTRY( gw_NextAudioID, Ahi ),
  &AROS_SLIB_ENTRY( gw_GetAudioAttrsA, Ahi ),
  &AROS_SLIB_ENTRY( gw_BestAudioIDA, Ahi ),
  &AROS_SLIB_ENTRY( gw_AllocAudioRequestA, Ahi ),
  &AROS_SLIB_ENTRY( gw_AudioRequestA, Ahi ),
  &AROS_SLIB_ENTRY( gw_FreeAudioRequest, Ahi ),
  &AROS_SLIB_ENTRY( gw_PlayA, Ahi ),
  &AROS_SLIB_ENTRY( gw_SampleFrameSize, Ahi ),
  &AROS_SLIB_ENTRY( gw_AddAudioMode, Ahi ),
  &AROS_SLIB_ENTRY( gw_RemoveAudioMode, Ahi ),
  &AROS_SLIB_ENTRY( gw_LoadModeFile, Ahi ),
  (APTR) -1
};


static const APTR InitTable[4] =
{
  (APTR) sizeof( struct AHIBase ),
  (APTR) &funcTable,
  0,
  (APTR) AROS_ASMSYMNAME( gw_initRoutine)
};


static struct timerequest *TimerIO        = NULL;

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
  TimerIO->tr_node.io_Message.mn_Length = sizeof(struct timerequest);
  
  if( TimerIO == NULL)
  {
    Req( "Out of memory." );
    return FALSE;
  }

  if( OpenDevice( "timer.device",
                  UNIT_VBLANK,
                  (struct IORequest *)
                  TimerIO,
                  0) != 0 )
  {
    Req( "Unable to open 'timer.device'." );
    return FALSE; 
  }
  else
  {
    TimerBase = (struct Device *) TimerIO->tr_node.io_Device;
  }

  /* Utility Library */

  UtilityBase = (struct UtilityBase *) OpenLibrary( "utility.library", 37 );

  if( UtilityBase == NULL)
  {
    Req( "Unable to open 'utility.library'." );
    return FALSE;
  }

  // Fill in some defaults...

  AddByteMonoPtr         = AddByteMono;
  AddByteStereoPtr       = AddByteStereo;
  AddBytesMonoPtr        = AddBytesMono;
  AddBytesStereoPtr      = AddBytesStereo;
  AddWordMonoPtr         = AddWordMono;
  AddWordStereoPtr       = AddWordStereo;
  AddWordsMonoPtr        = AddWordsMono;
  AddWordsStereoPtr      = AddWordsStereo;
  AddLongMonoPtr         = AddLongMono;
  AddLongStereoPtr       = AddLongStereo;
  AddLongsMonoPtr        = AddLongsMono;
  AddLongsStereoPtr      = AddLongsStereo;
  AddByteMonoBPtr        = AddByteMonoB;
  AddByteStereoBPtr      = AddByteStereoB;
  AddBytesMonoBPtr       = AddBytesMonoB;
  AddBytesStereoBPtr     = AddBytesStereoB;
  AddWordMonoBPtr        = AddWordMonoB;
  AddWordStereoBPtr      = AddWordStereoB;
  AddWordsMonoBPtr       = AddWordsMonoB;
  AddWordsStereoBPtr     = AddWordsStereoB;
  AddLongMonoBPtr        = AddLongMonoB;
  AddLongStereoBPtr      = AddLongStereoB;
  AddLongsMonoBPtr       = AddLongsMonoB;
  AddLongsStereoBPtr     = AddLongsStereoB;

  AddLofiByteMonoPtr     = AddLofiByteMono;
  AddLofiByteStereoPtr   = AddLofiByteStereo;
  AddLofiBytesMonoPtr    = AddLofiBytesMono;
  AddLofiBytesStereoPtr  = AddLofiBytesStereo;
  AddLofiWordMonoPtr     = AddLofiWordMono;
  AddLofiWordStereoPtr   = AddLofiWordStereo;
  AddLofiWordsMonoPtr    = AddLofiWordsMono;
  AddLofiWordsStereoPtr  = AddLofiWordsStereo;
  AddLofiLongMonoPtr     = AddLofiLongMono;
  AddLofiLongStereoPtr   = AddLofiLongStereo;
  AddLofiLongsMonoPtr    = AddLofiLongsMono;
  AddLofiLongsStereoPtr  = AddLofiLongsStereo;
  AddLofiByteMonoBPtr    = AddLofiByteMonoB;
  AddLofiByteStereoBPtr  = AddLofiByteStereoB;
  AddLofiBytesMonoBPtr   = AddLofiBytesMonoB;
  AddLofiBytesStereoBPtr = AddLofiBytesStereoB;
  AddLofiWordMonoBPtr    = AddLofiWordMonoB;
  AddLofiWordStereoBPtr  = AddLofiWordStereoB;
  AddLofiWordsMonoBPtr   = AddLofiWordsMonoB;
  AddLofiWordsStereoBPtr = AddLofiWordsStereoB;
  AddLofiLongMonoBPtr    = AddLofiLongMonoB;
  AddLofiLongStereoBPtr  = AddLofiLongStereoB;
  AddLofiLongsMonoBPtr   = AddLofiLongsMonoB;
  AddLofiLongsStereoBPtr = AddLofiLongsStereoB;

  /* MorphOS/PowerUp/WarpOS loading

     Strategy:

      1) If MorphOS is running, use it.
      2) If PowerUp is running, but not WarpUp, use the m68k core
      3) If neither of them are running, try WarpUp.

  */

  // Check if MorpOS/PowerUp/WarpUp is running.
  {
#if defined( ENABLE_WARPUP )
    struct Library* ppclib     = NULL;
    struct Library* powerpclib = NULL;
#endif

    Forbid();
    MorphOSRes  = FindResident( "MorphOS" );
    
#if defined( ENABLE_WARPUP )
    powerpclib = (struct Library *) FindName( &SysBase->LibList,
                                              "powerpc.library" );
    ppclib     = (struct Library *) FindName( &SysBase->LibList,
                                              "ppc.library" );
#endif

    Permit();

#if defined( ENABLE_WARPUP )
    if( MorphOSRes == NULL && ! ( ppclib != NULL && powerpclib == NULL ) )
    {
      // Open WarpUp (but not if MorphOS or PowerUp is active)

      PowerPCBase = OpenLibrary( "powerpc.library", 15 );
    }
#endif
  }

  if( MorphOSRes != NULL )
  {
    MixBackend  = MB_NATIVE;
  }

#if defined( ENABLE_WARPUP )

  else if( PowerPCBase != NULL )
  {
    MixBackend = MB_WARPUP;

    /* Load our code to PPC..  */

    PPCObject = AHILoadObject( "DEVS:ahi.device.elf" );

    if( PPCObject != NULL )
    {
      ULONG* version = NULL;
      ULONG* revision = NULL;

      int r = ~0;

      AHIGetELFSymbol( "__LIB_Version", (void*) &version );
      AHIGetELFSymbol( "__LIB_Revision", (void*) &revision );
    
      if( version != NULL && revision != NULL )
      {
        if( *version == Version && *revision == Revision )
        {
          r &= GetSymbol( AddByteMono     );
          r &= GetSymbol( AddByteStereo   );
          r &= GetSymbol( AddBytesMono    );
          r &= GetSymbol( AddBytesStereo  );
          r &= GetSymbol( AddWordMono     );
          r &= GetSymbol( AddWordStereo   );
          r &= GetSymbol( AddWordsMono    );
          r &= GetSymbol( AddWordsStereo  );
          r &= GetSymbol( AddLongMono     );
          r &= GetSymbol( AddLongStereo   );
          r &= GetSymbol( AddLongsMono    );
          r &= GetSymbol( AddLongsStereo  );
          r &= GetSymbol( AddByteMonoB    );
          r &= GetSymbol( AddByteStereoB  );
          r &= GetSymbol( AddBytesMonoB   );
          r &= GetSymbol( AddBytesStereoB );
          r &= GetSymbol( AddWordMonoB    );
          r &= GetSymbol( AddWordStereoB  );
          r &= GetSymbol( AddWordsMonoB   );
          r &= GetSymbol( AddWordsStereoB );
          r &= GetSymbol( AddLongMonoB    );
          r &= GetSymbol( AddLongStereoB  );
          r &= GetSymbol( AddLongsMonoB   );
          r &= GetSymbol( AddLongsStereoB );

          r &= GetSymbol( AddLofiByteMono     );
          r &= GetSymbol( AddLofiByteStereo   );
          r &= GetSymbol( AddLofiBytesMono    );
          r &= GetSymbol( AddLofiBytesStereo  );
          r &= GetSymbol( AddLofiWordMono     );
          r &= GetSymbol( AddLofiWordStereo   );
          r &= GetSymbol( AddLofiWordsMono    );
          r &= GetSymbol( AddLofiWordsStereo  );
          r &= GetSymbol( AddLofiLongMono     );
          r &= GetSymbol( AddLofiLongStereo   );
          r &= GetSymbol( AddLofiLongsMono    );
          r &= GetSymbol( AddLofiLongsStereo  );
          r &= GetSymbol( AddLofiByteMonoB    );
          r &= GetSymbol( AddLofiByteStereoB  );
          r &= GetSymbol( AddLofiBytesMonoB   );
          r &= GetSymbol( AddLofiBytesStereoB );
          r &= GetSymbol( AddLofiWordMonoB    );
          r &= GetSymbol( AddLofiWordStereoB  );
          r &= GetSymbol( AddLofiWordsMonoB   );
          r &= GetSymbol( AddLofiWordsStereoB );
          r &= GetSymbol( AddLofiLongMonoB    );
          r &= GetSymbol( AddLofiLongStereoB  );
          r &= GetSymbol( AddLofiLongsMonoB   );
          r &= GetSymbol( AddLofiLongsStereoB );

          if( r != 0 )
          {
            char buffer[ 2 ] = "0";
        
            GetVar( "PowerPC/UseDisable", buffer, sizeof buffer, 0 );
        
            if( buffer[ 0 ] == '1' )
            {
              // OK, then...
            }
            else
            {
              Req( "The WarpUp variable 'PowerPC/UseDisable' must be '1'." );

              AHIUnloadObject( PPCObject );
              PPCObject  = NULL;
              MixBackend = MB_NATIVE;
            }
          }
          else
          {
            Req( "Unable to fetch all symbols from ELF object." );

            AHIUnloadObject( PPCObject );
            PPCObject  = NULL;
            MixBackend = MB_NATIVE;
          }
        }
        else
        {
          Req( "'ahi.device.elf' version %ld.%ld doesn't match "
               "'ahi.device' version %ld.%ld.",
               *version, *revision, Version, Revision );

          AHIUnloadObject( PPCObject );
          PPCObject  = NULL;
          MixBackend = MB_NATIVE;
        }
      }
      else
      {
        Req( "Unable to fetch version information from 'ahi.device.elf'." );

        AHIUnloadObject( PPCObject );
        PPCObject  = NULL;
        MixBackend = MB_NATIVE;
      }
    }
    else
    {
      MixBackend = MB_NATIVE;
    }
  }

#endif

  else 
  {
    MixBackend = MB_NATIVE;
  }

  OpenahiCatalog(NULL, NULL);

#if defined( __amithlon__ )
  Req( "This is a *beta* release of AHI/x86,\n"
       "using the generic 'C' mixing routines.\n"
       "\n"
       "Detailed bug reports and patches are welcome.\n"
	 "/Martin Blom <martin@blom.org>\n" );
#endif

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

#if defined( ENABLE_WARPUP )
  if( PPCObject != NULL )
  {
    AHIUnloadObject( PPCObject );
  }

  CloseLibrary( PowerPCBase );
#endif

  CloseLibrary( (struct Library *) UtilityBase );
  if( TimerIO  != NULL )
  {
    CloseDevice( (struct IORequest *) TimerIO );
  }
  FreeVec( TimerIO );
  CloseLibrary( (struct Library *) LocaleBase );
  CloseLibrary( (struct Library *) IntuitionBase );
  CloseLibrary( IFFParseBase );
  CloseLibrary( GadToolsBase );
  CloseLibrary( (struct Library *) GfxBase );
  CloseLibrary( (struct Library *) DOSBase );
}
