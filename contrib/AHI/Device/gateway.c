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

#include <exec/types.h> 

#include <clib/alib_protos.h>


#if defined( __morphos__ ) || defined( __MORPHOS__ )
# define EMUL_NOQUICKMODE
# include <emul/emulregs.h>
#endif

#include <proto/exec.h>

#include "ahi_def.h"

#include "audioctrl.h"
#include "database.h"
#include "devcommands.h"
#include "device.h"
#include "header.h"
#include "misc.h"
#include "mixer.h"
#include "modeinfo.h"
#include "requester.h"
#include "sound.h"


/*
 * All these functions are supposed to be called by the m68k "processor",
 * with the arguments in m68k registers d0-d7/a0-a6.
 * The functions relay each call to a standard C function.
 *
 */

#if defined( __morphos__ ) || defined( __MORPHOS__ )

/******************************************************************************
** MorphOS gateway functions **************************************************
******************************************************************************/

/* gw_initRoutine ************************************************************/

struct AHIBase*
gw_initRoutine( struct AHIBase*  device,
                APTR             seglist,
                struct ExecBase* sysbase )
{
  return initRoutine( device, seglist, sysbase );
}


/* gw_DevExpunge *************************************************************/

BPTR 
gw_DevExpunge( void )
{
  struct AHIBase* device = (struct AHIBase*) REG_A6;

  return DevExpunge( device );
}


/* gw_DevOpen ****************************************************************/

ULONG 
gw_DevOpen( void )
{
  ULONG              unit    = (ULONG)              REG_D0;
  ULONG              flags   = (ULONG)              REG_D1;
  struct AHIRequest* ioreq   = (struct AHIRequest*) REG_A1;
  struct AHIBase*    AHIBase = (struct AHIBase*)    REG_A6;

  return DevOpen( unit, flags, ioreq, AHIBase );
}


/* gw_DevClose ***************************************************************/

BPTR
gw_DevClose( void )
{
  struct AHIRequest* ioreq   = (struct AHIRequest*)  REG_A1;
  struct AHIBase*    AHIBase = (struct AHIBase*)     REG_A6;

  return DevClose( ioreq, AHIBase );
}

/* gw_Null *******************************************************************/

ULONG
gw_Null( void )
{
  return 0;
}


/* gw_DevBeginIO *************************************************************/

void 
gw_DevBeginIO( void )
{
  struct AHIRequest* ioreq   = (struct AHIRequest*) REG_A1;
  struct AHIBase*    AHIBase = (struct AHIBase*)    REG_A6;

  DevBeginIO( ioreq, AHIBase );
}


/* gw_DevAbortIO *************************************************************/

ULONG 
gw_DevAbortIO( void )
{
  struct AHIRequest* ioreq   = (struct AHIRequest*) REG_A1;
  struct AHIBase*    AHIBase = (struct AHIBase*)    REG_A6;

  return DevAbortIO( ioreq, AHIBase );
}


/* gw_AllocAudioA ************************************************************/

struct AHIAudioCtrl* 
gw_AllocAudioA( void )
{
  struct TagItem* tags    = (struct TagItem*) REG_A1;
  struct AHIBase* AHIBase = (struct AHIBase*) REG_A6;

  return AllocAudioA( tags, AHIBase );
}


/* gw_FreeAudio **************************************************************/

ULONG 
gw_FreeAudio( void )
{
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) REG_A2;
  struct AHIBase*          AHIBase   = (struct AHIBase*)          REG_A6;

  return FreeAudio( audioctrl, AHIBase );
}


/* gw_KillAudio **************************************************************/

ULONG 
gw_KillAudio( void )
{
  struct AHIBase* AHIBase = (struct AHIBase*) REG_A6;

  return KillAudio( AHIBase );
}


/* gw_ControlAudioA **********************************************************/

ULONG 
gw_ControlAudioA( void )
{
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) REG_A2;
  struct TagItem*          tags      = (struct TagItem*)          REG_A1;
  struct AHIBase*          AHIBase   = (struct AHIBase*)          REG_A6;

  return ControlAudioA( audioctrl, tags, AHIBase );
}


/* gw_SetVol *****************************************************************/

ULONG 
gw_SetVol( void )
{
  UWORD                    channel   = (UWORD)                    REG_D0;
  Fixed                    volume    = (Fixed)                    REG_D1;
  sposition                pan       = (sposition)                REG_D2;
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) REG_A2;
  ULONG                    flags     = (ULONG)                    REG_D3;
  struct AHIBase*          AHIBase   = (struct AHIBase*)          REG_A6;

  return SetVol( channel, volume, pan, audioctrl, flags, AHIBase );
}


/* gw_SetFreq ****************************************************************/

ULONG 
gw_SetFreq( void )
{
  UWORD                    channel   = (UWORD)                    REG_D0;
  ULONG                    freq      = (ULONG)                    REG_D1;
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) REG_A2;
  ULONG                    flags     = (ULONG)                    REG_D2;
  struct AHIBase*          AHIBase   = (struct AHIBase*)          REG_A6;

  return SetFreq( channel, freq, audioctrl, flags, AHIBase );
}


/* gw_SetSound ***************************************************************/

ULONG 
gw_SetSound( void )
{
  UWORD                    channel   = (UWORD)                    REG_D0;
  UWORD                    sound     = (UWORD)                    REG_D1;
  ULONG                    offset    = (ULONG)                    REG_D2;
  LONG                     length    = (LONG)                     REG_D3;
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) REG_A2;
  ULONG                    flags     = (ULONG)                    REG_D4;
  struct AHIBase*          AHIBase   = (struct AHIBase*)          REG_A6;

  return SetSound( channel, sound, offset, length, audioctrl, flags, AHIBase );
}


/* gw_SetEffect **************************************************************/

ULONG 
gw_SetEffect( void )
{
  ULONG*                   effect    = (ULONG*)                   REG_A0;
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) REG_A2;
  struct AHIBase*          AHIBase   = (struct AHIBase*)          REG_A6;

  return SetEffect( effect, audioctrl, AHIBase );
}


/* gw_LoadSound **************************************************************/

ULONG 
gw_LoadSound( void )
{
  UWORD                    sound     = (UWORD)                    REG_D0;
  ULONG                    type      = (ULONG)                    REG_D1;
  APTR                     info      = (APTR)                     REG_A0;
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) REG_A2;
  struct AHIBase*          AHIBase   = (struct AHIBase*)          REG_A6;

  return LoadSound( sound, type, info, audioctrl, AHIBase );
}


/* gw_UnloadSound ************************************************************/

ULONG 
gw_UnloadSound( void )
{
  UWORD                    sound     = (UWORD)                    REG_D0;
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) REG_A2;
  struct AHIBase*          AHIBase   = (struct AHIBase*)          REG_A6;

  return UnloadSound( sound, audioctrl, AHIBase );
}


/* gw_PlayA ******************************************************************/

ULONG  
gw_PlayA( void )
{
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) REG_A2;
  struct TagItem*          tags      = (struct TagItem*)          REG_A1;
  struct AHIBase*          AHIBase   = (struct AHIBase*)          REG_A6;

  return PlayA( audioctrl, tags, AHIBase );
}


/* gw_SampleFrameSize ********************************************************/

ULONG 
gw_SampleFrameSize( void )
{
  ULONG           sampletype = (ULONG)           REG_D0;
  struct AHIBase* AHIBase    = (struct AHIBase*) REG_A6;

  return SampleFrameSize( sampletype, AHIBase );
}


/* gw_GetAudioAttrsA *********************************************************/

ULONG  
gw_GetAudioAttrsA( void )
{
  ULONG                    id      = (ULONG)                    REG_D0;
  struct AHIPrivAudioCtrl* actrl   = (struct AHIPrivAudioCtrl*) REG_A2;
  struct TagItem*          tags    = (struct TagItem*)          REG_A1;
  struct AHIBase*          AHIBase = (struct AHIBase*)          REG_A6;

  return GetAudioAttrsA( id, actrl, tags, AHIBase );
}


/* gw_BestAudioIDA ***********************************************************/

ULONG  
gw_BestAudioIDA( void )
{
  struct TagItem* tags    = (struct TagItem*) REG_A1;
  struct AHIBase* AHIBase = (struct AHIBase*) REG_A6;

  return BestAudioIDA( tags, AHIBase );
}


/* gw_AllocAudioRequestA *****************************************************/

struct AHIAudioModeRequester* 
gw_AllocAudioRequestA( void )
{
  struct TagItem* tags    = (struct TagItem*) REG_A0;
  struct AHIBase* AHIBase = (struct AHIBase*) REG_A6;

  return AllocAudioRequestA( tags, AHIBase );
}


/* gw_AudioRequestA **********************************************************/

ULONG  
gw_AudioRequestA( void )
{
  struct AHIAudioModeRequester* req_in  = (struct AHIAudioModeRequester*) REG_A0;
  struct TagItem*               tags    = (struct TagItem*)               REG_A1;
  struct AHIBase*               AHIBase = (struct AHIBase*)               REG_A6;

  return AudioRequestA( req_in, tags, AHIBase );
}


/* gw_FreeAudioRequest *******************************************************/

void  
gw_FreeAudioRequest( void )
{
  struct AHIAudioModeRequester* req     = (struct AHIAudioModeRequester*) REG_A0;
  struct AHIBase*               AHIBase = (struct AHIBase*)               REG_A6;

  FreeAudioRequest( req, AHIBase );
}


/* gw_NextAudioID ************************************************************/

ULONG 
gw_NextAudioID( void )
{
  ULONG           id      = (ULONG)           REG_D0;
  struct AHIBase* AHIBase = (struct AHIBase*) REG_A6;

  return NextAudioID( id, AHIBase );
}


/* gw_AddAudioMode ***********************************************************/

ULONG 
gw_AddAudioMode( void )
{
  struct TagItem* DBtags  = (struct TagItem*) REG_A0;
  struct AHIBase* AHIBase = (struct AHIBase*) REG_A6;

  return AddAudioMode( DBtags, AHIBase );
}


/* gw_RemoveAudioMode ********************************************************/

ULONG 
gw_RemoveAudioMode( void )
{
  ULONG           id      = (ULONG)           REG_D0;
  struct AHIBase* AHIBase = (struct AHIBase*) REG_A6;

  return RemoveAudioMode( id, AHIBase );
}


/* gw_LoadModeFile ***********************************************************/

ULONG 
gw_LoadModeFile( void )
{
  UBYTE*          name    = (UBYTE*)          REG_A0;
  struct AHIBase* AHIBase = (struct AHIBase*) REG_A6;

  return LoadModeFile( name, AHIBase );
}


/* m68k_IndexToFrequency *****************************************************/

static LONG
gw_IndexToFrequency( void )
{
  struct Gadget* gad   = (struct Gadget*) REG_A1;
  WORD           level = (WORD)           REG_D0;

  return IndexToFrequency( gad , level );
}

struct EmulLibEntry m68k_IndexToFrequency =
{
  TRAP_LIB, 0, (void (*)(void)) gw_IndexToFrequency
};


/* m68k_DevProc **************************************************************/

struct EmulLibEntry m68k_DevProc =
{
  TRAP_LIB, 0, (void (*)(void)) DevProc
};


/* HookEntry *****************************************************************/

/* Should be in libamiga, but isn't? */

static ULONG
gw_HookEntry( void )
{
  struct Hook* h   = (struct Hook*) REG_A0;
  void*        o   = (void*)        REG_A2; 
  void*        msg = (void*)        REG_A1;

  return ( ( (ULONG(*)(struct Hook*, void*, void*)) *h->h_SubEntry)( h, o, msg ) );
}

struct EmulLibEntry _HookEntry =
{
  TRAP_LIB, 0, (void (*)(void)) &gw_HookEntry
};

__asm( ".globl HookEntry;HookEntry=_HookEntry" );

/* m68k_PreTimer  ************************************************************/

static BOOL
gw_PreTimer( void )
{
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) REG_A2;

  return PreTimer( audioctrl );
}

static struct EmulLibEntry m68k_PreTimer =
{
  TRAP_LIB, 0, (void (*)(void)) &gw_PreTimer
};


/* m68k_PostTimer  ***********************************************************/

static void
gw_PostTimer( void )
{
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) REG_A2;

  PostTimer( audioctrl );
}

static struct EmulLibEntry m68k_PostTimer =
{
  TRAP_LIBNR, 0, (void (*)(void)) &gw_PostTimer
};

#elif defined( __amithlon__ )

/******************************************************************************
** Amithlon gateway functions *************************************************
******************************************************************************/

// A handy macro for fetching a little endian long integer from memory
#define GET_LONG(a) ({ long r; __asm__("movl %1,%0":"=r"(r) :"m"(a)); r; })

/* gw_initRoutine ************************************************************/

struct AHIBase*
gw_initRoutine( struct AHIBase*  device,
                APTR             seglist,
                struct ExecBase* sysbase )
{
  return initRoutine( device, seglist, sysbase );
}


/* gw_DevExpunge *************************************************************/

BPTR 
gw_DevExpunge( struct _Regs* regs ) __attribute__((regparm(3)));

BPTR 
gw_DevExpunge( struct _Regs* regs )
{
  struct AHIBase* device = (struct AHIBase*) GET_LONG( regs->a6 );

  return DevExpunge( device );
}


/* gw_DevOpen ****************************************************************/


ULONG 
gw_DevOpen( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_DevOpen( struct _Regs* regs )
{
  ULONG              unit    = (ULONG)              GET_LONG( regs->d0 );
  ULONG              flags   = (ULONG)              GET_LONG( regs->d1 );
  struct AHIRequest* ioreq   = (struct AHIRequest*) GET_LONG( regs->a1 );
  struct AHIBase*    AHIBase = (struct AHIBase*)    GET_LONG( regs->a6 );

  return DevOpen( unit, flags, ioreq, AHIBase );
}


/* gw_DevClose ***************************************************************/

BPTR
gw_DevClose( struct _Regs* regs ) __attribute__((regparm(3)));

BPTR
gw_DevClose( struct _Regs* regs )
{
  struct AHIRequest* ioreq   = (struct AHIRequest*)  GET_LONG( regs->a1 );
  struct AHIBase*    AHIBase = (struct AHIBase*)     GET_LONG( regs->a6 );

  return DevClose( ioreq, AHIBase );
}


/* gw_Null *******************************************************************/

ULONG
gw_NULL( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG
gw_NULL( struct _Regs* regs )
{
  return 0;
}


/* gw_DevBeginIO *************************************************************/

void 
gw_DevBeginIO( struct _Regs* regs ) __attribute__((regparm(3)));

void 
gw_DevBeginIO( struct _Regs* regs )
{
  struct AHIRequest* ioreq   = (struct AHIRequest*) GET_LONG( regs->a1 );
  struct AHIBase*    AHIBase = (struct AHIBase*)    GET_LONG( regs->a6 );

  DevBeginIO( ioreq, AHIBase );
}


/* gw_DevAbortIO *************************************************************/

ULONG 
gw_DevAbortIO( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_DevAbortIO( struct _Regs* regs )
{
  struct AHIRequest* ioreq   = (struct AHIRequest*) GET_LONG( regs->a1 );
  struct AHIBase*    AHIBase = (struct AHIBase*)    GET_LONG( regs->a6 );

  return DevAbortIO( ioreq, AHIBase );
}


/* gw_AllocAudioA ************************************************************/

struct AHIAudioCtrl* 
gw_AllocAudioA( struct _Regs* regs ) __attribute__((regparm(3)));

struct AHIAudioCtrl* 
gw_AllocAudioA( struct _Regs* regs )
{
  struct TagItem* tags    = (struct TagItem*) GET_LONG( regs->a1 );
  struct AHIBase* AHIBase = (struct AHIBase*) GET_LONG( regs->a6 );

  return (struct AHIAudioCtrl*)  AllocAudioA( tags, AHIBase );
}


/* gw_FreeAudio **************************************************************/

ULONG 
gw_FreeAudio( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_FreeAudio( struct _Regs* regs )
{
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) GET_LONG( regs->a2 );
  struct AHIBase*          AHIBase   = (struct AHIBase*)          GET_LONG( regs->a6 );

  return FreeAudio( audioctrl, AHIBase );
}


/* gw_KillAudio **************************************************************/

ULONG 
gw_KillAudio( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_KillAudio( struct _Regs* regs )
{
  struct AHIBase* AHIBase = (struct AHIBase*) GET_LONG( regs->a6 );

  return KillAudio( AHIBase );
}


/* gw_ControlAudioA **********************************************************/

ULONG 
gw_ControlAudioA( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_ControlAudioA( struct _Regs* regs )
{
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) GET_LONG( regs->a2 );
  struct TagItem*          tags      = (struct TagItem*)          GET_LONG( regs->a1 );
  struct AHIBase*          AHIBase   = (struct AHIBase*)          GET_LONG( regs->a6 );

  return ControlAudioA( audioctrl, tags, AHIBase );
}


/* gw_SetVol *****************************************************************/

ULONG 
gw_SetVol( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_SetVol( struct _Regs* regs )
{
  UWORD                    channel   = (UWORD)                    GET_LONG( regs->d0 );
  Fixed                    volume    = (Fixed)                    GET_LONG( regs->d1 );
  sposition                pan       = (sposition)                GET_LONG( regs->d2 );
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) GET_LONG( regs->a2 );
  ULONG                    flags     = (ULONG)                    GET_LONG( regs->d3 );
  struct AHIBase*          AHIBase   = (struct AHIBase*)          GET_LONG( regs->a6 );

  return SetVol( channel, volume, pan, audioctrl, flags, AHIBase );
}


/* gw_SetFreq ****************************************************************/

ULONG 
gw_SetFreq( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_SetFreq( struct _Regs* regs )
{
  UWORD                    channel   = (UWORD)                    GET_LONG( regs->d0 );
  ULONG                    freq      = (ULONG)                    GET_LONG( regs->d1 );
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) GET_LONG( regs->a2 );
  ULONG                    flags     = (ULONG)                    GET_LONG( regs->d2 );
  struct AHIBase*          AHIBase   = (struct AHIBase*)          GET_LONG( regs->a6 );

  return SetFreq( channel, freq, audioctrl, flags, AHIBase );
}


/* gw_SetSound ***************************************************************/

ULONG 
gw_SetSound( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_SetSound( struct _Regs* regs )
{
  UWORD                    channel   = (UWORD)                    GET_LONG( regs->d0 );
  UWORD                    sound     = (UWORD)                    GET_LONG( regs->d1 );
  ULONG                    offset    = (ULONG)                    GET_LONG( regs->d2 );
  LONG                     length    = (LONG)                     GET_LONG( regs->d3 );
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) GET_LONG( regs->a2 );
  ULONG                    flags     = (ULONG)                    GET_LONG( regs->d4 );
  struct AHIBase*          AHIBase   = (struct AHIBase*)          GET_LONG( regs->a6 );

  return SetSound( channel, sound, offset, length, audioctrl, flags, AHIBase );
}


/* gw_SetEffect **************************************************************/

ULONG 
gw_SetEffect( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_SetEffect( struct _Regs* regs )
{
  ULONG*                   effect    = (ULONG*)                   GET_LONG( regs->a0 );
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) GET_LONG( regs->a2 );
  struct AHIBase*          AHIBase   = (struct AHIBase*)          GET_LONG( regs->a6 );

  return SetEffect( effect, audioctrl, AHIBase );
}


/* gw_LoadSound **************************************************************/

ULONG 
gw_LoadSound( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_LoadSound( struct _Regs* regs )
{
  UWORD                    sound     = (UWORD)                    GET_LONG( regs->d0 );
  ULONG                    type      = (ULONG)                    GET_LONG( regs->d1 );
  APTR                     info      = (APTR)                     GET_LONG( regs->a0 );
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) GET_LONG( regs->a2 );
  struct AHIBase*          AHIBase   = (struct AHIBase*)          GET_LONG( regs->a6 );

  return LoadSound( sound, type, info, audioctrl, AHIBase );
}


/* gw_UnloadSound ************************************************************/

ULONG 
gw_UnloadSound( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_UnloadSound( struct _Regs* regs )
{
  UWORD                    sound     = (UWORD)                    GET_LONG( regs->d0 );
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) GET_LONG( regs->a2 );
  struct AHIBase*          AHIBase   = (struct AHIBase*)          GET_LONG( regs->a6 );

  return UnloadSound( sound, audioctrl, AHIBase );
}


/* gw_PlayA ******************************************************************/

ULONG  
gw_PlayA( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG  
gw_PlayA( struct _Regs* regs )
{
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) GET_LONG( regs->a2 );
  struct TagItem*          tags      = (struct TagItem*)          GET_LONG( regs->a1 );
  struct AHIBase*          AHIBase   = (struct AHIBase*)          GET_LONG( regs->a6 );

  return PlayA( audioctrl, tags, AHIBase );
}


/* gw_SampleFrameSize ********************************************************/

ULONG 
gw_SampleFrameSize( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_SampleFrameSize( struct _Regs* regs )
{
  ULONG           sampletype = (ULONG)           GET_LONG( regs->d0 );
  struct AHIBase* AHIBase    = (struct AHIBase*) GET_LONG( regs->a6 );

  return SampleFrameSize( sampletype, AHIBase );
}


/* gw_GetAudioAttrsA *********************************************************/

ULONG  
gw_GetAudioAttrsA( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG  
gw_GetAudioAttrsA( struct _Regs* regs )
{
  ULONG                    id      = (ULONG)                    GET_LONG( regs->d0 );
  struct AHIPrivAudioCtrl* actrl   = (struct AHIPrivAudioCtrl*) GET_LONG( regs->a2 );
  struct TagItem*          tags    = (struct TagItem*)          GET_LONG( regs->a1 );
  struct AHIBase*          AHIBase = (struct AHIBase*)          GET_LONG( regs->a6 );

  return GetAudioAttrsA( id, actrl, tags, AHIBase );
}


/* gw_BestAudioIDA ***********************************************************/

ULONG  
gw_BestAudioIDA( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG  
gw_BestAudioIDA( struct _Regs* regs )
{
  struct TagItem* tags    = (struct TagItem*) GET_LONG( regs->a1 );
  struct AHIBase* AHIBase = (struct AHIBase*) GET_LONG( regs->a6 );

  return BestAudioIDA( tags, AHIBase );
}


/* gw_AllocAudioRequestA *****************************************************/

struct AHIAudioModeRequester* 
gw_AllocAudioRequestA( struct _Regs* regs ) __attribute__((regparm(3)));

struct AHIAudioModeRequester* 
gw_AllocAudioRequestA( struct _Regs* regs )
{
  struct TagItem* tags    = (struct TagItem*) GET_LONG( regs->a0 );
  struct AHIBase* AHIBase = (struct AHIBase*) GET_LONG( regs->a6 );

  return (struct AHIAudioModeRequester*)  AllocAudioRequestA( tags, AHIBase );
}


/* gw_AudioRequestA **********************************************************/

ULONG  
gw_AudioRequestA( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG  
gw_AudioRequestA( struct _Regs* regs )
{
  struct AHIAudioModeRequester* req_in  = (struct AHIAudioModeRequester*) GET_LONG( regs->a0 );
  struct TagItem*               tags    = (struct TagItem*)               GET_LONG( regs->a1 );
  struct AHIBase*               AHIBase = (struct AHIBase*)               GET_LONG( regs->a6 );

  return AudioRequestA( req_in, tags, AHIBase );
}


/* gw_FreeAudioRequest *******************************************************/

void  
gw_FreeAudioRequest( struct _Regs* regs ) __attribute__((regparm(3)));

void  
gw_FreeAudioRequest( struct _Regs* regs )
{
  struct AHIAudioModeRequester* req     = (struct AHIAudioModeRequester*) GET_LONG( regs->a0 );
  struct AHIBase*               AHIBase = (struct AHIBase*)               GET_LONG( regs->a6 );

  FreeAudioRequest( req, AHIBase );
}


/* gw_NextAudioID ************************************************************/

ULONG 
gw_NextAudioID( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_NextAudioID( struct _Regs* regs )
{
  ULONG           id      = (ULONG)           GET_LONG( regs->d0 );
  struct AHIBase* AHIBase = (struct AHIBase*) GET_LONG( regs->a6 );

  return NextAudioID( id, AHIBase );
}


/* gw_AddAudioMode ***********************************************************/

ULONG 
gw_AddAudioMode( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_AddAudioMode( struct _Regs* regs )
{
  struct TagItem* DBtags  = (struct TagItem*) GET_LONG( regs->a0 );
  struct AHIBase* AHIBase = (struct AHIBase*) GET_LONG( regs->a6 );

  return AddAudioMode( DBtags, AHIBase );
}


/* gw_RemoveAudioMode ********************************************************/

ULONG 
gw_RemoveAudioMode( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_RemoveAudioMode( struct _Regs* regs )
{
  ULONG           id      = (ULONG)           GET_LONG( regs->d0 );
  struct AHIBase* AHIBase = (struct AHIBase*) GET_LONG( regs->a6 );

  return RemoveAudioMode( id, AHIBase );
}


/* gw_LoadModeFile ***********************************************************/

ULONG 
gw_LoadModeFile( struct _Regs* regs ) __attribute__((regparm(3)));

ULONG 
gw_LoadModeFile( struct _Regs* regs )
{
  UBYTE*          name    = (UBYTE*)          GET_LONG( regs->a0 );
  struct AHIBase* AHIBase = (struct AHIBase*) GET_LONG( regs->a6 );

  return LoadModeFile( name, AHIBase );
}


/* m68k_IndexToFrequency *****************************************************/

static LONG
gw_IndexToFrequency( struct Gadget *gad, WORD level ) __attribute__((regparm(3)));

static LONG
gw_IndexToFrequency( struct Gadget *gad, WORD level )
{
  return IndexToFrequency( gad, level );
}

struct
{
    UWORD movel_4sp_d0[2];
    UWORD movew_10sp_a0[2];
    UWORD movel_a0_d1;
    UWORD jmp;
    ULONG addr;
} m68k_IndexToFrequency =
{
  {0x202F,0x0004},
  {0x306F,0x000A},
  0x2208,
  0x4EF9, (ULONG) gw_IndexToFrequency + 1
};


/* m68k_DevProc **************************************************************/

struct
{
    UWORD nop;  // Just to 32-bit align the long word
    UWORD jmp;
    ULONG addr;
} m68k_DevProc =
{
  0x4E71,
  0x4EF9, (ULONG) DevProc + 1
};


/* m68k_PreTimer  ************************************************************/

static BOOL
gw_PreTimer( struct AHIPrivAudioCtrl* audioctrl ) __attribute__((regparm(3)));

static BOOL
gw_PreTimer( struct AHIPrivAudioCtrl* audioctrl )
{
  return 0; //PreTimer( audioctrl );
}

static struct
{
    UWORD nop;
    UWORD movel_4sp_d0[2];
    UWORD jmp;
    ULONG addr;
} m68k_PreTimer =
{
  0x4E71,
  {0x202F, 0x0004},
  0x4EF9, (ULONG) gw_PreTimer + 1
};


/* m68k_PostTimer  ***********************************************************/

static void
gw_PostTimer( struct _Regs* regs ) __attribute__((regparm(3)));

static void
gw_PostTimer( struct _Regs* regs )
{
  struct AHIPrivAudioCtrl* audioctrl = (struct AHIPrivAudioCtrl*) GET_LONG( regs->a2 );

  PostTimer( audioctrl );
}

static struct
{
    UWORD nop;
    UWORD movel_4sp_d0[2];
    UWORD jmp;
    ULONG addr;
} m68k_PostTimer =
{
  0x4E71,
  {0x202F, 0x0004},
  0x4EF9, (ULONG) gw_PostTimer + 1
};

#elif defined( __AROS__ )

/******************************************************************************
** AROS gateway functions *****************************************************
******************************************************************************/

#ifndef AROS_LIBCALL_H
#   include <aros/libcall.h>
#   include <aros/asmcall.h>
#endif

// Let the preprocessor re-arrange until I fix fd2inline and nuke
// this file forever ...

#undef REG
#define REG( reg, proto, name ) AROS_LHA( proto, name, reg )

#define d0 D0
#define d1 D1
#define a0 A0
#define a1 A1
#define a2 A2

/* gw_initRoutine ************************************************************/

AROS_UFH3( struct AHIBase*,
	  gw_initRoutine,
	  AROS_UFHA( struct AHIBase*,  device,  D0 ),
	  AROS_UFHA( APTR,             seglist, A0 ),
	  AROS_UFHA( struct ExecBase*, sysbase, A6 ) )
{
  AROS_USERFUNC_INIT

  return initRoutine( device, seglist, sysbase );
  
  AROS_USERFUNC_EXIT  
}


/* gw_DevExpunge *************************************************************/

AROS_LH0( BPTR,
	  gw_DevExpunge,
	  struct AHIBase*, device, 3, Ahi )
{
  AROS_LIBFUNC_INIT
  return DevExpunge( device );
  AROS_LIBFUNC_EXIT  
}

/* gw_DevOpen ****************************************************************/

AROS_LH3( ULONG,
	  gw_DevOpen,
	  AROS_LHA( struct AHIRequest*, ioreq, A1 ),
	  AROS_LHA( ULONG,              unit,  D0 ),
	  AROS_LHA( ULONG,              flags, D1 ),
	  struct AHIBase*, AHIBase, 1, Ahi )
{
  AROS_LIBFUNC_INIT
  return DevOpen( unit, flags, ioreq, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_DevClose ***************************************************************/

AROS_LH1( BPTR,
	  gw_DevClose,
	  AROS_LHA( struct AHIRequest*, ioreq, A1 ),
	  struct AHIBase*, AHIBase, 2, Ahi )
{
  AROS_LIBFUNC_INIT
  return DevClose( ioreq, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_Null *******************************************************************/

AROS_LH0( ULONG,
	  gw_Null,
	  struct AHIBase*, AHIBase, 4, Ahi )
{
  AROS_LIBFUNC_INIT
  return 0;
  AROS_LIBFUNC_EXIT  
}


/* gw_DevBeginIO *************************************************************/

AROS_LH1( void,
	  gw_DevBeginIO,
	  AROS_LHA( struct AHIRequest*, ioreq, A1 ),
	  struct AHIBase*, AHIBase, 5, Ahi )
{
  AROS_LIBFUNC_INIT
  DevBeginIO( ioreq, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_DevAbortIO *************************************************************/

AROS_LH1( ULONG,
	  gw_DevAbortIO,
	  AROS_LHA( struct AHIRequest*, ioreq, A1 ),
	  struct AHIBase*, AHIBase, 6, Ahi )
{
  AROS_LIBFUNC_INIT
  return DevAbortIO( ioreq, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_AllocAudioA ************************************************************/

AROS_LH1( struct AHIAudioCtrl*,
	  gw_AllocAudioA,
	  REG(a1, struct TagItem*, tags),
	  struct AHIBase*, AHIBase, 7, Ahi )
{
  AROS_LIBFUNC_INIT
  return AllocAudioA( tags, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_FreeAudio **************************************************************/

AROS_LH1( ULONG,
	  gw_FreeAudio,
	  REG(a2, struct AHIPrivAudioCtrl*, audioctrl),
	  struct AHIBase*, AHIBase, 8, Ahi )
{
  AROS_LIBFUNC_INIT
  return FreeAudio( audioctrl, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_KillAudio **************************************************************/

AROS_LH0( ULONG,
	  gw_KillAudio,
	  struct AHIBase*, AHIBase, 9, Ahi )
  
{
  AROS_LIBFUNC_INIT
  return KillAudio( AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_ControlAudioA **********************************************************/

AROS_LH2( ULONG,
	  gw_ControlAudioA,
	  REG(a2, struct AHIPrivAudioCtrl*, audioctrl),
	  REG(a1, struct TagItem*,          tags),
	  struct AHIBase*, AHIBase, 10, Ahi )
{
  AROS_LIBFUNC_INIT
  return ControlAudioA( audioctrl, tags, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_SetVol *****************************************************************/

AROS_LH5( ULONG,
	  gw_SetVol,
	  REG(d0, UWORD,                    channel),
	  REG(d1, Fixed,                    volume),
	  REG(d2, sposition,                pan),
	  REG(a2, struct AHIPrivAudioCtrl*, audioctrl),
	  REG(d3, ULONG,                    flags),
	  struct AHIBase*, AHIBase, 11, Ahi )
{
  AROS_LIBFUNC_INIT
  return SetVol( channel, volume, pan, audioctrl, flags, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_SetFreq ****************************************************************/

AROS_LH4( ULONG,
	  gw_SetFreq,
	  REG( d0, UWORD,                    channel ),
	  REG( d1, ULONG,                    freq ),
	  REG( a2, struct AHIPrivAudioCtrl*, audioctrl ),
	  REG( d2, ULONG,                    flags ),
	  struct AHIBase*, AHIBase, 12, Ahi )
{
  AROS_LIBFUNC_INIT
  return SetFreq( channel, freq, audioctrl, flags, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_SetSound ***************************************************************/

AROS_LH6( ULONG,
	  gw_SetSound,
	  REG(d0, UWORD,                    channel),
	  REG(d1, UWORD,                    sound),
	  REG(d2, ULONG,                    offset),
	  REG(d3, LONG,                     length),
	  REG(a2, struct AHIPrivAudioCtrl*, audioctrl),
	  REG(d4, ULONG,                    flags),
	  struct AHIBase*, AHIBase, 13, Ahi )
{
  AROS_LIBFUNC_INIT
  return SetSound( channel, sound, offset, length, audioctrl, flags, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_SetEffect **************************************************************/

AROS_LH2( ULONG,
	  gw_SetEffect,
	  REG(a0, ULONG*,                   effect),
	  REG(a2, struct AHIPrivAudioCtrl*, audioctrl),
	  struct AHIBase*, AHIBase, 14, Ahi )
{
  AROS_LIBFUNC_INIT
  return SetEffect( effect, audioctrl, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_LoadSound **************************************************************/

AROS_LH4( ULONG,
	  gw_LoadSound,
	  REG(d0, UWORD,                    sound),
	  REG(d1, ULONG,                    type),
	  REG(a0, APTR,                     info),
	  REG(a2, struct AHIPrivAudioCtrl*, audioctrl),
	  struct AHIBase*, AHIBase, 15, Ahi )
{
  AROS_LIBFUNC_INIT
  return LoadSound( sound, type, info, audioctrl, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_UnloadSound ************************************************************/

AROS_LH2( ULONG,
	  gw_UnloadSound,
	  REG(d0, UWORD,                    sound),
	  REG(a2, struct AHIPrivAudioCtrl*, audioctrl),
	  struct AHIBase*, AHIBase, 16, Ahi )
{
  AROS_LIBFUNC_INIT
  return UnloadSound( sound, audioctrl, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_PlayA ******************************************************************/

AROS_LH2( ULONG, 
	  gw_PlayA,
	  REG(a2, struct AHIPrivAudioCtrl*, audioctrl),
          REG(a1, struct TagItem*,          tags),
	  struct AHIBase*, AHIBase, 23, Ahi )
{
  AROS_LIBFUNC_INIT
  return PlayA( audioctrl, tags, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_SampleFrameSize ********************************************************/

AROS_LH1( ULONG,
	  gw_SampleFrameSize,
	  REG(d0, ULONG,           sampletype),
	  struct AHIBase*, AHIBase, 24, Ahi )
{
  AROS_LIBFUNC_INIT
  return SampleFrameSize( sampletype, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_GetAudioAttrsA *********************************************************/

AROS_LH3( ULONG, 
	  gw_GetAudioAttrsA,
	  REG(d0, ULONG,                    id),
	  REG(a2, struct AHIPrivAudioCtrl*, actrl),
	  REG(a1, struct TagItem*,          tags),
	  struct AHIBase*, AHIBase, 18, Ahi )
{
  AROS_LIBFUNC_INIT
  return GetAudioAttrsA( id, actrl, tags, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_BestAudioIDA ***********************************************************/

AROS_LH1( ULONG, 
	  gw_BestAudioIDA,
	  REG(a1, struct TagItem*, tags),
	  struct AHIBase*, AHIBase, 19, Ahi )
{
  AROS_LIBFUNC_INIT
  return BestAudioIDA( tags, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_AllocAudioRequestA *****************************************************/

AROS_LH1( struct AHIAudioModeRequester*,
	  gw_AllocAudioRequestA,
	  REG(a0, struct TagItem*, tags),
	  struct AHIBase*, AHIBase, 20, Ahi )
{
  AROS_LIBFUNC_INIT
  return AllocAudioRequestA( tags, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_AudioRequestA **********************************************************/

AROS_LH2( ULONG, 
	  gw_AudioRequestA,
	  REG(a0, struct AHIAudioModeRequester*, req_in),
	  REG(a1, struct TagItem*,               tags ),
	  struct AHIBase*, AHIBase, 21, Ahi )
{
  AROS_LIBFUNC_INIT
  return AudioRequestA( req_in, tags, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_FreeAudioRequest *******************************************************/

AROS_LH1( void, 
	  gw_FreeAudioRequest,
	  REG(a0, struct AHIAudioModeRequester*, req),
	  struct AHIBase*, AHIBase, 22, Ahi )
{
  AROS_LIBFUNC_INIT
  FreeAudioRequest( req, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_NextAudioID ************************************************************/

AROS_LH1( ULONG,
	  gw_NextAudioID,
	  REG(d0, ULONG,           id),
	  struct AHIBase*, AHIBase, 17, Ahi )
{
  AROS_LIBFUNC_INIT
  return NextAudioID( id, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_AddAudioMode ***********************************************************/

AROS_LH1( ULONG,
	  gw_AddAudioMode,
	  REG(a0, struct TagItem*, DBtags),
	  struct AHIBase*, AHIBase, 25, Ahi )
{
  AROS_LIBFUNC_INIT
  return AddAudioMode( DBtags, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_RemoveAudioMode ********************************************************/

AROS_LH1( ULONG,
	  gw_RemoveAudioMode,
	  REG(d0, ULONG,           id),
	  struct AHIBase*, AHIBase, 26, Ahi )
{
  AROS_LIBFUNC_INIT
  return RemoveAudioMode( id, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* gw_LoadModeFile ***********************************************************/

AROS_LH1( ULONG,
	  gw_LoadModeFile,
	  REG(a0, UBYTE*,          name),
	  struct AHIBase*, AHIBase, 27, Ahi )
{
  AROS_LIBFUNC_INIT
  return LoadModeFile( name, AHIBase );
  AROS_LIBFUNC_EXIT  
}


/* m68k_IndexToFrequency *****************************************************/

LONG
m68k_IndexToFrequency( struct Gadget *gad, WORD level )
{
  return IndexToFrequency( gad, level );
}


/* m68k_DevProc **************************************************************/

AROS_UFH0( void,
	   m68k_DevProc )
{
  DevProc();
}


/* m68k_PreTimer  ************************************************************/

AROS_UFH1( BOOL,
	   m68k_PreTimer,
	   AROS_UFHA( struct AHIPrivAudioCtrl*, audioctrl, A2 ) )
{
  AROS_USERFUNC_INIT
  return PreTimer( audioctrl );
  AROS_USERFUNC_EXIT  
}


/* m68k_PostTimer  ***********************************************************/

AROS_UFH1( void,
	   m68k_PostTimer,
	   AROS_UFHA( struct AHIPrivAudioCtrl*, audioctrl, A2 ) )
{
  AROS_USERFUNC_INIT
  PostTimer( audioctrl );
  AROS_USERFUNC_EXIT  
}

#else // Not MorphOS, not Amithlon, not AROS


/******************************************************************************
** AmigaOS gateway functions **************************************************
******************************************************************************/

/* gw_initRoutine ************************************************************/

struct AHIBase* ASMCALL
gw_initRoutine( REG( d0, struct AHIBase*  device ),
                REG( a0, APTR             seglist ),
                REG( a6, struct ExecBase* sysbase ) )
{
  return initRoutine( device, seglist, sysbase );
}


/* gw_DevExpunge *************************************************************/

BPTR ASMCALL
gw_DevExpunge( REG( a6, struct AHIBase* device ) )
{
  return DevExpunge( device );
}

/* gw_DevOpen ****************************************************************/

ULONG ASMCALL
gw_DevOpen( REG( d0, ULONG              unit ),
            REG( d1, ULONG              flags ),
            REG( a1, struct AHIRequest* ioreq ),
            REG( a6, struct AHIBase*    AHIBase ) )
{
  return DevOpen( unit, flags, ioreq, AHIBase );
}


/* gw_DevClose ***************************************************************/

BPTR ASMCALL
gw_DevClose( REG( a1, struct AHIRequest* ioreq ),
             REG( a6, struct AHIBase*    AHIBase ) )
{
  return DevClose( ioreq, AHIBase );
}


/* gw_Null *******************************************************************/

ULONG ASMCALL
gw_Null( REG( a6, struct AHIBase*    AHIBase ) )
{
  return 0;
}


/* gw_DevBeginIO *************************************************************/

void ASMCALL
gw_DevBeginIO( REG( a1, struct AHIRequest* ioreq ),
               REG( a6, struct AHIBase*    AHIBase ) )
{
  DevBeginIO( ioreq, AHIBase );
}


/* gw_DevAbortIO *************************************************************/

ULONG ASMCALL
gw_DevAbortIO( REG( a1, struct AHIRequest* ioreq ),
               REG( a6, struct AHIBase*    AHIBase ) )
{
  return DevAbortIO( ioreq, AHIBase );
}


/* gw_AllocAudioA ************************************************************/

struct AHIAudioCtrl* ASMCALL
gw_AllocAudioA( REG(a1, struct TagItem* tags),
                REG(a6, struct AHIBase* AHIBase) )
{
  return AllocAudioA( tags, AHIBase );
}


/* gw_FreeAudio **************************************************************/

ULONG ASMCALL
gw_FreeAudio( REG(a2, struct AHIPrivAudioCtrl* audioctrl),
              REG(a6, struct AHIBase*          AHIBase) )
{
  return FreeAudio( audioctrl, AHIBase );
}


/* gw_KillAudio **************************************************************/

ULONG ASMCALL
gw_KillAudio( REG(a6, struct AHIBase* AHIBase) )
{
  return KillAudio( AHIBase );
}


/* gw_ControlAudioA **********************************************************/

ULONG ASMCALL
gw_ControlAudioA( REG(a2, struct AHIPrivAudioCtrl* audioctrl),
                  REG(a1, struct TagItem*          tags),
                  REG(a6, struct AHIBase*          AHIBase) )
{
  return ControlAudioA( audioctrl, tags, AHIBase );
}


/* gw_SetVol *****************************************************************/

ULONG ASMCALL
gw_SetVol( REG(d0, UWORD                    channel),
           REG(d1, Fixed                    volume),
           REG(d2, sposition                pan),
           REG(a2, struct AHIPrivAudioCtrl* audioctrl),
           REG(d3, ULONG                    flags),
           REG(a6, struct AHIBase*          AHIBase) )
{
  return SetVol( channel, volume, pan, audioctrl, flags, AHIBase );
}


/* gw_SetFreq ****************************************************************/

ULONG ASMCALL
gw_SetFreq( REG( d0, UWORD                    channel ),
            REG( d1, ULONG                    freq ),
            REG( a2, struct AHIPrivAudioCtrl* audioctrl ),
            REG( d2, ULONG                    flags ),
            REG( a6, struct AHIBase*          AHIBase ) )
{
  return SetFreq( channel, freq, audioctrl, flags, AHIBase );
}


/* gw_SetSound ***************************************************************/

ULONG ASMCALL
gw_SetSound( REG(d0, UWORD                    channel),
             REG(d1, UWORD                    sound),
             REG(d2, ULONG                    offset),
             REG(d3, LONG                     length),
             REG(a2, struct AHIPrivAudioCtrl* audioctrl),
             REG(d4, ULONG                    flags),
             REG(a6, struct AHIBase*          AHIBase) )
{
  return SetSound( channel, sound, offset, length, audioctrl, flags, AHIBase );
}


/* gw_SetEffect **************************************************************/

ULONG ASMCALL
gw_SetEffect( REG(a0, ULONG*                   effect),
              REG(a2, struct AHIPrivAudioCtrl* audioctrl),
              REG(a6, struct AHIBase*          AHIBase) )
{
  return SetEffect( effect, audioctrl, AHIBase );
}


/* gw_LoadSound **************************************************************/

ULONG ASMCALL
gw_LoadSound( REG(d0, UWORD                    sound),
              REG(d1, ULONG                    type),
              REG(a0, APTR                     info),
              REG(a2, struct AHIPrivAudioCtrl* audioctrl),
              REG(a6, struct AHIBase*          AHIBase) )
{
  return LoadSound( sound, type, info, audioctrl, AHIBase );
}


/* gw_UnloadSound ************************************************************/

ULONG ASMCALL
gw_UnloadSound( REG(d0, UWORD                    sound),
                REG(a2, struct AHIPrivAudioCtrl* audioctrl),
                REG(a6, struct AHIBase*          AHIBase) )
{
  return UnloadSound( sound, audioctrl, AHIBase );
}


/* gw_PlayA ******************************************************************/

ULONG ASMCALL 
gw_PlayA( REG(a2, struct AHIPrivAudioCtrl* audioctrl),
          REG(a1, struct TagItem*          tags),
          REG(a6, struct AHIBase*          AHIBase) )
{
  return PlayA( audioctrl, tags, AHIBase );
}


/* gw_SampleFrameSize ********************************************************/

ULONG ASMCALL
gw_SampleFrameSize( REG(d0, ULONG           sampletype),
                    REG(a6, struct AHIBase* AHIBase) )
{
  return SampleFrameSize( sampletype, AHIBase );
}


/* gw_GetAudioAttrsA *********************************************************/

ULONG ASMCALL 
gw_GetAudioAttrsA( REG(d0, ULONG                    id),
                   REG(a2, struct AHIPrivAudioCtrl* actrl),
                   REG(a1, struct TagItem*          tags),
                   REG(a6, struct AHIBase*          AHIBase) )
{
  return GetAudioAttrsA( id, actrl, tags, AHIBase );
}


/* gw_BestAudioIDA ***********************************************************/

ULONG ASMCALL 
gw_BestAudioIDA( REG(a1, struct TagItem* tags),
                 REG(a6, struct AHIBase* AHIBase) )
{
  return BestAudioIDA( tags, AHIBase );
}


/* gw_AllocAudioRequestA *****************************************************/

struct AHIAudioModeRequester* ASMCALL
gw_AllocAudioRequestA( REG(a0, struct TagItem* tags),
                       REG(a6, struct AHIBase* AHIBase) )
{
  return AllocAudioRequestA( tags, AHIBase );
}


/* gw_AudioRequestA **********************************************************/

ULONG ASMCALL 
gw_AudioRequestA( REG(a0, struct AHIAudioModeRequester* req_in),
                  REG(a1, struct TagItem*               tags ),
                  REG(a6, struct AHIBase*               AHIBase) )
{
  return AudioRequestA( req_in, tags, AHIBase );
}


/* gw_FreeAudioRequest *******************************************************/

void ASMCALL 
gw_FreeAudioRequest( REG(a0, struct AHIAudioModeRequester* req),
                     REG(a6, struct AHIBase*               AHIBase) )
{
  FreeAudioRequest( req, AHIBase );
}


/* gw_NextAudioID ************************************************************/

ULONG ASMCALL
gw_NextAudioID( REG(d0, ULONG           id),
                REG(a6, struct AHIBase* AHIBase) )
{
  return NextAudioID( id, AHIBase );
}


/* gw_AddAudioMode ***********************************************************/

ULONG ASMCALL
gw_AddAudioMode( REG(a0, struct TagItem* DBtags),
                 REG(a6, struct AHIBase* AHIBase) )
{
  return AddAudioMode( DBtags, AHIBase );
}


/* gw_RemoveAudioMode ********************************************************/

ULONG ASMCALL
gw_RemoveAudioMode( REG(d0, ULONG           id),
                    REG(a6, struct AHIBase* AHIBase) )
{
  return RemoveAudioMode( id, AHIBase );
}


/* gw_LoadModeFile ***********************************************************/

ULONG ASMCALL
gw_LoadModeFile( REG(a0, UBYTE*          name),
                 REG(a6, struct AHIBase* AHIBase) )
{
  return LoadModeFile( name, AHIBase );
}


/* m68k_IndexToFrequency *****************************************************/

LONG STDARGS SAVEDS
m68k_IndexToFrequency( struct Gadget *gad, WORD level )
{
  return IndexToFrequency( gad, level );
}


/* m68k_DevProc **************************************************************/

void
m68k_DevProc( void )
{
  DevProc();
}


/* m68k_PreTimer  ************************************************************/

BOOL ASMCALL
m68k_PreTimer( REG(a2, struct AHIPrivAudioCtrl* audioctrl ) )
{
  return PreTimer( audioctrl );
}


/* m68k_PostTimer  ***********************************************************/

void
m68k_PostTimer( REG(a2, struct AHIPrivAudioCtrl* audioctrl ) )
{
  PostTimer( audioctrl );
}

#endif


/*** Some special wrappers ***************************************************/

#if defined( __AROS__ ) && !defined( __mc68000__ )

// We're not binary compatible!

#else

struct
{
    UWORD nop;                    // Just make sure the addr is 32-bit aligned
    UWORD pushm_d0_d1_a0_a1[2];
    UWORD jsr;
    ULONG addr;
    UWORD popm_d0_d1_a0_a1[2];
    UWORD rts;
} HookEntryPreserveAllRegs __attribute__ ((aligned (4))) =
{
  0x4E71,
  {0x48E7, 0xC0C0},
  0x4EB9, (ULONG) HookEntry,
  {0x4CDF, 0x0303},
  0x4E75
};


struct
{
    UWORD nop;
    UWORD pushm_d1_a0_a1[2];
    UWORD jsr;
    ULONG addr;
    UWORD popm_d1_a0_a1[2];
    UWORD extl_d0;
    UWORD rts;
} PreTimerPreserveAllRegs =
{
  0x4E71,
  {0x48E7, 0x40C0},
  0x4EB9, (ULONG) &m68k_PreTimer,
  {0x4CDF, 0x0302},
  0x48C0,
  0x4E75
};


struct
{
    UWORD nop;                    // Just make sure the addr is 32-bit aligned
    UWORD pushm_d0_d1_a0_a1[2];
    UWORD jsr;
    ULONG addr;
    UWORD popm_d0_d1_a0_a1[2];
    UWORD rts;
} PostTimerPreserveAllRegs __attribute__ ((aligned (4))) =
{
  0x4E71,
  {0x48E7, 0xC0C0},
  0x4EB9, (ULONG) &m68k_PostTimer,
  {0x4CDF, 0x0303},
  0x4E75
};

#endif /* !defined( __AROS__ ) */
