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

#include <dos/dos.h>
#include <exec/execbase.h>
#include <exec/interrupts.h>
#include <hardware/intbits.h>
#include <powerup/ppclib/interface.h>
#include <powerup/ppclib/object.h>
#include <powerup/ppclib/tasks.h>
#include <powerup/ppclib/memory.h>
#include <powerpc/powerpc.h>
#include <utility/tagitem.h>
#include <proto/exec.h> 
#include <proto/ppc.h> 
#include <proto/powerpc.h> 
#include <proto/utility.h> 

#include "ahi_def.h"
#include "header.h"
#include "misc.h"
#include "mixer.h"
#include "ppcheader.h"
#include "ppchandler.h"

static void
MixBuffer( void*                     mixbuffer,
           struct AHIPrivAudioCtrl*  audioctrl );

static INTERRUPT SAVEDS int
Interrupt( struct AHIPrivAudioCtrl *audioctrl __asm( "a1" ) );


void
PPCHandler( void )
{
  // Handles the invocation of the PPC mixing code (via
  // PPCRunKernelObject() or CausePPCInterrupt(), in order to make it
  // possible to invalidate caches from the PPC side. 

  struct MsgPort*           port          = NULL;
  struct PPCHandlerMessage* msg           = NULL;
  struct AHIPrivAudioCtrl*  audioctrl     = NULL;

  void*                     mixbuffer1    = NULL;
  void*                     mixbuffer2    = NULL;
  struct Interrupt*         mixinterrupt  = NULL;

  BOOL                      rc;

  struct Process*           slave_proc   = NULL;
  struct Process*           master_proc   = NULL;

  slave_proc = (struct Process*) FindTask( NULL );
  port = &slave_proc->pr_MsgPort;

  WaitPort( port );

  msg       = (struct PPCHandlerMessage*) GetMsg( port );
  audioctrl = msg->AudioCtrl;

  audioctrl->ahiac_PowerPCContext = AHIAllocVec( sizeof( struct PowerPCContext ), 
                                                 MEMF_PUBLIC 
                                                 | MEMF_CLEAR 
                                                 | MEMF_NOCACHEPPC 
                                                 | MEMF_NOCACHEM68K );

  if( audioctrl->ahiac_PowerPCContext == NULL )
  {
    Req( "Out of memory." );
  }
  else
  {
    audioctrl->ahiac_PowerPCContext->SlaveProcess = slave_proc;

    audioctrl->ahiac_PowerPCContext->Command      = PPCC_COM_NONE;
    audioctrl->ahiac_PowerPCContext->Active       = FALSE;

    audioctrl->ahiac_PowerPCContext->AudioCtrl    = audioctrl;
    audioctrl->ahiac_PowerPCContext->PowerPCBase  = PowerPCBase;

    rc = TRUE;

    switch( MixBackend )
    {
      case MB_NATIVE:
        Req( "Internal error: Illegal MixBackend in PPCHandler()" );
        break;
    
      case MB_POWERUP:
        mixbuffer1 = AHIAllocVec( audioctrl->ac.ahiac_BuffSize,
                                  MEMF_PUBLIC | MEMF_CLEAR | MEMF_NOCACHEM68K );
        mixbuffer2 = AHIAllocVec( audioctrl->ac.ahiac_BuffSize,
                                  MEMF_PUBLIC | MEMF_CLEAR | MEMF_NOCACHEM68K );
                                  
        if( mixbuffer1 == NULL || mixbuffer2 == NULL )
        {
          rc = FALSE;
        }
        break;
    
      case MB_WARPUP:
        mixbuffer1 = AHIAllocVec( audioctrl->ac.ahiac_BuffSize,
                                  MEMF_PUBLIC | MEMF_CLEAR );
        mixbuffer2 = AHIAllocVec( audioctrl->ac.ahiac_BuffSize,
                                  MEMF_PUBLIC | MEMF_CLEAR );
    
        if( mixbuffer1 == NULL || mixbuffer2 == NULL )
        {
          rc = FALSE;
        }
        else
        {
          // Initialize the WarpUp side
        
          struct PPCArgs args = 
          {
            NULL,
            0,
            0,
            NULL,
            0,
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }
          };
    
          args.PP_Regs[ 0 ] = (ULONG) audioctrl->ahiac_PowerPCContext;
    
          if( AHIGetELFSymbol( "InitWarpUp", &args.PP_Code ) )
          {
            if( RunPPC( &args ) != PPERR_SUCCESS )
            {
              Req( "Call to InitWarpUp() failed." );
              rc = FALSE;
            }
          }
          else
          {
            Req( "Unable to fetch symbol 'InitWarpUp'." );
            rc = FALSE;
          }
        }
    
        break;
    }

    if( rc )
    {
      // Set a default, empty buffer, which will be fed to the sound card
      // driver the first time FillBuffer() is called.

      audioctrl->ahiac_PowerPCContext->CurrentMixBuffer = mixbuffer1;

      mixinterrupt = AllocVec( sizeof( struct Interrupt ),
                               MEMF_PUBLIC | MEMF_CLEAR );

      if( mixinterrupt == NULL )
      {
        Req( "Out of memory." );
      }
      else
      {
        ULONG signalset;
        BOOL  loop;

        mixinterrupt->is_Node.ln_Type = NT_INTERRUPT;
        mixinterrupt->is_Node.ln_Pri  = 127;
        mixinterrupt->is_Node.ln_Name = AHINAME " PPC Handler Interrupt";
        mixinterrupt->is_Data         = audioctrl;
        mixinterrupt->is_Code         = (void(*)(void)) Interrupt;

        AddIntServer( INTB_PORTS, mixinterrupt );

        // Reply the startup message to signal that we're alive
        ReplyMsg( (struct Message*) msg );
        msg = NULL;

        loop = TRUE;

        while( loop )
        {
          signalset = Wait( SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_F );
          
          if( signalset & SIGBREAKF_CTRL_C )
          {
            loop = FALSE;
          }

          if( signalset & SIGBREAKF_CTRL_F )
          {
            // Fill buffer 1

            MixBuffer( mixbuffer1, audioctrl );

            audioctrl->ahiac_PowerPCContext->CurrentMixBuffer = mixbuffer1;

            // Swap buffers
            mixbuffer1 = mixbuffer2;
            mixbuffer2 = audioctrl->ahiac_PowerPCContext->CurrentMixBuffer;
          }
        }
      }
    }
  }

  if( mixinterrupt != NULL )
  {
    RemIntServer( INTB_PORTS, mixinterrupt );
  }

  switch( MixBackend )
  {
    case MB_NATIVE:
      break;

    case MB_POWERUP:
      break;

    case MB_WARPUP:
    {
      // Clean up the WarpUp side
      
      struct PPCArgs args = 
      {
        NULL,
        0,
        0,
        NULL,
        0,
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }
      };

      if( audioctrl->ahiac_PowerPCContext != NULL )
      {
        args.PP_Regs[ 0 ] = (ULONG) audioctrl->ahiac_PowerPCContext;
        
        if( AHIGetELFSymbol( "CleanUpWarpUp", &args.PP_Code ) )
        {
          RunPPC( &args );
        }
        else
        {
          Req( "Unable to fetch symbol 'CleanUpWarpUp'." );
        }
      }

      break;
    }
  }

  FreeVec( mixinterrupt );
  mixinterrupt = NULL;

  AHIFreeVec( mixbuffer1 );
  mixbuffer1 = NULL;

  AHIFreeVec( mixbuffer2 );
  mixbuffer2 = NULL;

  master_proc = audioctrl->ahiac_PowerPCContext->MainProcess;
  AHIFreeVec( audioctrl->ahiac_PowerPCContext );
  audioctrl->ahiac_PowerPCContext = NULL;
  
  Forbid();

  if( msg != NULL )
  {
    // Reply the startup message if not already done
    ReplyMsg( (struct Message*) msg );
  }
  else
  {
    if( master_proc != NULL )
    {
      // Acknowledge kill signal
      Signal( (struct Task*) master_proc, SIGF_SINGLE );
    }
  }
}


void
FillBuffer( void*                     dst,
            struct AHIPrivAudioCtrl*  audioctrl )
{
  // Fills the driver-allocated buffers from a filled internal mix-buffer

  // The PPC mix buffer is either not m68k-cachable or cleared;
  // just read from it.

  memcpy( dst,
          audioctrl->ahiac_PowerPCContext->CurrentMixBuffer,
          audioctrl->ahiac_BuffSizeNow );

  // Ask slave process to prepare next buffer

  Signal( (struct Task*) audioctrl->ahiac_PowerPCContext->SlaveProcess,
          SIGBREAKF_CTRL_F );
}


static void
MixBuffer( void*                     mixbuffer,
           struct AHIPrivAudioCtrl*  audioctrl )
{
  // Calls the PPC mixing code to fill a buffer with mixed samples

  struct AHISoundData *sd;
  int                  i;
  BOOL                 flushed = FALSE;

//kprintf("M");
  // Flush all DYNAMICSAMPLE's

  sd = audioctrl->ahiac_SoundDatas;

  for( i = 0; i < audioctrl->ac.ahiac_Sounds; i++)
  {
    if( sd->sd_Type == AHIST_DYNAMICSAMPLE )
    {
      if( sd->sd_Addr == NULL )
      {
//kprintf("a");
        // Flush all and exit
        CacheClearU();
        flushed = TRUE;
        break;
      }
      else
      {
//kprintf("b");
        switch( MixBackend )
        {
          case MB_POWERUP:
            PPCCacheClearE( sd->sd_Addr,
                            sd->sd_Length * InternalSampleFrameSize( sd->sd_Type ),
                            CACRF_ClearD );
            break;

          case MB_WARPUP:
            SetCache68K( CACHE_DCACHEFLUSH,
                         sd->sd_Addr,
                         sd->sd_Length * InternalSampleFrameSize( sd->sd_Type ) );
            break;

          case MB_NATIVE:
            // Ugh!
            break;
        }
      }
//kprintf("c");
    }
    sd++;
  }

//kprintf("d");
  if( ! flushed && MixBackend == MB_WARPUP )
  {
    /* Since the PPC mix buffer is m68k cacheable in WarpUp, we have to
       flush, or better, *invalidate* the cache before mixing starts. */

    SetCache68K( CACHE_DCACHEFLUSH,
                 mixbuffer,
                 audioctrl->ahiac_BuffSizeNow );
  }
//kprintf("e");

  audioctrl->ahiac_PowerPCContext->Hook         = audioctrl->ac.ahiac_MixerFunc;
  audioctrl->ahiac_PowerPCContext->Dst          = mixbuffer;
  audioctrl->ahiac_PowerPCContext->Active       = TRUE;
  audioctrl->ahiac_PowerPCContext->Command      = PPCC_COM_START;

  switch( MixBackend )
  {
    case MB_POWERUP:
    {
      struct ModuleArgs mod =
      {
        IF_CACHEFLUSHNO, 0, 0,
        IF_CACHEFLUSHNO, 0, 0,

        (ULONG) audioctrl->ahiac_PowerPCContext,
        TRUE,                                  // Flush buffer afterwards!
        0, 0, 0, 0, 0, 0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      };

//kprintf("K");
      PPCRunKernelObject( PPCObject, &mod );
//kprintf("k");
      break;
    }

    case MB_WARPUP:
//kprintf("C");
      CausePPCInterrupt();
//kprintf("c");
      break;

    case MB_NATIVE:
      // Ugh!
      break;
  }

//kprintf("f");
  while( audioctrl->ahiac_PowerPCContext->Command != PPCC_COM_FINISHED );
//kprintf("g");
}


static INTERRUPT SAVEDS int
Interrupt( struct AHIPrivAudioCtrl *audioctrl __asm( "a1" ) )
{
  if( audioctrl->ahiac_PowerPCContext->Command != PPCC_COM_INIT )
  {
    /* Not for us, continue */
    return 0;
  }
  else
  {
    BOOL running = TRUE;
//kprintf("I");
    while( running )
    {
//kprintf("0");
      switch( audioctrl->ahiac_PowerPCContext->Command )
      {
        case PPCC_COM_INIT:
//kprintf("1");
          // Keep looping
          audioctrl->ahiac_PowerPCContext->Command = PPCC_COM_ACK;
          break;

        case PPCC_COM_ACK:
//kprintf("2");
          // Keep looping, try not to waste to much memory bandwidth...
          asm( "stop #(1<<13) | (2<<8)" : );
          break;

        case PPCC_COM_SOUNDFUNC:
//kprintf("3");
          CallHookPkt( audioctrl->ac.ahiac_SoundFunc,
                       audioctrl,
                       (APTR) audioctrl->ahiac_PowerPCContext->Argument );
          audioctrl->ahiac_PowerPCContext->Command = PPCC_COM_ACK;
          break;

        case PPCC_COM_DEBUG:
//kprintf("4");
          kprintf( "%lx ", (ULONG) audioctrl->ahiac_PowerPCContext->Argument );
          audioctrl->ahiac_PowerPCContext->Command = PPCC_COM_ACK;
          break;

        case PPCC_COM_QUIT:
//kprintf("5");
          running = FALSE;
          audioctrl->ahiac_PowerPCContext->Command = PPCC_COM_ACK;
          break;
        
        case PPCC_COM_NONE:
        default:
//kprintf("6");
          // Error
          running  = FALSE;
          audioctrl->ahiac_PowerPCContext->Command = PPCC_COM_ACK;
          break;
      }
    }
//kprintf("i");

    /* End chain! */
    return 1;
  }
};
