
#include <exec/exec.h>
#include <devices/ahi.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <libraries/ahi_sub.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/ahi_sub.h>

struct DeviceData {
  UBYTE            m_Pad1;
  BYTE             m_MasterSignal;
  BYTE             m_SlaveSignal;
  UBYTE            m_Pad[ 2 ];
  struct Process*  m_MasterTask;
  struct Process*  m_SlaveTask;
  struct Library*  m_AHIsubBase;
  APTR             m_MixBuffer;
};

#define dd ((struct devicedata*) AudioCtrl->ahiac_DriverData)

extern char __far _LibID[];
extern char __far _LibName[];

void KPrintF(char* fmt,...);


/******************************************************************************
** Globals ********************************************************************
******************************************************************************/

#define FREQUENCIES 13

static const ULONG frequency[FREQUENCIES] =
{
  8000,     // µ- and A-Law
  11025,    // CD/4
  12000,    // DAT/4
  14700,    // CD/3
  16000,    // DAT/3
  17640,    // CD/2.5
  19200,    // DAT/2.5
  22050,    // CD/2
  24000,    // DAT/2
  29400,    // CD/1.5
  32000,    // DAT/1.5
  44100,    // CD
  48000,    // DAT
};

struct Library*         AHIsubBase     = NULL;
struct DosLibrary*      DOSBase        = NULL;
struct IntuitionBase*   IntuitionBase  = NULL;
struct Library*         UtilityBase    = NULL;


/******************************************************************************
** Library init ***************************************************************
******************************************************************************/

int  __saveds __asm 
__UserLibInit( register __a6 struct Library* libbase )
{

  AHIsubBase    = libbase;
  DOSBase       = (struct DosLibrary*) OpenLibrary( "dos.library", 37 );
  IntuitionBase = (struct IntuitionBase*) OpenLibrary( "intuition.library", 37 );
  UtilityBase   = OpenLibrary( "utility.library", 37 );

  if( IntuitionBase == NULL )
  {
    Alert( AN_Unknown|AG_OpenLib|AO_Intuition );
    return 1;
  }

  if( DOSBase == NULL )
  {
    struct EasyStruct req =
    {
      sizeof( struct EasyStruct ), 0, _LibName,
      "Cannot open 'dos.library' v37", "OK"
    };

    EasyRequest( NULL, &req, NULL, NULL );
    return 1;
  }

  if( UtilityBase == NULL )
  {
    struct EasyStruct req =
    {
      sizeof( struct EasyStruct ), 0, _LibName,
      "Cannot open 'utility.library' v37", "OK"
    };

    EasyRequest( NULL, &req, NULL, NULL );
    return 1;
  }

  return 0;
}


/******************************************************************************
** Library clean-up ***********************************************************
******************************************************************************/

void __saveds __asm
__UserLibCleanup( register __a6 struct Library* libbase )
{
  CloseLibrary( (struct Library*) DOSBase);
  CloseLibrary( (struct Library*) IntuitionBase);
  CloseLibrary( UtilityBase );
}


/******************************************************************************
** AHIsub_AllocAudio **********************************************************
******************************************************************************/

ULONG __asm __saveds
intAHIsub_AllocAudio( register __a1 struct TagItem* tagList,
                      register __a2 struct AHIAudioCtrlDrv* AudioCtrl )
{
  AudioCtrl->ahiac_DriverData = AllocVec( sizeof( struct devicedata ),
                                          MEMF_CLEAR | MEMF_PUBLIC );

  if( AudioCtrl->ahiac_DriverData != NULL )
  {
    dd->v_SlaveSignal      = -1;
    dd->v_MasterSignal     = AllocSignal( -1 );
    dd->v_MasterTask       = (struct Process*) FindTask( NULL );
    dd->v_AHIsubBase       = AHIsubBase;
  }
  else
  {
    return AHISF_ERROR;
  }

  if( dd->v_MasterSignal == -1 )
  {
    return AHISF_ERROR;
  }

  return AHISF_KNOWHIFI | 
         AHISF_KNOWSTEREO |
         AHISF_MIXING |
         AHISF_TIMING;
}


/******************************************************************************
** AHIsub_FreeAudio ***********************************************************
******************************************************************************/

void __asm __saveds
intAHIsub_FreeAudio( register __a2 struct AHIAudioCtrlDrv* AudioCtrl )
{
  if( AudioCtrl->ahiac_DriverData != NULL )
  {
    FreeSignal( dd->v_MasterSignal );
    FreeVec( AudioCtrl->ahiac_DriverData );
    AudioCtrl->ahiac_DriverData = NULL;
  }
}

ULONG __asm __saveds
intAHIsub_Start( register __d0 ULONG Flags,
                 register __a2 struct AHIAudioCtrlDrv* AudioCtrl )
{
  AHIsub_Stop( Flags, AudioCtrl );

  if(Flags & AHISF_PLAY)
  {
    dd->v_MixBuffer = AllocVec( AudioCtrl->ahiac_BuffSize,
                                 MEMF_ANY | MEMF_PUBLIC );

    if( dd->v_MixBuffer == NULL ) return AHIE_NOMEM;

    Forbid();

    dd->v_SlaveTask = CreateNewProcTags( NP_Entry,     SlaveEntry,
                                         NP_Name,      _LibName,
                                         NP_Priority,  -1,
                                         TAG_DONE );

    if( dd->v_SlaveTask != NULL )
    {
      dd->v_SlaveTask->pr_Task.tc_UserData = AudioCtrl;
    }

    Permit();

    if( dd->v_SlaveTask != NULL )
    {
      Wait( 1L << dd->v_MasterSignal );  // Wait for slave to come alive

      if( dd->v_SlaveTask == NULL )      // Is slave alive or dead?
      {
        return AHIE_UNKNOWN;
      }
    }
    else
    {
      return AHIE_NOMEM;                 // Well, out of memory or whatever...
    }
  }

  if( Flags & AHISF_RECORD )
  {
    return AHIE_UNKNOWN;
  }

  return AHIE_OK;
}


/******************************************************************************
** AHIsub_Update **************************************************************
******************************************************************************/

void __asm __saveds
intAHIsub_Update( register __d0 ULONG Flags,
                  register __a2 struct AHIAudioCtrlDrv* AudioCtrl )
{
  // Empty function
}


/******************************************************************************
** AHIsub_Stop ****************************************************************
******************************************************************************/

void __asm __saveds
intAHIsub_Stop( register __d0 ULONG Flags,
                register __a2 struct AHIAudioCtrlDrv* AudioCtrl )
{
  if( Flags & AHISF_PLAY )
  {
    if( dd->v_SlaveTask != NULL )
    {
      if( dd->v_SlaveSignal != -1 )
      {
        Signal( (struct Task*) dd->v_SlaveTask,
                1L << dd->v_SlaveSignal );         // Kill him!
      }

      Wait( 1L << dd->v_MasterSignal );            // Wait for slave to die
    }

    FreeVec( dd->v_MixBuffer );
    dd->v_MixBuffer = NULL;
  }

  if(Flags & AHISF_RECORD)
  {
    // Do nothing
  }
}


/******************************************************************************
** AHIsub_GetAttr *************************************************************
******************************************************************************/

LONG __asm __saveds
intAHIsub_GetAttr( register __d0 ULONG Attribute,
                   register __d1 LONG Argument,
                   register __d2 LONG Default,
                   register __a1 struct TagItem* tagList,
                   register __a2 struct AHIAudioCtrlDrv* AudioCtrl )
{
  int i;

  switch( Attribute )
  {
    case AHIDB_Bits:
      return 32;

    case AHIDB_Frequencies:
      return FREQUENCIES;

    case AHIDB_Frequency: // Index->Frequency
      return (LONG) frequency[ Argument ];

    case AHIDB_Index: // Frequency->Index
      if( Argument <= frequency[ 0 ] )
      {
        return 0;
      }

      if( Argument >= frequency[ FREQUENCIES - 1 ] )
      {
        return FREQUENCIES-1;
      }

      for( i = 1; i < FREQUENCIES; i++ )
      {
        if( frequency[ i ] > Argument )
        {
          if( ( Argument - frequency[ i - 1 ] ) < ( frequency[ i ] - Argument ) )
          {
            return i-1;
          }
          else
          {
            return i;
          }
        }
      }

      return 0;  // Will not happen

    case AHIDB_Author:
      return (LONG) "Martin 'Leviticus' Blom";

    case AHIDB_Copyright:
      return (LONG) "Public Domain";

    case AHIDB_Version:
      return (LONG) _LibID;

    case AHIDB_Record:
      return FALSE;

    case AHIDB_Realtime:
      return TRUE;             // This is not actually true

    case AHIDB_Outputs:
      return 1;

    case AHIDB_Output:
      return (LONG) "Void";    // We have only one "output"!

    default:
      return Default;
  }
}


/******************************************************************************
** AHIsub_HardwareControl *****************************************************
******************************************************************************/

ULONG __asm __saveds
intAHIsub_HardwareControl( register __d0 ULONG attribute,
                           register __d1 LONG argument,
                           register __a2 struct AHIAudioCtrlDrv* AudioCtrl )
{
  return NULL;
}


/******************************************************************************
** AHIsub_SetVol **************************************************************
******************************************************************************/

ULONG __asm __saveds
intAHIsub_SetVol( register __d0 UWORD channel,
                  register __d1 Fixed volume,
                  register __d2 sposition pan,
                  register __a2 struct AHIAudioCtrlDrv* AudioCtrl,
                  register __d3 ULONG Flags )
{
  return AHIS_UNKNOWN;
}


/******************************************************************************
** AHIsub_SetFreq *************************************************************
******************************************************************************/

ULONG __asm __saveds
intAHIsub_SetFreq( register __d0 UWORD channel,
                   register __d1 ULONG freq,
                   register __a2 struct AHIAudioCtrlDrv* AudioCtrl,
                   register __d2 ULONG flags )
{
  return AHIS_UNKNOWN;
}


/******************************************************************************
** AHIsub_SetSound ************************************************************
******************************************************************************/

ULONG __asm __saveds
intAHIsub_SetSound( register __d0 UWORD channel,
                    register __d1 UWORD sound,
                    register __d2 ULONG offset,
                    register __d3 LONG length,
                    register __a2 struct AHIAudioCtrlDrv* AudioCtrl,
                    register __d4 ULONG flags )
{
  return AHIS_UNKNOWN;
}


/******************************************************************************
** AHIsub_SetEffect ***********************************************************
******************************************************************************/

ULONG __asm __saveds
intAHIsub_SetEffect( register __a0 APTR effect,
                     register __a2 struct AHIAudioCtrlDrv* AudioCtrl )
{
  return AHIS_UNKNOWN;
}


/******************************************************************************
** AHIsub_LoadSound ***********************************************************
******************************************************************************/

ULONG __asm __saveds
intAHIsub_LoadSound( register __d0 UWORD sound,
                     register __d1 ULONG type,
                     register __a0 APTR info,
                     register __a2 struct AHIAudioCtrlDrv* AudioCtrl )
{ 
  return AHIS_UNKNOWN;
}


/******************************************************************************
** AHIsub_UnloadSound *********************************************************
******************************************************************************/

ULONG __asm __saveds
intAHIsub_UnloadSound( register __d0 UWORD sound,
                       register __a2 struct AHIAudioCtrlDrv* AudioCtrl )
{
  return AHIS_UNKNOWN;
}



/******************************************************************************
** The slave process **********************************************************
******************************************************************************/

void __asm __saveds
SlaveTask( void )
{
  struct AHIAudioCtrlDrv* AudioCtrl;
  BOOL                    running;
  ULONG                   signals;

  AudioCtrl = (struct AHIAudioCtrlDrv*) FindTask( NULL )->tc_UserData;

  dd->v_SlaveSignal = AllocSignal( -1 );

  if( dd->v_SlaveSignal != -1 )
  {
    // Everything set up. Tell Master we're alive and healthy.

    Signal( (struct Task*) dd->v_MasterTask,
            1L << dd->v_MasterSignal );

    running = TRUE;

    while( running )
    {
      signals = SetSignal(0L,0L);

      if( signals & ( SIGBREAKF_CTRL_C | 1L << dd->v_SlaveSignal ) )
      {
        running = FALSE;
      }
      else
      {
        CallHookPkt( AudioCtrl->ahiac_PlayerFunc, AudioCtrl, NULL );
        CallHookPkt( AudioCtrl->ahiac_MixerFunc, AudioCtrl, dd->v_MixBuffer );
        
        // The mixing buffer is now filled with AudioCtrl->ahiac_BuffSamples
        // of sample frames (type AudioCtrl->ahiac_BuffType). Send them
        // to the sound card here.
      }
    }
  }

  FreeSignal( dd->v_SlaveSignal );
  dd->v_SlaveSignal = -1;

  Forbid();

  // Tell the Master we're dying

  Signal( (struct Task*) dd->v_MasterTask,
          1L << dd->v_MasterSignal );

  dd->v_SlaveTask = NULL;

  // Multitaking will resume when we are dead.
}
