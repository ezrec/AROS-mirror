
#include <stdio.h>

#include <powerpc/powerpc.h>
#include <proto/exec.h>
#include <proto/powerpc.h>

#include "elfloader.h"
#include "ppcheader.h"

int TimerBase;
int MixGeneric;
int MixPowerUp;
void* PPCObject;

struct Library* PowerPCBase = NULL;

int
main( void )
{
  PowerPCBase = OpenLibrary( "powerpc.library", 15 );

  printf( "PowerPCBase: 0x%08lx\n", (ULONG) PowerPCBase );

  if( PowerPCBase != NULL )
  {
    PPCObject = ELFLoadObject( "devs:ahi.elf" );

    CacheClearU();

    printf( "PPCObject: 0x%08lx\n", (ULONG) PPCObject );

    if( PPCObject != NULL )
    {
      void* init    = NULL;
      void* cleanup = NULL;
      
      ELFGetSymbol( PPCObject, "InitWarpUp", &init );
      ELFGetSymbol( PPCObject, "CleanUpWarpUp", &cleanup );

      printf( "init:    0x%08lx\n", (ULONG) init );
      printf( "cleanup: 0x%08lx\n", (ULONG) cleanup );

      if( init != NULL && cleanup != NULL )
      {
        LONG status;

        struct WarpUpContext* wc = NULL;

        struct PPCArgs init_pps = 
        {
          init,
          0,
          0,
          NULL,
          0,
          { 
            0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0
          },
          {
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
          }
        };

        struct PPCArgs cleanup_pps = 
        {
          cleanup,
          0,
          0,
          NULL,
          0,
          { 
            0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0
          },
          {
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
          }
        };

        wc = AllocVec32( sizeof( struct WarpUpContext ), 
                         MEMF_CLEAR | MEMF_CHIP );

        wc->PowerPCBase = PowerPCBase;

        init_pps.PP_Regs[ 0 ] = wc;
        status = RunPPC( &init_pps );

        printf( "status: %ld; xlock: 0x%08lx\n", status, (ULONG) wc->XLock);

        wc->Active      = TRUE;
        CausePPCInterrupt();
        wc->Active      = FALSE;
        CausePPCInterrupt();

        cleanup_pps.PP_Regs[ 0 ] = wc;
        status = RunPPC( &cleanup_pps );
        
        printf( "status: %ld\n", status );
        
        FreeVec32( wc );
      }

      ELFUnLoadObject( PPCObject );
    }
  
    CloseLibrary( PowerPCBase );
  }

  return 0;
}


APTR
AHIAllocVec( ULONG byteSize, ULONG requirements )
{
  return AllocVec32( byteSize, requirements );
}

void
AHIFreeVec( APTR memoryBlock )
{
  FreeVec32( memoryBlock );
}
