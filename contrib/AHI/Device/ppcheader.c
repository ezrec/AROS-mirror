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

#if defined( VERSIONPPC )

#include <config.h>
#include <CompilerSpecific.h>

#include <exec/types.h>
#include <hardware/intbits.h>
#include <powerup/ppclib/tasks.h>
#include <powerpc/powerpc.h>
#include <powerup/gcclib/powerup_protos.h>

#include "version.h"
#include "ahi_def.h"
#include "mixer.h"
#include "ppcheader.h"

/******************************************************************************
** Prototypes *****************************************************************
******************************************************************************/

int
CallMixroutine( struct PowerPCContext*   context );

void
FlushCache( void* address, unsigned long length );

void
FlushCacheAll( void  );

void
InvalidateCache( void* address, unsigned long length );

void WarpUpInt( void );

/******************************************************************************
** First address **************************************************************
******************************************************************************/

// This must be the first code in the ELF object due to a bug in
// ppc.library < 46.26

asm("
        .text

        .align  2
        .globl  KernelObject
      	.type   KernelObject,@function


KernelObject:
        stwu    1,-24(1)
        mflr    0
        stw     0,28(1)
        stw     11,8(1)
        stw     12,12(1)
        stw     13,16(1)

        bl      CallMixroutine

        lwz     11,8(1)
        lwz     12,12(1)
        lwz     13,16(1)
        lwz     0,28(1)
        mtlr    0
        addi    1,1,24
        blr

");


/******************************************************************************
** Function used to call the actual mixing routine ****************************
******************************************************************************/

int
CallMixroutine( struct PowerPCContext* context )
{
  struct AHIPrivAudioCtrl* audioctrl;
  struct AHISoundData*     sd;
  int                      i;

  audioctrl = context->AudioCtrl;

//PPCkprintf( "Start? %08lx, %08lx, %08lx\n", context, audioctrl, audioctrl->ahiac_PowerPCContext );
  // Wait for start signal...

  while( audioctrl->ahiac_PowerPCContext->Command != PPCC_COM_START );

//PPCkprintf( "Start!\n" );

  // Start m68k interrupt handler

  audioctrl->ahiac_PowerPCContext->Command = PPCC_COM_INIT;
  *((WORD*) 0xdff09C)  = INTF_SETCLR | INTF_PORTS;

//PPCkprintf( "Int trigger!\n" );

  // Invalidate dynamic sample sounds (which is faster than flushing).
  // Currently, the PPC is assumed not to modify dynamic samples.
  // It makes sense as long as no PPC hooks can be called from AHI.
  // Anyway, each dynamic sample is flushed on the m68k side before
  // this routine is called, and invalidated here on the PPC side.
  // However, should a dynamic sample start at address 0, which
  // probably means that the whole address space is used for that
  // sample, all of the data caches are instead flushed.

  sd = audioctrl->ahiac_SoundDatas;

  for( i = 0; i < audioctrl->ac.ahiac_Sounds; i++)
  {
    if( sd->sd_Type == AHIST_DYNAMICSAMPLE )
    {
      if( sd->sd_Addr == NULL )
      {
        // *Flush* all and exit (add an L2 cache and watch this code break!)

        FlushCacheAll();
        break;
      }
      else
      {
        // *Invalidate* block

        InvalidateCache( sd->sd_Addr,
                         sd->sd_Length * InternalSampleFrameSize( sd->sd_Type ) );
      }
    }
    sd++;
  }


//PPCkprintf( "Wait!\n" );

  // Wait for m68k interrupt handler to go active

  while( audioctrl->ahiac_PowerPCContext->Command != PPCC_COM_ACK );

//PPCkprintf( "Mixing...!\n" );
  // Mix

  Mix( context->Hook, context->Dst, audioctrl );

  // Flush mixed samples to memory

  FlushCache( context->Dst, audioctrl->ahiac_BuffSizeNow );

  // Kill the m68k interrupt handler

  audioctrl->ahiac_PowerPCContext->Command = PPCC_COM_QUIT;
  *((WORD*) 0xdff09C)  = INTF_SETCLR | INTF_PORTS;

  // Wait for it

  while( audioctrl->ahiac_PowerPCContext->Command != PPCC_COM_ACK );

  audioctrl->ahiac_PowerPCContext->Command = PPCC_COM_FINISHED;
  return 0;
}

/******************************************************************************
** Cache manipulation routines ************************************************
******************************************************************************/

asm( "
        .text

/*     r3 = beginning address of data block to flush
 *     r4 = size of data block to flush (in bytes)
 *     assumes cache block granule is 32 bytes
 */

        .align  2
        .globl  FlushCache
      	.type   FlushCache,@function

FlushCache:
        addi    4,4,31
        srwi    4,4,5           /* convert to cache blocks to flush */
        mtctr   4
        li      4,0
1:
        dcbf    3,4             /* flush data cache block to mem */
        addi    4,4,32
        bdnz    1b

        sync                    /* force mem transactions to complete */
        blr                     /* return to calling routine */


        .align  2
        .globl  FlushCacheAll
      	.type   FlushCacheAll,@function

FlushCacheAll:

/* Load the entire data cache with known contents. */
        li      3,-16           /* Start at address 0 */
        li      4,2*256         /* 2 ways, 256 sets per way */
        mtctr   4               /* (use CTR register to save an instruction) */
1:
        lwzu    5,16(3)         /* load a cache line if it's not already present */
        bdnz    1b

/* Flush those known contents from the cache. */
        li      3,0             /* Read 2*128*16 bytes at address 0 */
        mtctr   4               /* (use CTR register to save an instruction) */
2:
        dcbf    0,3             /* flush a cache line */
        addi    3,3,16          /* next line: assumes cache lines are 16 bytes */
        bdnz    2b
        sync
        blr


/*     r3 = beginning address of data block to flush
 *     r4 = size of data block to flush (in bytes)
 *     assumes cache block granule is 32 bytes
 */
        .align  2
        .globl  InvalidateCache
      	.type   InvalidateCache,@function

InvalidateCache:
        addi    4,4,31
        srwi    4,4,5           /* convert to cache blocks to invalidate */
        mtctr   4
        li      4,0
1:
        dcbi    3,4             /* invalidate data cache block */
        addi    4,4,32
        bdnz    1b

        sync                    /* force mem transactions to complete */
        blr                     /* return to calling routine */

");


/******************************************************************************
** WarpUp stuff ***************************************************************
******************************************************************************/

struct TagItem InitTags[] =
{
  { EXCATTR_CODE,  (ULONG) &WarpUpInt,                    },
  { EXCATTR_DATA,  0,                                     },
  { EXCATTR_NAME,  (ULONG) "AHI/WarpUp Exception Handler" },
  { EXCATTR_PRI,   32,                                    },
  { EXCATTR_EXCID, EXCF_INTERRUPT,                        },
  { EXCATTR_FLAGS, EXCF_GLOBAL | EXCF_LARGECONTEXT,       },
  { TAG_DONE,      0                                      }
};

asm( "
          .text

_LVOSetExcHandler = -516
_LVORemExcHandler = -522
_LVOSetExcMMU     = -576
_LVOClearExcMMU   = -582

EXCRETURN_ABORT   = 1

# struct PowerPCContext

ppcc_Command          = 0*4
ppcc_Argument         = 1*4
ppcc_SlaveProcess     = 2*4
ppcc_MainProcess      = 3*4
ppcc_CurrentMixBuffer = 4*4
ppcc_Active           = 5*4
ppcc_Hook             = 6*4
ppcc_Dst              = 7*4
ppcc_XLock            = 8*4
ppcc_AudioCtrl        = 9*4
ppcc_PowerPCBase      = 10*4

/* InitWarpUp ****************************************************************/

        .align  2
        .globl  InitWarpUp
        .type   InitWarpUp,@function

/*     r3 = struct WarpUpContext*
 */

InitWarpUp:
        mflr    0
        stw     0,8(1)
        mfcr    0
        stw     0,4(1)
        stw     13,-4(1)
        subi    13,1,4
        stwu    1,-(28+14*4+1*4)(1)

        stw     14,-4(13)

        mr      14,3                        # Save WarpUpContext in r14

# Build the tag list on the stack

        lis     4,(InitTags-4)@ha
        addi    4,4,InitTags-4@l

        addi    5,1,28-4

1:
        lwzu    6,4(4)
        stwu    6,4(5)
        cmpwi   0,6,0
        lwzu    6,4(4)
        stwu    6,4(5)
        bne     1b

        stw     14,28+3*4(1)                # Store WarpUpContext in tag list

# Register the exception handler

        lwz     3,ppcc_PowerPCBase(14)
        addi    4,1,28
        lwz     0,_LVOSetExcHandler+2(3)
        mtlr    0
        blrl

        stw     3,ppcc_XLock(14)

        lwz     14,-4(13)

        lwz     1,0(1)
        lwz     13,-4(1)
        lwz     0,4(1)
        mtcr    0
        lwz     0,8(1)
        mtlr    0
        blr


/* WarpUpInt *****************************************************************/

        .align  2
        .globl  WarpUpInt
        .type   WarpUpInt,@function

WarpUpInt:
        mflr    0
        stw     0,8(1)
        mfcr    0
        stw     0,4(1)
        stw     13,-4(1)
        subi    13,1,4
        stwu    1,-(28+1*4)(1)

        stw     14,-4(13)

        mr      14,2

# Set up MMU

        lwz     3,ppcc_PowerPCBase(14)
        lwz     0,_LVOSetExcMMU+2(3)
        mtlr    0
        blrl

# Test and clear activation flag (is this out interrupt or somebody elses?)

        addi    3,14,ppcc_Active
        li      4,0
1:
        lwarx   5,0,3
        stwcx.  4,0,3
        bne-    1b

        cmpwi   0,5,0
        beq     2f

# Call the CallMixroutine (V.4 ABI)

        stwu    1,-16(1)
        stw     2,8(1)
        stw     13,12(1)

        mr      3,14
        bl      CallMixroutine

        lwz     2,8(1)
        lwz     13,12(1)
        addi    1,1,16
2:

# Restore MMU

        lwz     3,ppcc_PowerPCBase(14)
        lwz     0,_LVOClearExcMMU+2(3)
        mtlr    0
        blrl

        li      3,EXCRETURN_ABORT

        lwz     14,-4(13)

        lwz     1,0(1)
        lwz     13,-4(1)
        lwz     0,4(1)
        mtcr    0
        lwz     0,8(1)
        mtlr    0
        blr

/* CleanUpWarpUp *************************************************************/

        .align  2
        .globl  CleanUpWarpUp
        .type   CleanUpWarpUp,@function

/*     r3 = struct WarpUpContext*
 */

CleanUpWarpUp:
        mflr    0
        stw     0,8(1)
        mfcr    0
        stw     0,4(1)
        stw     13,-4(1)
        subi    13,1,4
        stwu    1,-28(1)

# Unregister the exception handler

        lwz     4,ppcc_XLock(3)
        lwz     3,ppcc_PowerPCBase(3)
        lwz     0,_LVORemExcHandler+2(3)
        mtlr    0
        blrl

        lwz     1,0(1)
        lwz     13,-4(1)
        lwz     0,4(1)
        mtcr    0
        lwz     0,8(1)
        mtlr    0
        blr
");


/******************************************************************************
** Library & Linking **********************************************************
******************************************************************************/

// Just some library stuff... All the stuff will have to be added 
// in the final release. TODO!

ULONG	__LIB_Version  = VERSION;
ULONG	__LIB_Revision = REVISION;

static const char VersTag[] = 
 "$VER: ahi.elf " VERS " ©1994-1999 Martin Blom. " CPU " version.\r\n";


// Make sure all add-routines are fetched.

static void* a1 = AddByteMono;
static void* a2 = AddLofiByteMono;

#endif /* defined( VERSIONPPC ) */
