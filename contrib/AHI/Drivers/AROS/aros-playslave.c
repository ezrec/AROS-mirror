
#include <config.h>

#include <devices/ahi.h>
#include <libraries/ahi_sub.h>

#include "DriverData.h"
#include "library.h"

#define dd ((struct AROSData*) AudioCtrl->ahiac_DriverData)

/******************************************************************************
** The slave process **********************************************************
******************************************************************************/

#undef SysBase

static void Slave( struct ExecBase* SysBase );

#if defined( __AROS__ )

#include <aros/asmcall.h>

AROS_UFH3(LONG, SlaveEntry,
	  AROS_UFHA(STRPTR, argPtr, A0),
	  AROS_UFHA(ULONG, argSize, D0),
	  AROS_UFHA(struct ExecBase *, SysBase, A6))
{
   AROS_USERFUNC_INIT
   Slave( SysBase );
   return 0;
   AROS_USERFUNC_EXIT
}

#else

void SlaveEntry(void)
{
  struct ExecBase* SysBase = *((struct ExecBase**) 4);

  Slave( SysBase );
}

#endif

static void
Slave( struct ExecBase* SysBase )
{
  struct AHIAudioCtrlDrv* AudioCtrl;
  struct DriverBase*      AHIsubBase;
  struct AROSBase*        AROSBase;
  BOOL                    running;
  ULONG                   signals;

  AudioCtrl  = (struct AHIAudioCtrlDrv*) FindTask( NULL )->tc_UserData;
  AHIsubBase = (struct DriverBase*) dd->ahisubbase;
  AROSBase   = (struct AROSBase*) AHIsubBase;

  dd->slavesignal = AllocSignal( -1 );

  if( dd->slavesignal != -1 )
  {
    // Everything set up. Tell Master we're alive and healthy.

    Signal( (struct Task*) dd->mastertask,
            1L << dd->mastersignal );

    running = TRUE;

    while( running )
    {
      signals = SetSignal(0L,0L);

      if( signals & ( SIGBREAKF_CTRL_C | (1L << dd->slavesignal) ) )
      {
        running = FALSE;
      }
      else
      {
	int skip_mix;
	int skip     = 0;
	int offset   = 0;
	int samples  = AudioCtrl->ahiac_BuffSamples;
	int bytes    = AudioCtrl->ahiac_BuffSamples * 2;
	int i;
	WORD* src;
	WORD* dst;

	skip_mix = CallHookA( AudioCtrl->ahiac_PreTimerFunc,
			      (Object*) AudioCtrl, 0 );
	
        CallHookPkt( AudioCtrl->ahiac_PlayerFunc, AudioCtrl, NULL );

	if( ! skip_mix )
	{
	  CallHookPkt( AudioCtrl->ahiac_MixerFunc, AudioCtrl, dd->mixbuffer );
	}

	switch( AudioCtrl->ahiac_BuffType )
	{
	  case AHIST_M16S:
	    skip = 1;
	    offset = 0;
	    break;
	    
	  case AHIST_M32S:
	    skip = 2;
	    offset = 1;
	    break;
	    
	  case AHIST_S16S:
	    skip = 1;
	    offset = 0;
	    samples *= 2;
	    bytes *= 2;
	    break;

	  case AHIST_S32S:
	    skip = 2;
	    offset = 1;
	    samples *= 2;
	    bytes *= 2;
	    break;
	}

	src = ((WORD*) dd->mixbuffer) + offset;
	dst = dd->mixbuffer;

	for( i = 0; i < samples; ++i )
	{
	  *dst++ = *src;
	  src += skip;
	}
	
	OSS_Write( dd->mixbuffer,  bytes );
	
	CallHookA( AudioCtrl->ahiac_PostTimerFunc, (Object*) AudioCtrl, 0 );

	// Now Delay() until there are at least 'bytes' bytes available
	// again.
	
	{
	  int frag_avail, frag_alloc, frag_size, bytes_avail;

	  OSS_GetOutputInfo( &frag_avail, &frag_alloc, &frag_size,
			     &bytes_avail );
	  if( bytes_avail < bytes )
	  {
	    Delay( 1 );
	  }
	  
//	  KPrintF( "%ld fragments available, %ld alloced (%ld bytes each). %ld bytes total\n", frag_avail, frag_alloc, frag_size, bytes_avail );
	}
      }
    }
  }

  FreeSignal( dd->slavesignal );
  dd->slavesignal = -1;

  Forbid();

  // Tell the Master we're dying

  Signal( (struct Task*) dd->mastertask,
          1L << dd->mastersignal );

  dd->slavetask = NULL;

  // Multitaking will resume when we are dead.
}
